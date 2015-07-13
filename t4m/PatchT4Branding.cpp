#include "StdInc.h"

const char* SetConsoleVersion();
const char* SetVersion();

void PatchT4_Branding()
{
	/*
	Useful addresses
	buildDateTime	0x8711E4
	buildGame		0x871228
	buildUser		0x871200
	buildNumber		0x59D35C (hex), PatchMemory doesn't like it :(
	buildNumber2	0x8D8D5C 0x59CECB why are there multiple instances (still more), none of these are correct. (end address +1 to not break the push opcode) Just replace in exe....
	consoleInfo		0x851208 (1.7.[buildNumber] T4 >)
	'7'				0x851200 (versionInfo)
	version check	0x5FD91B
	gameName		0x871228
	*/
	//TODO: Replace shortversion DVars and other version related locations
	nop(0x5FD91B, 5);										// disable pc_newversionavailable check
	PatchMemory(0x851208, (PBYTE)CONSOLEVERSION_STR, 14);	// change the console input version
	//PatchMemory(0x59D430, (PBYTE)SHORTVERSION_STR, 4);
	Detours::X86::DetourFunction((PBYTE)0x4743D2, (PBYTE)&SetConsoleVersion, Detours::X86Option::USE_CALL); // change the version info of console window
	Detours::X86::DetourFunction((PBYTE)0x59D393, (PBYTE)&SetConsoleVersion, Detours::X86Option::USE_CALL); // change the version info of version dvar
}

const char* SetConsoleVersion()
{
	return va("Call of Duty %s", VERSION_STR);
}