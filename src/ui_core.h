#ifndef _UI_CORE_H_
#define _UI_CORE_H_

#include <cstdarg>

#define UI_INDEX(xw, x, y) ((xw) * (y) + (x))

#ifndef NULL
#define NULL 0
#endif

typedef uint32_t UIPixel;

struct UIPoint {
	int x = 0;
	int y = 0;

	UIPoint() {}
	UIPoint(int x, int y): x(x), y(y) {}

	// TODO: Pick a better name for From.
	UIPoint From(UIPoint p) { return UIPoint(x + p.x, y + p.y); }
	UIPoint From(int x, int y) { return From(UIPoint(x, y)); }
};

struct UIRect {
	UIPoint p;
	int xw, yw;

	UIRect(int x, int y, int xw, int yw) : p(x, y), xw(xw), yw(yw) {}
	UIRect(UIPoint origin, int xw, int yw) : p(origin), xw(xw), yw(yw) {}
	UIRect(UIPoint p0, UIPoint p1) : p(p0), xw(p1.x - p0.x), yw(p1.y - p0.y) {}

	bool Hit(int x, int y) {
		return (x >= p.x) && (x <= p.x + xw) && (y >= p.y) && (y <= p.y + yw);
	}
	bool Hit(UIPoint q) { return Hit(q.x, q.y); }

	bool Boundary(int x, int y) {
		return (x == p.x || x == p.x + xw) && (y == p.y || y == p.y + yw);
	}
	bool Boundary(UIPoint q) { return Boundary(q.x, q.y); }

	int Area() { return xw * yw; }
	int Perimeter() { return 2 * (xw + yw); }

	// TODO: Should I rename From to Move?
	UIRect From(int x, int y) {
		return UIRect(p.From(x, y), xw, yw);
	}
	UIRect From(UIPoint q) {
		return From(q.x, q.y);
	}

	UIRect Resize(int dx, int dy) {
		return UIRect(p, xw + dx, yw + dy);
	}

	UIRect Width(int xw) {
		return UIRect(this->p, xw, this->yw);
	}

	UIRect Height(int yw) {
		return UIRect(this->p, this->xw, yw);
	}

	UIRect Size(int xw, int yw) {
		return UIRect(this->p, xw, yw);
	}
};

// TODO: Split UIScreen into a framebuffer class and an abstract
// class. The abstract class should just define the minimum methods a
// backend renderer requires.
struct UIScreen {
	UIScreen(int xw, int yw); // TODO: Abstract renderer
	UIScreen(uint32_t *pixels, int xw, int yw);
	UIScreen(uint32_t *pixels, int xw, int yw, int pitch);

	UIScreen* Subset(UIRect r);// TODO: Abstract renderer
	void Resize(int xw, int yw);

	void DrawHLine(UIPixel c, UIPoint p, int width);// TODO: Abstract renderer
	void DrawHLine(UIPixel c, UIPoint p, int width, UIRect clip);// TODO: Abstract renderer
	void DrawVLine(UIPixel c, UIPoint p, int height);// TODO: Abstract renderer
	void DrawVLine(UIPixel c, UIPoint p, int height, UIRect clip);// TODO: Abstract renderer
	void DrawFill(UIPixel c, UIRect r);// TODO: Abstract renderer
	void DrawRect(UIPixel c, UIRect r);// TODO: Abstract renderer
	void DrawRect(UIPixel c, UIRect r, UIRect clip);// TODO: Abstract renderer

	int xw, yw, pitch;
	uint32_t *pixels;
};

typedef int UIHandle;
  
struct UIWidget {
	UIWidget(UIHandle id) : id(id) {}
	UIWidget(UIHandle id, UIRect r) : id(id), r(r) {}
	UIWidget(UIHandle id, UIPoint pos, int xw, int yw) : id(id), r(pos, xw, yw) {}

	virtual UIWidget* Hit(UIPoint p);
	virtual UIWidget* Hit(int x, int y);

	virtual void HandlePress(UIPoint) {}
	virtual void HandleClick(UIPoint) {}
	virtual void Draw(UIScreen *scr) {}

	virtual void Move(int x, int y);
	virtual void Move(UIPoint p);
	virtual void Push(int x, int y);
	virtual void Push(UIPoint p);
	virtual void Resize(int dx, int dy);
	UIRect Abs();

	UIWidget* Parent(UIWidget* parent);
	UIWidget* __Children(UIWidget* w, ...);
#define Children(...) __Children(__VA_ARGS__, NULL) // TODO: Is there a better C++ way to do this?


	// Children (subtree)
	UIWidget* childhead = NULL; // Youngest child
	UIWidget* childtail = NULL; // Oldest child
	UIWidget* next      = NULL; // Older sibling
	UIWidget* prev      = NULL; // Younger sibling
	UIWidget* parent    = NULL; // Our Parent

	// Properties
	UIHandle id       = -1;                 // Unique ID of widget instance
	UIRect   r        = UIRect(0, 0, 0, 0); // The widget's real estate.
	bool     visible  = true;               // Widget is active and visable.
	bool     disabled = false;              // Widget is visible but not functional.
	bool     pressed  = false;              // Pressed with left mouse button.
	bool     drag     = false; // TODO: Should this exist?
};

struct UIRawInput {
	UIPoint pointer       = UIPoint(0, 0);
	UIPoint dpointer      = UIPoint(0, 0);
	bool    m[32]         = {false};
	char    keys[127]     = {0};
	int     screen_width  = 0;
	int     screen_height = 0;
	bool    halt          = false;
};

struct UIReaction  {
	UIWidget* pressed;
	UIWidget* clicked;
};

// Provided to the platform by UIPixelGrab
extern       UIPixel* screen;
extern const int      SCREEN_WIDTH;
extern const int      SCREEN_HEIGHT;
extern const char*    WINDOW_TITLE;

int        UIMain(int argc, char **argv);
UIReaction UIImpacted(UIRawInput state, UIWidget* root);
void       UIDraw(UIScreen* scr, UIWidget* root);


// Platform Dependent, implemented in os_*.cc
UIRawInput UINativeState();
void       UINativeUpdate();
void       UINativeToClipboard(const char* s);
void       UINativeConsole(const char* s);

// TODO: Extra native functions for pixelgrab. Possibly remove later.
void       UINativeGrabMouse();
void       UINativeReleaseMouse();
UIPixel    UINativeGetPixel(int x, int y);

#endif // _UI_CORE_H_
