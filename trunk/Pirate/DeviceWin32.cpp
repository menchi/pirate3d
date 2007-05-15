#include "DeviceWin32.h"
#include "OS.h"
#include "pirateList.h"

namespace Pirate
{
D3D9Driver* CreateDirectX9Driver(s32 width, s32 height, HWND window,
								 u32 bits, BOOL fullscreen, BOOL stencilbuffer,
								 BOOL vsync, BOOL antiAlias, FileSystem* io);
}

struct SEnvMapper
{
	HWND hWnd;
	Pirate::DeviceWin32* PirateDev;
};

Pirate::list<SEnvMapper> g_EnvMap;

SEnvMapper* getEnvMapperFromHWnd(HWND hWnd)
{
	Pirate::list<SEnvMapper>::Iterator it = g_EnvMap.begin();
	for (; it!= g_EnvMap.end(); ++it)
	{
		if ((*it).hWnd == hWnd)
			return &(*it);
	}

	return NULL;
}

Pirate::DeviceWin32* getDeviceFromHWnd(HWND hWnd)
{
	Pirate::list<SEnvMapper>::Iterator it = g_EnvMap.begin();
	for (; it!= g_EnvMap.end(); ++it)
	{
		if ((*it).hWnd == hWnd)
			return (*it).PirateDev;
	}

	return NULL;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

	Pirate::DeviceWin32* dev = 0;
	Pirate::SEvent event;
	SEnvMapper* envm = 0;

	BYTE allKeys[256];

	static int ClickCount=0;
	if (GetCapture() != hWnd && ClickCount > 0)
		ClickCount = 0;

	switch (message)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		return 0;

	case WM_ERASEBKGND:
		return 0;

	case WM_SETCURSOR:
		envm = getEnvMapperFromHWnd(hWnd);
		if (envm &&	!envm->PirateDev->GetWin32CursorControl()->IsVisible())
		{
			SetCursor(NULL);
			return 0;
		}
		break;

	case WM_MOUSEWHEEL:
		event.EventType = Pirate::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Wheel = ((Pirate::f32)((short)HIWORD(wParam))) / (Pirate::f32)WHEEL_DELTA;
		event.MouseInput.Event = Pirate::EMIE_MOUSE_WHEEL;

		POINT p; // fixed by jox
		p.x = 0; p.y = 0;
		ClientToScreen(hWnd, &p);
		event.MouseInput.X = LOWORD(lParam) - p.x;
		event.MouseInput.Y = HIWORD(lParam) - p.y;

		dev = getDeviceFromHWnd(hWnd);
		if (dev)
			dev->PostEventFromUser(event);
		break;

	case WM_LBUTTONDOWN:
		ClickCount++;
		SetCapture(hWnd);
		event.EventType = Pirate::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = Pirate::EMIE_LMOUSE_PRESSED_DOWN;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);
		dev = getDeviceFromHWnd(hWnd);
		if (dev)
			dev->PostEventFromUser(event);
		return 0;

	case WM_LBUTTONUP:
		ClickCount--;
		if (ClickCount<1)
		{
			ClickCount=0;
			ReleaseCapture();
		}
		event.EventType = Pirate::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = Pirate::EMIE_LMOUSE_LEFT_UP;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);
		dev = getDeviceFromHWnd(hWnd);
		if (dev)
			dev->PostEventFromUser(event);
		return 0;

	case WM_RBUTTONDOWN:
		ClickCount++;
		SetCapture(hWnd);
		event.EventType = Pirate::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = Pirate::EMIE_RMOUSE_PRESSED_DOWN;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);
		dev = getDeviceFromHWnd(hWnd);
		if (dev)
			dev->PostEventFromUser(event);
		return 0;

	case WM_RBUTTONUP:
		ClickCount--;
		if (ClickCount<1)
		{
			ClickCount=0;
			ReleaseCapture();
		}
		event.EventType = Pirate::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = Pirate::EMIE_RMOUSE_LEFT_UP;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);
		dev = getDeviceFromHWnd(hWnd);
		if (dev)
			dev->PostEventFromUser(event);
		return 0;

	case WM_MBUTTONDOWN:
		ClickCount++;
		SetCapture(hWnd);
		event.EventType = Pirate::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = Pirate::EMIE_MMOUSE_PRESSED_DOWN;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);
		dev = getDeviceFromHWnd(hWnd);
		if (dev)
			dev->PostEventFromUser(event);
		return 0;

	case WM_MBUTTONUP:
		ClickCount--;
		if (ClickCount<1)
		{
			ClickCount=0;
			ReleaseCapture();
		}
		event.EventType = Pirate::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = Pirate::EMIE_MMOUSE_LEFT_UP;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);
		dev = getDeviceFromHWnd(hWnd);
		if (dev)
			dev->PostEventFromUser(event);
		return 0;

	case WM_MOUSEMOVE:
		event.EventType = Pirate::EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = Pirate::EMIE_MOUSE_MOVED;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);
		dev = getDeviceFromHWnd(hWnd);
		if (dev)
			dev->PostEventFromUser(event);
		return 0;

	case WM_KEYDOWN:
		{
			event.EventType = Pirate::EET_KEY_INPUT_EVENT;
			event.KeyInput.Key = (Pirate::EKEY_CODE)wParam;
			event.KeyInput.PressedDown = true;
			dev = getDeviceFromHWnd(hWnd);

			WORD KeyAsc=0;
			GetKeyboardState(allKeys);
			ToAscii((UINT)wParam,(UINT)lParam,allKeys,&KeyAsc,0);

			event.KeyInput.Shift = ((allKeys[VK_SHIFT] & 0x80)!=0);
			event.KeyInput.Control = ((allKeys[VK_CONTROL] & 0x80)!=0);
			event.KeyInput.Char = KeyAsc; //KeyAsc >= 0 ? KeyAsc : 0;

			if (dev)
				dev->PostEventFromUser(event);

			return 0;
		}
	case WM_KEYUP:
		{
			event.EventType = Pirate::EET_KEY_INPUT_EVENT;
			event.KeyInput.Key = (Pirate::EKEY_CODE)wParam;
			event.KeyInput.PressedDown = false;
			dev = getDeviceFromHWnd(hWnd);

			WORD KeyAsc=0;
			GetKeyboardState(allKeys);
			ToAscii((UINT)wParam,(UINT)lParam,allKeys,&KeyAsc,0);

			event.KeyInput.Shift = ((allKeys[VK_SHIFT] & 0x80)!=0);
			event.KeyInput.Control = ((allKeys[VK_CONTROL] & 0x80)!=0);
			event.KeyInput.Char = KeyAsc; //KeyAsc >= 0 ? KeyAsc : 0;

			if (dev)
				dev->PostEventFromUser(event);

			return 0;
		}

	case WM_SIZE:
		{
			// resize
			dev = getDeviceFromHWnd(hWnd);
			if (dev)
				dev->OnResized();
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SYSCOMMAND:
		// prevent screensaver or monitor powersave mode from starting
		if ((wParam & 0xFFF0) == SC_SCREENSAVE ||
			(wParam & 0xFFF0) == SC_MONITORPOWER)
			return 0;
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

namespace Pirate
{

DeviceWin32::DeviceWin32(s32 Width, s32 Height, BOOL fullscreen, BOOL stencilbuffer, BOOL vsync, IEventReceiver* recv)
: m_HWnd(0), m_bChangedToFullScreen(FALSE), m_bFullScreen(fullscreen), m_bResized(FALSE), m_pUserReceiver(recv),
  m_bExternalWindow(FALSE), m_pVideoDriver(NULL), m_pLogger(new Logger(m_pUserReceiver)), m_pFileSystem(new FileSystem)
{
#ifdef _DEBUG
	SetDebugName("DeviceWin32");
#endif

	Printer::pLogger = m_pLogger;
	stringc s = "Pirate Engine version ";
	s.append(GetVersion());
	Printer::Log(s.c_str(), ELL_NONE);

	stringc winversion;
	GetWindowsVersion(winversion);
	m_pOSOperator = new OSOperator(winversion.c_str());
	Printer::Log(winversion.c_str(), ELL_INFORMATION);

	HINSTANCE hInstance = GetModuleHandle(0);

	// create the window, only if we do not use the null device
	HWND externalWindow = 0;
	if (externalWindow==0)
	{
		const wchar_t* ClassName = L"DeviceWin32";

		// Register Class
		WNDCLASSEX wcex;
		wcex.cbSize		= sizeof(WNDCLASSEX);
		wcex.style		= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= (WNDPROC)WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon		= NULL;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= 0;
		wcex.lpszClassName	= ClassName;
		wcex.hIconSm		= 0;

		// if there is an icon, load it
		wcex.hIcon = (HICON)LoadImage(hInstance, L"Pirate3D.ico", IMAGE_ICON, 0,0, LR_LOADFROMFILE); 

		RegisterClassEx(&wcex);

		// calculate client size

		RECT clientSize;
		clientSize.top = 0;
		clientSize.left = 0;
		clientSize.right = Width;
		clientSize.bottom = Height;

		DWORD style = WS_POPUP;

		if (!fullscreen)
			style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

		AdjustWindowRect(&clientSize, style, FALSE);

		int realWidth = clientSize.right - clientSize.left;
		int realHeight = clientSize.bottom - clientSize.top;

		int windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
		int windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

		if (fullscreen)
		{
			windowLeft = 0;
			windowTop = 0;
		}

		// create window

		m_HWnd = CreateWindow( ClassName, L"", style, windowLeft, windowTop,
			realWidth, realHeight,	NULL, NULL, hInstance, NULL);

		ShowWindow(m_HWnd , SW_SHOW);
		UpdateWindow(m_HWnd);

		// fix ugly ATI driver bugs. Thanks to ariaci
		MoveWindow(m_HWnd, windowLeft, windowTop, realWidth, realHeight, TRUE);
	}

	// attach external window
	if (externalWindow)
	{
		m_HWnd = externalWindow;
		RECT r;
		GetWindowRect(m_HWnd, &r);
		Width = r.right - r.left;
		Height = r.bottom - r.top;
		fullscreen = FALSE;
		m_bExternalWindow = TRUE;
	}

	// create cursor control
	m_pWin32CursorControl = new CursorControl(m_HWnd);

	// create driver
	CreateDriver(Width, Height, 32, fullscreen, stencilbuffer, vsync, FALSE);
/*
	if (VideoDriver)
		createGUIAndScene();
*/
	// register environment
	SEnvMapper em;
	em.PirateDev = this;
	em.hWnd = m_HWnd;
	g_EnvMap.push_back(em);

	// set this as active window
	SetActiveWindow(m_HWnd);
	SetForegroundWindow(m_HWnd);
}

DeviceWin32::~DeviceWin32()
{
	list<SEnvMapper>::Iterator it = g_EnvMap.begin();
	for (; it!= g_EnvMap.end(); ++it)
	{
		if ((*it).hWnd == m_HWnd)
		{
			g_EnvMap.erase(it);
			break;
		}
	}
	if (m_bChangedToFullScreen)
		ChangeDisplaySettings(NULL,0);
	delete m_pWin32CursorControl;

	m_pVideoDriver->Drop();
	m_pOSOperator->Drop();
	m_pFileSystem->Drop();
	m_pLogger->Drop();
}

//! returns the video driver
D3D9Driver* DeviceWin32::GetVideoDriver()
{
	return m_pVideoDriver;
}

BOOL DeviceWin32::Run()
{
	MSG msg;
	BOOL quit = FALSE;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);

		if (m_bExternalWindow && msg.hwnd == m_HWnd)
			WndProc(m_HWnd, msg.message, msg.wParam, msg.lParam);
		else
			DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
			quit = TRUE;
	}
/*
	if (!quit)
		resizeIfNecessary();
*/
	return !quit;
}

