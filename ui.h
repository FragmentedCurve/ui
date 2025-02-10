#ifndef _UI_H_
#define _UI_H_

#include <cstdarg>

#include "ui_common.h"
#include "ui_style.h"

// TODO: Split Screen into a framebuffer class and an abstract
// class. The abstract class should just define the minimum methods a
// backend renderer requires.
struct Screen {
	Screen(int xw, int yw); // TODO: Abstract renderer
	Screen(uint32_t *pixels, int xw, int yw);
	Screen(uint32_t *pixels, int xw, int yw, int pitch);

	Screen* Subset(Rect r);// TODO: Abstract renderer
	void Resize(int xw, int yw);

	void DrawHLine(Pixel c, Point p, int width);// TODO: Abstract renderer
	void DrawHLine(Pixel c, Point p, int width, Rect clip);// TODO: Abstract renderer
	void DrawVLine(Pixel c, Point p, int height);// TODO: Abstract renderer
	void DrawVLine(Pixel c, Point p, int height, Rect clip);// TODO: Abstract renderer
	void DrawFill(Pixel c, Rect r);// TODO: Abstract renderer
	void DrawRect(Pixel c, Rect r);// TODO: Abstract renderer
	void DrawRect(Pixel c, Rect r, Rect clip);// TODO: Abstract renderer

	int xw, yw, pitch;
	uint32_t *pixels;
};

typedef int UIHandle;
  
struct UIWidget {
	UIWidget(UIHandle id) : id(id) { }
	UIWidget(UIHandle id, Rect r) : id(id), r(r) { }
	UIWidget(UIHandle id, Point pos, int xw, int yw) : id(id), r(pos, xw, yw) { }

	virtual UIWidget* Hit(Point p) {
		if (!r.Hit(p) || !visible) {
			return NULL;
		}

		// this is the new parent so p must be relative to this.r
		p = Point(p.x - r.p.x, p.y - r.p.y);

		for (UIWidget* walk = childtail; walk; walk = walk->prev) {
			UIWidget* hit = walk->Hit(p);
			if (hit) {
				return hit;
			}
		}

		return this;
	}

	virtual UIWidget* Hit(int x, int y) {
		return Hit(Point(x, y));
	}

	virtual void HandlePress(Point) {}
	virtual void HandleClick(Point) {}

	virtual void Draw(Screen *scr) {}

	virtual void Move(int x, int y) {
		r = Rect(x, y, r.xw, r.yw);
	}

	virtual void Move(Point p) {
		Move(p.x, p.y);
	}

	virtual void Push(int x, int y) {
		r = r.From(x, y);
	}
	
	virtual void Push(Point p) {
		Push(p.x, p.y);
	}

	virtual void Resize(int dx, int dy) {
		r = r.Resize(dx, dy);
	}

	Rect Abs() {
		Rect result = r;
		for (UIWidget* walk = parent; walk; walk = walk->parent) {
			result = result.From(walk->r.p);
		}
		return result;
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

	UIWidget* Children(UIWidget* w, ...) {
		if (!w) {
			return this;
		}
		w->Parent(this);

		va_list ap;
		va_start(ap, w);
		for (UIWidget* child = va_arg(ap, UIWidget*); child; child = va_arg(ap, UIWidget*)) {
			child->Parent(this);
		}
		va_end(ap);

		return this;
	}
#define Children(...) Children(__VA_ARGS__, NULL) // TODO: Is there a better C++ way to do this?


	// Children (subtree)
	UIWidget* childhead = NULL; // Youngest child
	UIWidget* childtail = NULL; // Oldest child
	UIWidget* next      = NULL; // Older sibling
	UIWidget* prev      = NULL; // Younger sibling
	UIWidget* parent    = NULL; // Our Parent

	// Properties
	UIHandle id       = -1;               // Unique ID of widget instance
	Rect     r        = Rect(0, 0, 0, 0); // The widget's real estate.
	bool     visible  = true;             // Widget is active and visable.
	bool     disabled = false;            // Widget is visible but not functional.
	bool     pressed  = false;            // Pressed with left mouse button.
	bool     drag     = false;
};

struct UISurface : UIWidget {
	UISurface(UIHandle id, Rect r) : UIWidget(id, r) {}

