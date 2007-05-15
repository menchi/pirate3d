#ifndef _PIRATE_OS_H_
#define _PIRATE_OS_H_

#include "CompileConfig.h"
#include "pirateTypes.h"
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

}

#endif