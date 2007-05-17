#ifndef _PIRATE_BIG_PIRATE_H_
#define _PIRATE_BIG_PIRATE_H_

#include "IEventReceiver.h"
#include "position2d.h"

namespace Pirate
{

class D3D9Driver;
class Logger;
class SceneManager;
class FileSystem;
class OSOperator;

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

	//! notifies the device that it should close itself
	void CloseDevice();

	//! sets the caption of the window
	void SetWindowCaption(const wchar_t* text);

	//! Implementation of the win32 cursor control
	class CursorControl : public virtual RefObject
	{
	public:

		CursorControl(const dimension2d<s32>& wsize, HWND hwnd, BOOL fullscreen)
			: m_WindowSize(wsize), m_InvWindowSize(0.0f, 0.0f), m_bIsVisible(TRUE),
			m_HWnd(hwnd), m_iBorderX(0), m_iBorderY(0)
		{
			if (m_WindowSize.Width!=0)
				m_InvWindowSize.Width = 1.0f / m_WindowSize.Width;

			if (m_WindowSize.Height!=0)
				m_InvWindowSize.Height = 1.0f / m_WindowSize.Height;

			if (!fullscreen)
			{
				m_iBorderX = GetSystemMetrics(SM_CXDLGFRAME);
				m_iBorderY = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYDLGFRAME);
			}
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

		//! Sets the new position of the cursor.
		void SetPosition(const position2d<f32> &pos)
		{
			SetPosition(pos.X, pos.Y);
		}

		//! Sets the new position of the cursor.
		void SetPosition(f32 x, f32 y)
		{
			SetPosition((s32)(x*m_WindowSize.Width), (s32)(y*m_WindowSize.Height));
		}

		//! Sets the new position of the cursor.
		void SetPosition(const position2d<s32> &pos)
		{
			SetPosition(pos.X, pos.Y);
		}

		//! Sets the new position of the cursor.
		void SetPosition(s32 x, s32 y)
		{
			RECT rect;
			if (GetWindowRect(m_HWnd, &rect))
				SetCursorPos(x + rect.left + m_iBorderX, y + rect.top + m_iBorderY);

			m_CursorPos.X = x;
			m_CursorPos.Y = y;
		}

		//! Returns the current position of the mouse cursor.
		position2d<s32> GetPosition()
		{
			UpdateInternalCursorPosition();
			return m_CursorPos;
		}

		//! Returns the current position of the mouse cursor.
		position2d<f32> GetRelativePosition()
		{
			UpdateInternalCursorPosition();
			return position2d<f32>(m_CursorPos.X * m_InvWindowSize.Width,
				m_CursorPos.Y * m_InvWindowSize.Height);
		}

	private:

		//! Updates the internal cursor position
		void UpdateInternalCursorPosition()
		{
			POINT p;
			GetCursorPos(&p);
			RECT rect;
			GetWindowRect(m_HWnd, &rect);
			m_CursorPos.X = p.x-rect.left-m_iBorderX;
			m_CursorPos.Y = p.y-rect.top-m_iBorderY;
		}

		position2d<s32> m_CursorPos;
		dimension2d<s32> m_WindowSize;
		dimension2d<f32> m_InvWindowSize;
		BOOL m_bIsVisible;
		HWND m_HWnd;

		s32 m_iBorderX, m_iBorderY;
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
