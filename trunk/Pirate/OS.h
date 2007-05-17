#ifndef _PIRATE_OS_H_
#define _PIRATE_OS_H_

#include "Logger.h"

namespace Pirate
{

class Byteswap
{
public:
	static u16 ByteSwap(u16 num);
	static s16 ByteSwap(s16 num);
	static u32 ByteSwap(u32 num);
	static s32 ByteSwap(s32 num);
	static f32 ByteSwap(f32 num);
};

class Logger;

class Printer
{
public:
	// prints out a string to the console out stdout or debug log or whatever
	static void Print(const c8* message);
	static void Log(const c8* message, ELOG_LEVEL ll = ELL_INFORMATION);
	static void Log(const c8* message, const c8* hint, ELOG_LEVEL ll = ELL_INFORMATION);
	static void Log(const wchar_t* message, ELOG_LEVEL ll = ELL_INFORMATION);
	static Logger* pLogger;
};

class Timer
{
public:

	//! returns the current time in milliseconds
	static u32 GetTime();

	//! initializes the real timer
	static void InitTimer();

	//! sets the current virtual (game) time
	static void SetTime(u32 time);

	//! stops the virtual (game) timer
	static void StopTimer();

	//! starts the game timer
	static void StartTimer();

	//! sets the speed of the virtual timer
	static void SetSpeed(f32 speed);

	//! gets the speed of the virtual timer
	static f32 GetSpeed();

	//! returns if the timer currently is stopped
	static BOOL IsStopped();

	//! makes the virtual timer update the time value based on the real time
	static void Tick();

	//! returns the current real time in milliseconds
	static u32 GetRealTime();

private:

	static void InitVirtualTimer();

	static f32 VirtualTimerSpeed;
	static s32 VirtualTimerStopCounter;
	static u32 StartRealTime;
	static u32 LastVirtualTime;
	static u32 StaticTime;
};

}

#endif