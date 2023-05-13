#ifndef _COMMON_H_
#define _COMMON_H_

#include <cstdint>

typedef uint32_t Pixel;

#define BLACK ((Pixel)0x000000)
#define WHITE ((Pixel)0xffffff)
#define RED   ((Pixel)0xff0000)
#define GREEN ((Pixel)0x00ff00)
#define BLUE  ((Pixel)0x0000ff)

#define INVERT_PIXEL(c) ((WHITE ^ (c)) | RED)

enum Event {
	EVENT_NULL,           // Who knows?
	EVENT_QUIT,           // Quit message from WM
	EVENT_RESIZE,         // Screen/Window was resized
	EVENT_MOUSE_BUTTON,   // A mouse button was either pressed or released
	EVENT_MOUSE_MOVE,     // You figure this one out
	EVENT_KEY_PRESS,      // Any key was pressed
	EVENT_KEY_RELEASE,    // Any key was released
	EVENT_UPDATE_WINDOW,  // UpdateWindow() should be called
	
	/* TODO
	   Add events for

	   - Updating the window (basically call UpdateWindow()).
	   - Screen/window resized.
	   - Redraw part of the screen.
	*/
	   
	// --
	
	EVENT_LAST
};

#define NUMBER_OF(type, array) (sizeof(array)/sizeof(type))

#define HANDLED_FAILURE false
#define HANDLED_SUCCESS true

struct Point {
	int x, y;

	Point() { y = x = 0; }

	Point(int x, int y) {
		this->x = x;
		this->y = y;
	}

	Point From(Point p) { return Point(x + p.x, y + p.y); }
	Point From(int x, int y) { return From(Point(x, y)); }
};

#ifndef NULL
#define NULL 0
#endif

#define UNUSED(var) (void) var;

#define WINDOW_TITLE "PixelGrab"

// Provided to the platform by PixelGrab
extern Point pointer;
extern bool mouse_buttons[2];
extern Pixel* screen;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
int AppMain(int argc, char **argv);

#ifdef _DEBUG
#include <cstdio>
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#define DEBUG_MOUSE                                                            \
  DEBUG("mouse_buttons = {%d, %d}; pointer = {%d, %d}\n", mouse_buttons[0],          \
        mouse_buttons[1], pointer.x, pointer.y)
#endif

#endif // _COMMON_H_