//! Returns the version of the engine. 
const char* DeviceWin32::GetVersion()
{
	return PIRATE_SDK_VERSION;
}

void DeviceWin32::GetWindowsVersion(stringc& out)
{
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if(!(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO*) &osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) )
			return;
	}

	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		if (osvi.dwMajorVersion <= 4)
			out.append("Microsoft Windows NT ");
		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
			out.append("Microsoft Windows 2000 ");
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
			out.append("Microsoft Windows XP ");

		if( bOsVersionInfoEx )
		{
#ifdef VER_SUITE_ENTERPRISE
			if (osvi.wProductType == VER_NT_WORKSTATION)
			{
				if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
					out.append("Personal ");
				else
					out.append("Professional ");
			}
			else if (osvi.wProductType == VER_NT_SERVER)
			{
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					out.append("DataCenter Server ");
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					out.append("Advanced Server ");
				else
					out.append("Server ");
			}
#endif
		}
		else
		{
			HKEY hKey;
			char szProductType[80];
			DWORD dwBufLen;

			RegOpenKeyExA( HKEY_LOCAL_MACHINE,
				"SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
				0, KEY_QUERY_VALUE, &hKey );
			RegQueryValueExA( hKey, "ProductType", NULL, NULL,
				(LPBYTE) szProductType, &dwBufLen);
			RegCloseKey( hKey );

			if (lstrcmpiA( "WINNT", szProductType) == 0 )
				out.append("Professional ");
			if ( lstrcmpiA( "LANMANNT", szProductType) == 0 )
				out.append("Server " );
			if ( lstrcmpiA( "SERVERNT", szProductType) == 0 )
				out.append("Advanced Server ");
		}

		// Display version, service pack (if any), and build number.

		wchar_t tmp[255];

		if (osvi.dwMajorVersion <= 4 )
		{
			wsprintf (tmp, L"version %ld.%ld %s (Build %ld)",
				osvi.dwMajorVersion,
				osvi.dwMinorVersion,
				osvi.szCSDVersion,
				osvi.dwBuildNumber & 0xFFFF);
		}
		else
		{
			wsprintf (tmp, L"%s (Build %ld)", osvi.szCSDVersion,
				osvi.dwBuildNumber & 0xFFFF);
		}

		out.append(tmp);
		break;

	case VER_PLATFORM_WIN32_WINDOWS:

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			out.append("Microsoft Windows 95 ");
			if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
				out.append("OSR2 " );
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			out.append("Microsoft Windows 98 ");
			if ( osvi.szCSDVersion[1] == 'A' )
				out.append( "SE " );
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
			out.append("Microsoft Windows Me ");

		break;

	case VER_PLATFORM_WIN32s:

		out.append("Microsoft Win32s ");
		break;
	}
}

