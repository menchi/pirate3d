#include "DeviceWin32.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

DeviceWin32::DeviceWin32(int width, int height)
{
	HINSTANCE hInstance = GetModuleHandle(0);

	const wchar_t* ClassName = L"DeviceWin32";

	// Register Class
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
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
	clientSize.right = width;
	clientSize.bottom = height;

	DWORD style = WS_POPUP;

	style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	AdjustWindowRect(&clientSize, style, FALSE);

	int realWidth = clientSize.right - clientSize.left;
	int realHeight = clientSize.bottom - clientSize.top;
	int windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
	int windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

	// create window
	HWND m_HWnd = CreateWindow( ClassName, L"", style, windowLeft, windowTop,
		realWidth, realHeight,	NULL, NULL, hInstance, NULL);

	ShowWindow(m_HWnd , SW_SHOW);
	UpdateWindow(m_HWnd);
}

DeviceWin32::~DeviceWin32()
{
}

bool DeviceWin32::Run()
{
	MSG msg;
	bool quit = false;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
			quit = true;
	}

	return !quit;
}