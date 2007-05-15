#include "OS.h"

#ifdef _PIRATE_WINDOWS_
#include <Windows.h>
#endif
#ifdef _PIRATE_XBOX_
#include <xtl.h>
#endif

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

}