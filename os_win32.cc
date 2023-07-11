#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>

#include "common.h"

#include <cstdio> // TODO: For debugging, remove later.

static HWND hwnd;
static HHOOK hook;

Pixel GetPixel(int x, int y) {
	auto desktop = GetShellWindow();
	auto hdc = GetDC(desktop);
	Pixel c = (Pixel) GetPixel(hdc, x, y);
	ReleaseDC(desktop, hdc);
	return c;
}

void UpdateWindow() {
	HDC hdc = GetDC(hwnd);
	HDC screen_dc = CreateCompatibleDC(hdc);
	HBITMAP screen_bitmap = CreateBitmap(SCREEN_WIDTH, SCREEN_HEIGHT, 1, 32, screen);
	
	SelectObject(screen_dc, screen_bitmap);
	BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, screen_dc, 0, 0, SRCCOPY);
	DeleteDC(screen_dc);
	ReleaseDC(hwnd, hdc);
}

void ToClipboard(const char* s) {}

static bool grab_mode = false;

Event GetGrabEvent() {
	Event event = EVENT_NULL;
	MSG msg = {};
	
	PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE);

	switch (msg.message) {
	case WM_LBUTTONDOWN: {
		mouse_buttons[0] = true;
		pointer.x = GET_X_LPARAM(msg.lParam); 
		pointer.y = GET_Y_LPARAM(msg.lParam);
		event = EVENT_MOUSE_BUTTON;
	} break;
	case WM_LBUTTONUP: {
		mouse_buttons[0] = false;
		pointer.x = GET_X_LPARAM(msg.lParam); 
		pointer.y = GET_Y_LPARAM(msg.lParam);
		event = EVENT_MOUSE_BUTTON;
	} break;
	case WM_RBUTTONDOWN: {
		mouse_buttons[1] = true;
		pointer.x = GET_X_LPARAM(msg.lParam); 
		pointer.y = GET_Y_LPARAM(msg.lParam);
		event = EVENT_MOUSE_BUTTON;
	} break;
	case WM_RBUTTONUP: {
		mouse_buttons[1] = false;
		pointer.x = GET_X_LPARAM(msg.lParam); 
		pointer.y = GET_Y_LPARAM(msg.lParam);
		event = EVENT_MOUSE_BUTTON;
	} break;
	default: {
		/*
		POINT pos;
		GetCursorPos(&pos);
		pointer.x = pos.x;
		pointer.y = pos.y;
		event = EVENT_MOUSE_MOVE;
		*/
	} break;
	}

	return event;
}

Event GetEvent() {
	Event event = EVENT_NULL;
	MSG msg = {};

	if (grab_mode)
		return GetGrabEvent();
	
	if (GetMessage(&msg, NULL, 0, 0) <= 0) {
		return EVENT_QUIT;
	}

	switch (msg.message) {
	case WM_MOUSEMOVE: {
		pointer.x = GET_X_LPARAM(msg.lParam); 
		pointer.y = GET_Y_LPARAM(msg.lParam);
		event = EVENT_MOUSE_MOVE;
	} break;

	case WM_LBUTTONDOWN: {
		mouse_buttons[0] = true;
		pointer.x = GET_X_LPARAM(msg.lParam); 
		pointer.y = GET_Y_LPARAM(msg.lParam);
		event = EVENT_MOUSE_BUTTON;
	} break;
	case WM_LBUTTONUP: {
		mouse_buttons[0] = false;
		pointer.x = GET_X_LPARAM(msg.lParam); 
		pointer.y = GET_Y_LPARAM(msg.lParam);
		event = EVENT_MOUSE_BUTTON;
	} break;
	case WM_RBUTTONDOWN: {
		mouse_buttons[1] = true;
		pointer.x = GET_X_LPARAM(msg.lParam); 
		pointer.y = GET_Y_LPARAM(msg.lParam);
		event = EVENT_MOUSE_BUTTON;
	} break;
	case WM_RBUTTONUP: {
		mouse_buttons[1] = false;
		pointer.x = GET_X_LPARAM(msg.lParam); 
		pointer.y = GET_Y_LPARAM(msg.lParam);
		event = EVENT_MOUSE_BUTTON;
	} break;

	default: {
		printf("DISPATCH\n");
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} break;
	}

	return event;
}

Event PeekEvent() {
	return EVENT_NULL;
}

int PendingEvents() {
	return EVENT_NULL;
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	MOUSEHOOKSTRUCT m = *((MOUSEHOOKSTRUCT*) lParam);
	
	pointer.x = m.pt.x;
	pointer.y = m.pt.y;

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void GrabMouse() {
	grab_mode = true;
	hook = SetWindowsHookEx(WH_MOUSE, MouseProc, NULL, GetCurrentThreadId());
	SetCapture(hwnd);
}

void ReleaseMouse() {
	grab_mode = false;
	ReleaseCapture();
	UnhookWindowsHookEx(hook);
}

void Console(const char* s) {
	// No consoles for windows.
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CLOSE: {
		printf("CLOSE\n");
		DestroyWindow(hwnd);
	} break;
	case WM_DESTROY: {
		printf("DESTROY\n");
		PostQuitMessage(0);
	} break;
	}
	
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {	
	// Register the window class.
	const wchar_t CLASS_NAME[]  = L"PixelGrab";
    
	WNDCLASS wc = { };

	wc.lpfnWndProc   = WindowProc;
	wc.hInstance     = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	{ // Create & Setup Window
		DWORD style = (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX);
		RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

		AdjustWindowRectEx(&rc, style, FALSE, 0);
		
		hwnd = CreateWindowEx(
			0,            // Optional window styles.
			CLASS_NAME,   // Window class
			L"PixelGrab", // Window text
			style,

			// Size and position
			CW_USEDEFAULT, CW_USEDEFAULT, (rc.right - rc.left), (rc.bottom - rc.top),

			NULL,       // Parent window    
			NULL,       // Menu
			hInstance,  // Instance handle
			NULL        // Additional application data
			);
	}
	
	if (hwnd == NULL)
		return 0;

	screen = new Pixel[SCREEN_WIDTH * SCREEN_HEIGHT];
	ShowWindow(hwnd, nCmdShow);
	
	auto status = AppMain(0, NULL);

	return status;
}
