#ifndef _COMMON_H_
#define _COMMON_H_

#include <cstdint>

typedef uint32_t Pixel;

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
	int x = 0;
	int y = 0;

	Point() {}
	Point(int x, int y): x(x), y(y) {}

	// TODO: Pick a better name for From.
	Point From(Point p) { return Point(x + p.x, y + p.y); }
	Point From(int x, int y) { return From(Point(x, y)); }
};

struct Rect {
	Point p;
	int xw, yw;

	Rect(int x, int y, int xw, int yw) : p(x, y), xw(xw), yw(yw) {}
	Rect(Point origin, int xw, int yw) : p(origin), xw(xw), yw(yw) {}
	Rect(Point p0, Point p1) : p(p0), xw(p1.x - p0.x), yw(p1.y - p0.y) {}

	bool Hit(int x, int y) {
		return (x >= p.x) && (x <= p.x + xw) && (y >= p.y) && (y <= p.y + yw);
	}
	bool Hit(Point q) { return Hit(q.x, q.y); }

	bool Boundary(int x, int y) {
		return (x == p.x || x == p.x + xw) && (y == p.y || y == p.y + yw);
	}
	bool Boundary(Point q) { return Boundary(q.x, q.y); }

	int Area() { return xw * yw; }
	int Perimeter() { return 2 * (xw + yw); }

	// TODO: Should I rename From to Move?
	Rect From(int x, int y) { return Rect(p.From(x, y), xw, yw); }
	Rect From(Point q) { return From(q.x, q.y); }
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
