#ifndef _UI_H_
#define _UI_H_

#include "ui_common.h"
#include "ui_util.h"

struct Screen {
	Screen(int xw, int yw);
	Screen(uint32_t *pixels, int xw, int yw);

	void DrawHLine(Pixel c, Point p, int width);
	void DrawHLine(Pixel c, Point p, int width, Rect clip);
	void DrawVLine(Pixel c, Point p, int height);
	void DrawVLine(Pixel c, Point p, int height, Rect clip);
	void DrawFill(Pixel c, Rect r);
	void DrawRect(Pixel c, Rect r);
	void DrawRect(Pixel c, Rect r, Rect clip);

	int xw, yw;
	uint32_t *pixels;
};

enum UIColors {
	UI_DARKEST        = RGB(   0,    0   , 0),
	UI_LIGHTEST       = RGB(0xff, 0xff, 0xff),
	UI_SURFACE_BG     = RGB(0xde, 0xdb, 0xde), // TODO: Rename this, "BG" isn't properly descriptive.
	UI_SURFACE_FG     = RGB(0xe8, 0xe8, 0xe8), // TODO: Rename for the same reasons above.
	UI_SURFACE_HILITE = RGB(0xf0, 0xf0, 0xf0),
	UI_SURFACE_SHADOW = RGB(0xa0, 0xa0, 0xa0),
	UI_SURFACE_BORDER = RGB(   0,    0,    0),
	UI_TEXT_BG        = RGB(0xff, 0xff, 0xff),
	UI_TEXT_FG        = RGB(   0,    0,    0),
	UI_TEXT_BORDER    = RGB(0x2e, 0x2e, 0x2e),
	UI_LIGHT_BORDER   = RGB(0x2e, 0x2e, 0x2e),
	UI_LIGHT_ON       = RGB(0xff, 0xd1, 0x66), // TODO: Potential alternative default (green): 0x00cd00
	UI_LIGHT_OFF      = RGB(0xde, 0xdb, 0xde),
};

// TODO: Change "EVENT_" prefix and reduce to only 2 results.
//enum EventOutcome { EVENT_UNHANDLED, EVENT_PROCESSED, EVENT_ABSORBED };

typedef void (*UICallback)(struct UIWidget*, Event, void*);
typedef bool (*UIHandler)(struct UIWidget*, Event);

struct UIWidget {
	UIWidget() {}
	UIWidget(Rect r) : r(r) {}
	UIWidget(Point pos, int xw, int yw) : r(pos, xw, yw) {}

	bool Hit(Point p) {
		return Hit(p.x, p.y);
	}

	bool Hit(int x, int y) {
		return r.Hit(x, y);
	}

	virtual void Draw(Screen *scr) {}
	//virtual void Draw(Screen *scr, Rect clip = Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)) {}

	virtual bool Handle(Event e) {
		return HANDLED_FAILURE;
	}

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
	
	Rect r = Rect(0, 0, 0, 0);

	// Properties
	bool visible = true;   // Widget is active and visable.
	
	// Called when the widget executes it's behavior.
	UICallback callback = NULL;

	// Call before the handler is called.
	UIHandler prehook[EVENT_LAST] = {0};

	// Replace the default handler.
	UIHandler hook[EVENT_LAST] = {0};

	// Call after the handler is called.
	UIHandler posthook[EVENT_LAST] = {0};
};


struct UIRoot {
	UIRoot() {
		
	}

	UIWidget** children = NULL;
	char* names = NULL;
};

struct UISurface : UIWidget {
	UISurface(Rect r) : UIWidget(r) {}

	virtual void Draw(Screen* scr) {
		scr->DrawFill(UI_SURFACE_BG, r);
	}
  /*
	virtual bool Handle(Event e) {
		static auto entry = false;
		static auto mstates = mouse_buttons;
		
		if (e != EVENT_MOUSE_BUTTON)
			return HANDLED_FAILURE;
	}
  */
};

struct UILight : UIWidget {
	UILight(Point position) : UIWidget(position, 16, 16) {}
  	UILight(Rect r) : UIWidget(r) {}
	virtual void Draw(Screen *scr);
	void On() { on = true; }
	void Off() { on = false; }
	void Toggle() { on = !on; }
	bool State() { return on; }

private:
	bool on = false;
};

struct UIButton : UIWidget {
	UIButton(Point position, int xw, int yw) : UIWidget(position, xw, yw) {}

	void Draw(Screen* scr);
	virtual bool Handle(Event e);

	bool Pressed() { return pressed; }
	
private:
	void Draw(Screen* scr, Pixel hilite, Pixel shadow, Pixel bg);
	char* label;
	bool pressed = false;
};

struct UIToggle : UIButton {
	UIToggle(Point p, int xw, int yw)
		: UIButton(p, xw, yw), light(Rect(p.From(8, 8), 16, yw - 16)) { }
	
	virtual void Move(int x, int y) {
		UIWidget::Move(x, y);
		light.Move(x, y);
	}

	virtual void Resize(int xw, int yw) {
		UIWidget::Resize(xw, yw);
		light.Resize(0, yw);
	}
	
	void Toggle() { light.Toggle(); }
	void On() { light.On(); }
	void Off() { light.Off(); }

	virtual void Draw(Screen *scr);
	virtual bool Handle(Event e);

private:
	bool toggle = false;
	UILight light;
};

struct UIPixelGrid : UIWidget {
	UIPixelGrid(Point position, int xw, int yw , int zoom);
	virtual ~UIPixelGrid();
	void Draw(Screen* scr);
	Pixel Get(int x, int y) { return pixels[INDEX(cols, x, y)]; }
	void Set(Pixel c, int x, int y) { pixels[INDEX(cols, x, y)] = c; }

//protected:
	void DrawCell(Screen* scr, int x, int y);
	Point CellPosition(int x, int y);

	int cols, rows, zoom;
	Pixel* pixels;
};

struct UIPixelSelector : UIPixelGrid {
	UIPixelSelector(Point position, int xw, int yw, int zoom)
		: UIPixelGrid(position, xw, yw, zoom) {
		select = prev_select = Point(cols / 2, rows / 2);
	}
	void Select(int x, int y);
	Pixel Get() { return UIPixelGrid::Get(select.x, select.y); }
	virtual void Draw(Screen* scr);
	virtual bool Handle(Event e);

protected:
	Point select, prev_select;

private:
	void DrawSelection(Screen* scr);
	void EraseSelection(Screen* scr);
	bool pressed = false;
};

struct HexFloat : UIWidget {
	HexFloat() : UIWidget(Point(0, 0), 48 + 25 + 2, 25 + 2) { Set(UI_LIGHTEST); }
	HexFloat(Point p) : UIWidget(p, 48 + 25 + 2, 25 + 2) { Set(UI_LIGHTEST);
	}
	void Draw(Screen* scr);
	void Set(Pixel c);

	char color_s[8];

private:
	void DrawFont(Screen* scr, bool *gylph, int len, int x0, int y0, Pixel c);
};

UIWidget* UIDelegate(Event e, UIWidget* w[], int n);
void UIDraw(Screen *scr, UIWidget *w[], int n);
//void UIDraw(Screen *scr, UIWidget *w[], int n, Rect clip);

void SetOwner(UIWidget* w, ...);
void ReleaseOwner(UIWidget* w);
void ReleaseOwner(UIWidget* w, int e, ...);
#endif // _UI_H_
