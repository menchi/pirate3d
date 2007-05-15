#ifndef _PIRATE_BIG_PIRATE_H_
#define _PIRATE_BIG_PIRATE_H_

#include "D3D9DriverWin32.h"
#include "OSOperator.h"
#include "IEventReceiver.h"
#include "FileSystem.h"
#include <Windows.h>

namespace Pirate
{

class Logger;
class IEventReceiver;

class DeviceWin32 : public virtual RefObject
{
public:
	DeviceWin32(s32 Width, s32 Height, BOOL fullscreen, BOOL stencilbuffer, BOOL vsync, IEventReceiver* recv);
	~DeviceWin32();

	D3D9Driver* GetVideoDriver();
	BOOL Run();
	const char* GetVersion();
	void GetWindowsVersion(stringc& out);
	void PostEventFromUser(SEvent event);
	void OnResized();

	//! return file system
	FileSystem* DeviceWin32::GetFileSystem();

	class CursorControl
	{
	public:

		CursorControl(HWND hwnd) : m_bIsVisible(TRUE), m_HWnd(hwnd)
		{
		}

		//! Changes the visible state of the mouse cursor.
		void SetVisible(BOOL visible)
		{
			m_bIsVisible = visible;
		}

		//! Returns if the cursor is currently visible.
		BOOL IsVisible()
		{
			return m_bIsVisible;
		}

	private:
		BOOL m_bIsVisible;
		HWND m_HWnd;
	};


	//! returns the win32 cursor control
	CursorControl* GetWin32CursorControl();

private:
	void CreateDriver(s32 width, s32 height, u32 bits, BOOL fullscreen, BOOL stencilbuffer, BOOL vsync, BOOL antiAlias);

	HWND m_HWnd;
	D3D9Driver* m_pVideoDriver;

	FileSystem* m_pFileSystem;
	IEventReceiver* m_pUserReceiver;
	Logger* m_pLogger;
	OSOperator* m_pOSOperator;

	BOOL m_bChangedToFullScreen;
	BOOL m_bFullScreen;
	BOOL m_bResized;
	BOOL m_bExternalWindow;
	CursorControl* m_pWin32CursorControl;
};

}

#endif
