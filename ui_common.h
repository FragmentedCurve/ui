#ifndef _UI_COMMON_H_
#define _UI_COMMON_H_

#include <cstdint>

#define UI_INDEX(xw, x, y) ((xw) * (y) + (x))

#ifndef NULL
#define NULL 0
#endif

typedef uint32_t Pixel;

#define __RGB(r, g, b) ((Pixel)(((r) & 0xff) << 16 | ((g) & 0xff) << 8 | ((b) & 0xff)))
#define __BGR(r, g, b) ((Pixel)(((b) & 0xff) << 16 | ((g) & 0xff) << 8 | ((r) & 0xff)))

#define UI_RGB(r, g, b) __RGB(r, g, b)

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

// TODO: UIState needs a better name.
struct UIState {
	Point pointer       = Point(0, 0);
	Point dpointer      = Point(0, 0);
	bool  m[32]         = {false};
	char  keys[127]     = {0};
	int   screen_width  = 0;
	int   screen_height = 0;
	bool  halt          = false;
//TODO:	int   dscreen_width  = 0;  Maybe
//TODO:	int   dscreen_height = 0;  Maybe
};

// Provided to the platform by PixelGrab
extern Point pointer;
extern bool mouse_buttons[2];
extern Pixel* screen;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const char* WINDOW_TITLE;
int UIMain(int argc, char **argv);


// Platform Dependent, implemented in os_*.cc
Pixel GetPixel(int x, int y);
void UpdateWindow();
void ToClipboard(const char* s);
UIState UIGetState();
void GrabMouse();
void ReleaseMouse();
void Console(const char* s);

#endif // _UI_COMMON_H_
