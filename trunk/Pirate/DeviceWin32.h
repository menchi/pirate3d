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
class SceneManager;

class DeviceWin32 : public virtual RefObject
{
public:
	DeviceWin32(s32 Width, s32 Height, BOOL fullscreen, BOOL stencilbuffer, BOOL vsync, IEventReceiver* recv);
	~DeviceWin32();

	//! returns the video driver
	D3D9Driver* GetVideoDriver();

	//! runs the device. Returns false if device wants to be deleted
	BOOL Run();

	//! Returns the version of the engine.
	const char* GetVersion();

	//! send the event to the right receiver
	void PostEventFromUser(SEvent event);
	void OnResized();

	//! return file system
	FileSystem* DeviceWin32::GetFileSystem();

	//! returns the scene manager
	SceneManager* GetSceneManager();

	//! Sets a new event receiver to receive events
	void SetEventReceiver(IEventReceiver* receiver);

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

	void GetWindowsVersion(stringc& out);

	void CreateGUIAndScene();

	HWND m_HWnd;
	D3D9Driver* m_pVideoDriver;

	FileSystem* m_pFileSystem;
	IEventReceiver* m_pUserReceiver;
	Logger* m_pLogger;
	OSOperator* m_pOSOperator;

	SceneManager* m_pSceneManager;

	BOOL m_bChangedToFullScreen;
	BOOL m_bFullScreen;
	BOOL m_bResized;
	BOOL m_bExternalWindow;
	CursorControl* m_pWin32CursorControl;
};

SceneManager* CreateSceneManager(D3D9Driver* driver, FileSystem* fs, DeviceWin32::CursorControl* cc);

}

#endif
