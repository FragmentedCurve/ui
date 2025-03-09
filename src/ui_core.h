/*
 * Copyright (c) 2025 Paco Pascal <me@pacopascal.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

#define UI_INDEX(xw, x, y) ((xw) * (y) + (x))

#ifndef NULL
#    define NULL 0
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

	UIRect() : UIRect(0, 0, 0, 0) {}
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
	// TODO: Abstract renderer
	UIScreen(int xw, int yw)
		: UIScreen(new uint32_t[xw * yw], xw, yw, xw) {}
	UIScreen(uint32_t* pixels, int xw, int yw)
		: xw(xw), yw(yw), pitch(xw), pixels(pixels) {}
	UIScreen(uint32_t* pixels, int xw, int yw, int pitch)
		: xw(xw), yw(yw), pitch(pitch), pixels(pixels) {}

	// TODO: Abstract renderer
	UIScreen* Subset(UIRect r) {
		return new UIScreen(pixels + UI_INDEX(pitch, r.p.x, r.p.y), r.xw, r.yw, pitch);
	}

	void Resize(int xw, int yw) {
		/*
		if (xw < this->xw && yw< this->yw) {
			return;
		}

		if (xw * yw <= (this->xw + pitch) * this->yw) {
			this->xw = xw;
			this->xy = yw;
			this->pitch =
				// (pitch + xw)
				}

		if (xw < this->xw && yw < this->yw) {
			return;
		}
		*/
	}

	// TODO: Abstract renderer
	void DrawHLine(UIPixel c, UIPoint p, int width) {
		DrawHLine(c, p, width, UIRect(0, 0, xw, yw));
	}

	// TODO: Abstract renderer
	void DrawHLine(UIPixel c, UIPoint p, int width, UIRect clip) {
		// Completely outside the clip area
		if (p.x >= clip.p.x + clip.xw || p.y < clip.p.y || p.y >= clip.p.y + yw)
			return;

		// Clip left
		if (p.x < clip.p.x) {
			width -= clip.p.x - p.x;
			p.x = clip.p.x;
		}

		// Clip right
		if (p.x + width > clip.p.x + clip.xw)
			width = clip.p.x + clip.xw - p.x;

		while (--width >= 0)
			pixels[UI_INDEX(pitch, p.x + width, p.y)] = c;
	}

	// TODO: Abstract renderer
	void DrawVLine(UIPixel c, UIPoint p, int height) {
		DrawVLine(c, p, height, UIRect(0, 0, xw, yw));
	}

	// TODO: Abstract renderer
	void DrawVLine(UIPixel c, UIPoint p, int height, UIRect clip) {
		// Completely outside the clip area
		if (p.y >= clip.p.y + clip.yw || p.x < clip.p.x || p.x >= clip.p.x + clip.xw)
			return;

		// Clip top
		if (p.y < clip.p.y) {
			height -= clip.p.y - p.y;
			p.y = clip.p.y;
		}

		// Clip bottom
		if (p.y + height > clip.p.y + clip.yw)
			height = clip.p.y + clip.yw - p.y;

		while (--height >= 0)
			pixels[UI_INDEX(pitch, p.x, p.y + height)] = c;
	}

	// TODO: Abstract renderer
	void DrawFill(UIPixel c, UIRect r) {
		for (int i = 0; i < r.yw; i++)
			DrawHLine(c, r.p.From(0, i), r.xw);
	}

	// TODO: Abstract renderer
	void DrawRect(UIPixel c, UIRect r) {
		DrawRect(c, r, UIRect(0, 0, xw, yw));
	}

	// TODO: Abstract renderer
	void DrawRect(UIPixel c, UIRect r, UIRect clip) {
		// Top
		DrawHLine(c, r.p, r.xw, clip);
		// Bottom
		DrawHLine(c, r.p.From(0, r.yw - 1), r.xw, clip);
		// Left
		DrawVLine(c, r.p, r.yw, clip);
		// Right
		DrawVLine(c, r.p.From(r.xw - 1, 0), r.yw, clip);
	}

	int xw, yw, pitch;
	uint32_t *pixels;
};

typedef int UIHandle;

struct UIWidget {
	UIWidget(UIHandle id) : id(id) {}
	UIWidget(UIHandle id, UIRect r) : id(id), r(r) {}