//! send the event to the right receiver
void DeviceWin32::PostEventFromUser(SEvent event)
{
	BOOL absorbed = FALSE;

	if (m_pUserReceiver)
		absorbed = m_pUserReceiver->OnEvent(event);
/*
	if (!absorbed && GUIEnvironment)
		absorbed = GUIEnvironment->postEventFromUser(event);

	if (!absorbed && SceneManager)
		absorbed = SceneManager->postEventFromUser(event);*/
}

//! Notifies the device, that it has been resized
void DeviceWin32::OnResized()
{
	m_bResized = TRUE;
}

//! return file system
FileSystem* DeviceWin32::GetFileSystem()
{
	return m_pFileSystem;
}

void DeviceWin32::CreateDriver(s32 width, s32 height, u32 bits, BOOL fullscreen, BOOL stencilbuffer, BOOL vsync, BOOL antiAlias)
{
	m_pVideoDriver = CreateDirectX9Driver(width, height, m_HWnd, bits, fullscreen, stencilbuffer, vsync, antiAlias, m_pFileSystem);
	if (!m_pVideoDriver)
	{
		Printer::Log("Could not create DIRECT3D9 Driver.", ELL_ERROR);
	}
}

DeviceWin32::CursorControl* DeviceWin32::GetWin32CursorControl()
{
	return m_pWin32CursorControl;
}

}