#ifndef _UI_COMMON_H_
#define _UI_COMMON_H_

#include <cstdint>

typedef uint32_t Pixel;

#define __RGB(r, g, b) ((Pixel)(((r) & 0xff) << 16 | ((g) & 0xff) << 8 | ((b) & 0xff)))
#define __BGR(r, g, b) ((Pixel)(((b) & 0xff) << 16 | ((g) & 0xff) << 8 | ((r) & 0xff)))

#define RGB(r, g, b) __RGB(r, g, b)

#define WHITE RGB(255, 255, 255)
#define BLACK RGB(  0,   0,   0)
#define RED   RGB(255,   0,   0)
#define GREEN RGB(  0, 255,   0)
#define BLUE  RGB(  0,   0, 255)


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
	Rect From(int x, int y) {
		return Rect(p.From(x, y), xw, yw);
	}
	Rect From(Point q) {
		return From(q.x, q.y);
	}

	Rect Resize(int dx, int dy) {
		return Rect(p, xw + dx, yw + dy);
	}

	Rect Width(int xw) {
		return Rect(this->p, xw, this->yw);
	}

	Rect Height(int yw) {
		return Rect(this->p, this->xw, yw);
	}

	Rect Size(int xw, int yw) {
		return Rect(this->p, xw, yw);
	}
};

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

// Provided to the platform by PixelGrab
extern Point pointer;
extern bool mouse_buttons[2];
extern Pixel* screen;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern const char* WINDOW_TITLE;
int AppMain(int argc, char **argv);
#endif // _UI_COMMON_H_