	virtual UIWidget* Hit(UIPoint p) {
		if (!r.Hit(p) || !visible) {
			return NULL;
		}

		// this is the new parent so p must be relative to this.r
		p = UIPoint(p.x - r.p.x, p.y - r.p.y);

		for (UIWidget* walk = childtail; walk; walk = walk->prev) {
			UIWidget* hit = walk->Hit(p);
			if (hit) {
				return hit;
			}
		}

		return this;
	}
	virtual UIWidget* Hit(int x, int y) { return Hit(UIPoint(x, y)); }

	virtual void HandlePress(UIPoint) {}
	virtual void HandleClick(UIPoint) {}
	virtual void Draw(UIScreen *scr) {}

	virtual void Move(int x, int y) { r = UIRect(x, y, r.xw, r.yw); }
	virtual void Move(UIPoint p) { Move(p.x, p.y); }
	virtual void Push(int x, int y) { r = r.From(x, y); }
	virtual void Push(UIPoint p) { Push(p.x, p.y); }
	virtual void Resize(int dx, int dy) { this->r = this->r.Resize(dx, dy); }

	virtual UIWidget* SetSize(int xw, int yw) {
		r = UIRect(r.p, xw, yw);
		return this;
	}
/*
  TODO: Do I want this?
	virtal UIWidget* Set(UIProperty flag, boolean value) {
		switch (flag) {
		case UI_PROP_RESIZE:
			resize = true;
			break;
		}
		return this;
	}
*/
	UIRect Abs() {
		UIRect result = r;
		for (UIWidget* walk = parent; walk; walk = walk->parent) {
			result = result.From(walk->r.p);
		}
		return result;
	}

	UIWidget* Find(UIHandle id) {
		if (this->id == id) {
			return this;
		}

		for (UIWidget* w = childhead; w; w = w->next) {
			if (auto result = w->Find(id); result) {
				return result;
			}
		}

		// 404 not found
		return NULL;
	}

	UIWidget* Parent(UIWidget* parent) {
		if (!parent) {
			return this;
		}

		if (parent->childhead) {
			next = parent->childhead;
			next->prev = this;
			parent->childhead = this;
			this->parent = parent;
		} else {
			next = NULL;
			prev = NULL;
			parent->childhead = this;
			parent->childtail = this;
			this->parent = parent;
		}

		return this;
	}

	UIWidget* Children(UIWidget* w) {
		// TODO: Reverse the order.
		w->Parent(this);
		return this;
	}

	template <typename... Rest>
	UIWidget* Children(UIWidget* w, Rest...rest) {
		w->Parent(this);
		return Children(rest...);
	}

	// Children (subtree)
	UIWidget* childhead = NULL; // Youngest child
	UIWidget* childtail = NULL; // Oldest child
	UIWidget* next      = NULL; // Older sibling
	UIWidget* prev      = NULL; // Younger sibling
	UIWidget* parent    = NULL; // Our Parent

	// Properties
	UIHandle id              = -1;                 // Unique ID of widget instance
	UIRect   r               = UIRect(0, 0, 0, 0); // The widget's real estate.
	bool     visible         = true;               // Widget is active and visable.
	bool     disabled        = false;              // Widget is visible but not functional.
	bool     pressed         = false;              // Pressed with left mouse button.
	bool     pressed_right   = false;              // Pressed with right mouse button.
	bool     pressed_key     = false;              // Pressed with keyboard.
	bool     drag            = false;              // Follow pointer. TODO: Should this exist?
	bool     fill_screen     = false;              // Respond to screen resize.
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
extern const char*    SCREEN_TITLE;

int        UIMain(int argc, char **argv);
UIReaction UIImpacted(UIRawInput state, UIWidget* root);
void       UIDraw(UIScreen* scr, UIWidget* root);


// Platform Dependent, implemented in os_*.cc
extern int SCREEN_WIDTH_MAX;
extern int SCREEN_HEIGHT_MAX;

UIRawInput UINativeState();
void       UINativeUpdate();
void       UINativeToClipboard(const char* s);
void       UINativeConsole(const char* s);


// TODO: Extra native functions for pixelgrab. Possibly remove later.
void       UINativeGrabMouse();
void       UINativeReleaseMouse();
UIPixel    UINativeGetPixel(int x, int y);

