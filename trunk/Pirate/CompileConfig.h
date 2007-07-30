#ifndef _PIRATE_COMPILE_CONFIG_H_INCLUDED_
#define _PIRATE_COMPILE_CONFIG_H_INCLUDED_

//! Pirate SDK Version
#define PIRATE_SDK_VERSION "BABY"

//! The defines for different operating system are:

#if defined(_XBOX)
#define _PIRATE_XBOX_
#elif defined(WIN32) || defined(WIN64)
#define _PIRATE_WINDOWS_
#endif

#if defined(_PIRATE_XBOX_)
#define _PIRATE_BIG_ENDIAN_
#endif

#if defined(_PIRATE_WINDOWS_)

#define _PIRATE_COMPILE_WITH_DIRECT3D_9_PC_

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment( lib, "d3dx9" )

#elif defined(_PIRATE_XBOX_)

#define _PIRATE_COMPILE_WITH_DIRECT3D_9_XBOX_

#include <xtl.h>

#endif

#ifdef _PIRATE_WINDOWS_

#ifndef _PIRATE_STATIC_LIB_
#ifdef PIRATE_EXPORTS
#define PIRATE_API __declspec(dllexport)
#else
#define PIRATE_API __declspec(dllimport)
#endif // IRRLICHT_EXPORT
#else
#define PIRATE_API
#endif // _IRR_STATIC_LIB_

// Declare the calling convention.
#if defined(_STDCALL_SUPPORTED)
#define PIRATECALLCONV __stdcall
#else
#define PIRATECALLCONV __cdecl
#endif // STDCALL_SUPPORTED

#else
#define PIRATE_API 
#define PIRATECALLCONV
#endif // _IRR_WINDOWS_

#endif // __IRR_COMPILE_CONFIG_H_INCLUDED__