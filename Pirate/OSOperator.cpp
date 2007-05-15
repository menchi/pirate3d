#include "OSOperator.h"
#include "CompileConfig.h"

#ifdef _PIRATE_WINDOWS_
#include <windows.h>
#endif
#ifdef _PIRATE_XBOX_
#include <xtl.h>
#endif

namespace Pirate
{

// constructor
OSOperator::OSOperator(const c8* osVersion)
{
	OperationSystem = osVersion;
}


//! destructor
OSOperator::~OSOperator()
{
}


//! returns the current operation system version as string.
const wchar_t* OSOperator::GetOperationSystemVersion()
{
	return OperationSystem.c_str();
}


BOOL OSOperator::GetProcessorSpeedMHz(u32* MHz)
{
#if defined(_PIRATE_WINDOWS_)
	LONG Error;

	HKEY Key;
	Error = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
		0, KEY_READ, &Key);

	if(Error != ERROR_SUCCESS)
		return false;

	DWORD Speed = 0;
	DWORD Size = sizeof(Speed);
	Error = RegQueryValueEx(Key, L"~MHz", NULL, NULL, (LPBYTE)&Speed, &Size);

	RegCloseKey(Key);

	if (Error != ERROR_SUCCESS)
		return FALSE;
	else if (MHz)
		*MHz = Speed;
	return TRUE;
#else
	// could probably be read from "/proc/cpuinfo" or "/proc/cpufreq"
	return FALSE;
#endif
}

BOOL OSOperator::GetSystemMemory(u32* Total, u32* Avail)
{
#if defined(_PIRATE_WINDOWS_)
	MEMORYSTATUS MemoryStatus;
	MemoryStatus.dwLength = sizeof(MEMORYSTATUS);

	// cannot fail
	GlobalMemoryStatus(&MemoryStatus);

	if (Total)
		*Total = (u32)(MemoryStatus.dwTotalPhys>>10);
	if (Avail)
		*Avail = (u32)(MemoryStatus.dwAvailPhys>>10);

	return TRUE;
#endif

	return FALSE;
}


} // end namespace