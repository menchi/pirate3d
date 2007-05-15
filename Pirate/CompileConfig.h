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

#elif defined(_PIRATE_XBOX_)

#define _PIRATE_COMPILE_WITH_DIRECT3D_9_XBOX_

#endif

//! Define _PIRATE_COMPILE_WITH_ZLIB_ to enable compiling the engine using zlib.
/** This enables the engine to read from compressed .zip archives. If you
disable this feature, the engine can still read archives, but only uncompressed
ones. */
#define _PIRATE_COMPILE_WITH_ZLIB_

//! Define _PIRATE_USE_NON_SYSTEM_ZLIB_ to let irrlicht use the zlib which comes with irrlicht.
/** If this is commented out, Irrlicht will try to compile using the zlib installed in the system.
This is only used when _IRR_COMPILE_WITH_ZLIB_ is defined. */
#define _PIRATE_USE_NON_SYSTEM_ZLIB_


//! Define _PIRATE_COMPILE_WITH_LIBJPEG_ to enable compiling the engine using libjpeg.
/** This enables the engine to read jpeg images. If you comment this out,
the engine will no longer read .jpeg images. */
#define _PIRATE_COMPILE_WITH_LIBJPEG_

//! Define _PIRATE_USE_NON_SYSTEM_JPEG_LIB_ to let irrlicht use the jpeglib which comes with irrlicht.
/** If this is commented out, Irrlicht will try to compile using the jpeg lib installed in the system.
This is only used when _IRR_COMPILE_WITH_LIBJPEG_ is defined. */
#define _PIRATE_USE_NON_SYSTEM_JPEG_LIB_


//! Define _PIRATE_COMPILE_WITH_LIBPNG_ to enable compiling the engine using libpng.
/** This enables the engine to read png images. If you comment this out,
the engine will no longer read .png images. */
#define _PIRATE_COMPILE_WITH_LIBPNG_

//! Define _PIRATE_USE_NON_SYSTEM_LIB_PNG_ to let irrlicht use the libpng which comes with irrlicht.
/** If this is commented out, Irrlicht will try to compile using the libpng installed in the system.
This is only used when _IRR_COMPILE_WITH_LIBPNG_ is defined. */
#define _PIRATE_USE_NON_SYSTEM_LIB_PNG_


//! Define _PIRATE_D3D_NO_SHADER_DEBUGGING_ to disable shader debugging in D3D9
/** If _PIRATE_D3D_NO_SHADER_DEBUGGING_ is undefined in CompileConfig.h,
it is possible to debug all D3D9 shaders in VisualStudio. All shaders 
(which have been generated in memory or read from archives for example) will be emitted
into a temporary file at runtime for this purpose. To debug your shaders, choose  
Debug->Direct3D->StartWithDirect3DDebugging in Visual Studio, and for every shader a
file named 'irr_dbg_shader_%%.vsh' or 'irr_dbg_shader_%%.psh' will be created. Drag'n'drop
the file you want to debug into visual studio. That's it. You can now set breakpoints and
watch registers, variables etc. This works with ASM, HLSL, and both with pixel and vertex shaders.
Note that the engine will run in D3D REF for this, which is a lot slower than HAL. */
#define _PIRATE_D3D_NO_SHADER_DEBUGGING_


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