#include "OS.h"

namespace Pirate
{
#define bswap_16(X) _byteswap_ushort(X)
#define bswap_32(X) _byteswap_ulong(X)

u16 Byteswap::ByteSwap(u16 num) {return bswap_16(num);}
s16 Byteswap::ByteSwap(s16 num) {return bswap_16(num);}
u32 Byteswap::ByteSwap(u32 num) {return bswap_32(num);}
s32 Byteswap::ByteSwap(s32 num) {return bswap_32(num);}
f32 Byteswap::ByteSwap(f32 num) {u32 tmp=bswap_32(*((u32*)&num)); return *((f32*)&tmp);}

Logger* Printer::pLogger = NULL;

void Printer::Print(const c8* message)
{
	c8* tmp = new c8[strlen(message) + 2];
	sprintf_s(tmp, strlen(message) + 2, "%s\n", message);
	OutputDebugStringA(tmp);
	printf(tmp);
	delete [] tmp;
}

void Printer::Log(const c8* message, ELOG_LEVEL ll)
{
	if (pLogger)
		pLogger->Log(message, ll);
}

void Printer::Log(const c8* message, const c8* hint, ELOG_LEVEL ll)
{
	if (!pLogger)
		return;

	pLogger->Log(message, hint, ll);
}

void Printer::Log(const wchar_t* message, ELOG_LEVEL ll)
{
	if (pLogger)
		pLogger->Log(message, ll);
}

// ----------------------------------------------------------------
// Windows specific functions
// ----------------------------------------------------------------

LARGE_INTEGER HighPerformanceFreq;
BOOL HighPerformanceTimerSupport = FALSE;


void Timer::InitTimer()
{
	HighPerformanceTimerSupport = QueryPerformanceFrequency(&HighPerformanceFreq);
	InitVirtualTimer();
}

u32 Timer::GetRealTime()
{
	if (HighPerformanceTimerSupport)
	{
		LARGE_INTEGER nTime;
		QueryPerformanceCounter(&nTime);
		return u32((nTime.QuadPart) * 1000 / HighPerformanceFreq.QuadPart);
	}
	return GetTickCount();
}

// ------------------------------------------------------
// virtual timer implementation

f32 Timer::VirtualTimerSpeed = 1.0f;
s32 Timer::VirtualTimerStopCounter = 0;
u32 Timer::LastVirtualTime = 0;
u32 Timer::StartRealTime = 0;
u32 Timer::StaticTime = 0;

//! returns current virtual time
u32 Timer::GetTime()
{
	if (IsStopped())
		return LastVirtualTime;

	return LastVirtualTime + (u32)((StaticTime - StartRealTime) * VirtualTimerSpeed);
}

//! ticks, advances the virtual timer
void Timer::Tick()
{
	StaticTime = GetRealTime();
}

//! sets the current virtual time
void Timer::SetTime(u32 time)
{
	StaticTime = GetRealTime();
	LastVirtualTime = time;
	StartRealTime = StaticTime;
}

//! stops the virtual timer
void Timer::StopTimer()
{
	if (!IsStopped())
	{
		// stop the virtual timer
		LastVirtualTime = GetTime();
	}

	--VirtualTimerStopCounter;
}

//! starts the virtual timer
void Timer::StartTimer()
{
	++VirtualTimerStopCounter;

	if (!IsStopped())
	{
		// restart virtual timer
		SetTime(LastVirtualTime);
	}
}

//! sets the speed of the virtual timer
void Timer::SetSpeed(f32 speed)
{
	SetTime(GetTime());

	VirtualTimerSpeed = speed;
	if (VirtualTimerSpeed < 0.0f)
		VirtualTimerSpeed = 0.0f;
}

//! gets the speed of the virtual timer
f32 Timer::GetSpeed()
{
	return VirtualTimerSpeed;
}

//! returns if the timer currently is stopped
BOOL Timer::IsStopped()
{
	return VirtualTimerStopCounter != 0;
}

void Timer::InitVirtualTimer()
{
	StaticTime = GetRealTime();
	StartRealTime = StaticTime;
}

}