	virtual void Draw(Screen* scr) {
		scr->DrawFill(UI_SURFACE_SHADOW, r);
	}
};

struct UIPanel : UIWidget {
	UIPanel(UIHandle id, Rect r) : UIWidget(id, r) {}

	virtual void Draw(Screen* scr) {
		scr->DrawFill(UI_SURFACE_BG, r);
	}
};

struct UILight : UIWidget {
	UILight(UIHandle id, Point position) : UIWidget(id, position, 16, 16) {}
	UILight(UIHandle id, Rect r) : UIWidget(id, r) {}
	virtual void Draw(Screen *scr);

	void On() { on = true; }
	void Off() { on = false; }
	void Toggle() { on = !on; }
	bool State() { return on; }

	// Properties
	bool on = false;
};

struct UIButton : UIWidget {
	UIButton(UIHandle id, Point position, int xw, int yw) : UIWidget(id, position, xw, yw) {}
	void Draw(Screen* scr);

	// Properties
	char* caption;

private:
	void Draw(Screen* scr, Pixel hilite, Pixel shadow, Pixel bg);
};

struct UIToggle : UIButton {
	UIToggle(UIHandle id, Point p, int xw, int yw)
		: UIButton(id, p, xw, yw), light(id, Rect(8, 8, 16, yw - 16)) {
	}

	virtual void Move(int x, int y) {
		UIWidget::Move(x, y);
		light.Move(x, y);
	}

	virtual void Resize(int xw, int yw) {
		UIWidget::Resize(xw, yw);
		light.Resize(0, yw);
	}

	virtual void HandleClick(Point p) {
		Toggle();
	}

	void Toggle() { light.Toggle(); }
	void On() { light.On(); }
	void Off() { light.Off(); }

	virtual void Draw(Screen *scr);
	UILight light;
};

struct UIPixelGrid : UIWidget {
	UIPixelGrid(UIHandle id, Point position, int xw, int yw , int zoom);
	virtual ~UIPixelGrid();
	void Draw(Screen* scr);
	Pixel Get(int x, int y) { return pixels[UI_INDEX(cols, x, y)]; }
	void Set(Pixel c, int x, int y) { pixels[UI_INDEX(cols, x, y)] = c; }

//protected:
	void DrawCell(Screen* scr, int x, int y);
	Point CellPosition(int x, int y);

	int cols, rows, zoom;
	Pixel* pixels;
};

struct UIPixelSelector : UIPixelGrid {
	UIPixelSelector(UIHandle id, Point position, int xw, int yw, int zoom)
		: UIPixelGrid(id, position, xw, yw, zoom) {
		select = prev_select = Point(cols / 2, rows / 2);
	}
	void Select(int x, int y);
	Pixel Get() { return UIPixelGrid::Get(select.x, select.y); }
	virtual void Draw(Screen* scr);
	virtual void HandleClick(Point p);

protected:
	Point select, prev_select;

private:
	void DrawSelection(Screen* scr);
	void EraseSelection(Screen* scr);
	bool pressed = false;
};

struct HexFloat : UIWidget {
	HexFloat(UIHandle id) : UIWidget(id, Point(0, 0), 48 + 25 + 2, 25 + 2) { Set(UI_LIGHTEST); }
	HexFloat(UIHandle id, Point p) : UIWidget(id, p, 48 + 25 + 2, 25 + 2) { Set(UI_LIGHTEST); }
	void Draw(Screen* scr);
	void Set(Pixel c);

	char color_s[8];

private:
	void DrawFont(Screen* scr, bool *gylph, int len, int x0, int y0, Pixel c);
};

// This is the compliment of UIState.
// TODO: UIOutput needs a better name.
struct UIOutput {
	UIWidget* pressed;
	UIWidget* clicked;
};

// TODO: UIDelegate needs a new name. It's doing transformations
// instead of delegations now.
UIOutput UIDelegate(UIState state, UIWidget* root);
void UIDraw(Screen* scr, UIWidget* root);

#endif // _UI_H_
