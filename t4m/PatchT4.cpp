// ==========================================================
// T4M project
// 
// Component: clientdll
// Purpose: World at War patches
//
// Initial author: UNKNOWN
// Started: 2015-07-08
// ==========================================================

#include <string>
#include "StdInc.h"

void loadGameOverlay();
void PatchT4();
void PatchT4_MemoryLimits();
void PatchT4_Branding();
void PatchT4_Console();
void PatchT4_Dvars();
void PatchT4_NoBorder();
void PatchT4_PreLoad();
void PatchT4_SteamDRM();
void PatchT4_FileDebug();
using namespace std;


void Sys_RunInit()
{
	PatchT4();
}

void PatchT4()
{
	// ↓ 这个造成服务器验证失败？
	//PatchT4_SteamDRM();
	PatchT4_MemoryLimits();
	PatchT4_Branding();
	PatchT4_Console();
	PatchT4_Dvars();
	PatchT4_NoBorder();
	PatchT4_PreLoad();
	//改个标题压压惊
	*(DWORD*)0x6D65A0 = (DWORD)"Call of Duty: World at War Campaign/Coop - T4M MHD4 Editon";
	//PatchT4_FileDebug();

	//加载Sbeam Overlay
	// Check if game got started using steam
	if (!GetModuleHandle("gameoverlayrenderer.dll"))
		loadGameOverlay();
}

void PatchT4_PreLoad()
{
	//这里是去掉那几个框
	// ↓ 导致我分辨率和抗狗牙一直变回默认的罪魁祸首,T4M将其去掉却导致直接帮我点了是(cao)
	//nop(0x5FE685, 5); // remove optimal settings popup
	//
	*(BYTE*)0x5FF386 = (BYTE)0xEB; // skip safe mode check
}

void PatchT4_SteamDRM()
{
	// Replace encrypted .text segment
	DWORD size = 0x3EA000;
	std::string data = GetBinaryResource(IDB_TEXT);
	uncompress((unsigned char*)0x401000, &size, (unsigned char*)data.data(), data.size());

	// Apply new entry point
	HMODULE hModule = GetModuleHandle(NULL);
	PIMAGE_DOS_HEADER header = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((DWORD)hModule + header->e_lfanew);
	ntHeader->OptionalHeader.AddressOfEntryPoint = 0x3AF316;
}

//code from https://github.com/momo5502/cod-mod/
void loadGameOverlay()
{
	try
	{
		std::string m_steamDir;
		HKEY hRegKey;

		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Valve\\Steam", 0, KEY_QUERY_VALUE, &hRegKey) == ERROR_SUCCESS)
		{
			char pchSteamDir[MAX_PATH];
			DWORD dwLength = sizeof(pchSteamDir);
			RegQueryValueExA(hRegKey, "InstallPath", NULL, NULL, (BYTE*)pchSteamDir, &dwLength);
			RegCloseKey(hRegKey);

			m_steamDir = pchSteamDir;
		}

		//Com_Printf(0, "Loading %s\\gameoverlayrenderer.dll...\n", m_steamDir.c_str());
		HMODULE overlay = LoadLibrary(va("%s\\gameoverlayrenderer.dll", m_steamDir.c_str()));

		if (overlay)
		{
			FARPROC _SetNotificationPosition = GetProcAddress(overlay, "SetNotificationPosition");

			if (_SetNotificationPosition)
			{
				((void(*)(uint32_t))_SetNotificationPosition)(1);
			}
		}
	}
	catch (int e)
	{
		//Com_Printf(0, "Failed to inject Steam's gameoverlay: %d", e);
	}
}