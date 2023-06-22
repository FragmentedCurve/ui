#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <windowsx.h>

#include "common.h"

#include <cstdio> // TODO: For debugging, remove later.

static HWND hwnd;

Pixel GetPixel(int x, int y) { return 0; }

void UpdateWindow() {
	HDC hdc = GetDC(hwnd);

	for (auto i = 0; i < SCREEN_WIDTH; i++) {
		for (auto j = 0; j < SCREEN_HEIGHT; j++) {
			auto c = screen[j * SCREEN_WIDTH + i];
			// Swap the blue and red bytes
			c = ((c & 0x00ff0000) >> 16) | ((c & 0x000000ff) << 16) | (c & 0x0000ff00);
			SetPixel(hdc, i, j, c);
		}
	}

	ReleaseDC(hwnd, hdc);
}

void ToClipboard(const char* s) {}

Event GetEvent() {
	Event event = EVENT_NULL;
	MSG msg = {};

	if (GetMessage(&msg, NULL, 0, 0) <= 0) {
		return EVENT_QUIT;
	}

	switch (msg.message) {
	case WM_DESTROY: {
		//TranslateMessage(&msg);
		//DispatchMessage(&msg);
		//PostQuitMessage(0);
		event = EVENT_QUIT;
	} break;

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

void GrabMouse() {
	SetCapture(hwnd);
}

void ReleaseMouse() {
	ReleaseCapture();
}

void Console(const char* s) {
	// No consoles for windows.
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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

	// Create the window.
	hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"PixelGrab",                   // Window text
		(WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME),

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH + 100, SCREEN_HEIGHT + 100,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
		);
	

	if (hwnd == NULL)
		return 0;

	screen = new Pixel[SCREEN_WIDTH * SCREEN_HEIGHT];
	ShowWindow(hwnd, nCmdShow);
	
	auto status = AppMain(0, NULL);

	return status;
}
