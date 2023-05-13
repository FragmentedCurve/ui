#ifndef _UI_H_
#define _UI_H_

#include "common.h"

#define INDEX(xw, x, y) ((xw) * (y) + (x))

struct Screen {
	Screen(int xw, int yw);
	Screen(uint32_t *pixels, int xw, int yw);

	void DrawHLine(Pixel c, Point p, int width);
	void DrawVLine(Pixel c, Point p, int height);
	void DrawFill(Pixel c, Point p0, Point p1);
	void DrawRect(Pixel c, Point p0, Point p1);

	int xw, yw;
	uint32_t *pixels;
};

enum UIColors {
	UI_DARKEST        = 0x000000,
	UI_LIGHTEST       = 0xffffff,
	UI_SURFACE_BG     = 0xdedbde, // TODO: Rename this, "BG" isn't properly descriptive.
	UI_SURFACE_FG     = 0xe8e8e8, // TODO: Rename for the same reasons above.
	UI_SURFACE_HILITE = 0xf0f0f0,
	UI_SURFACE_SHADOW = 0xa0a0a0,
	UI_SURFACE_BORDER = 0x000000,
	UI_TEXT_BG        = 0xffffff,
	UI_TEXT_FG        = 0x000000,
	UI_TEXT_BORDER    = 0x2e2e2e,
	UI_LIGHT_BORDER   = 0x2e2e2e,
	UI_LIGHT_ON       = 0xffd166,
	UI_LIGHT_OFF      = 0xdedbde,
};

// TODO: Change "EVENT_" prefix and reduce to only 2 results.
//enum EventOutcome { EVENT_UNHANDLED, EVENT_PROCESSED, EVENT_ABSORBED };

struct UIWidget {
        UIWidget() {}
	
        UIWidget(Point pos, int xw, int yw) {
		this->pos = pos;
		this->xw = xw;
		this->yw = yw;
        }

        bool Hit(Point p) {
		return Hit(p.x, p.y);
	}
	
        bool Hit(int x, int y) {
		return (x >= pos.x) && (x < pos.x + xw) && (y >= pos.y) && (y < pos.y + yw);
	}

	//bool CaptureMouse(Event e);
	
        virtual void Draw(Screen *scr) {}

	virtual bool Handle(Event e) {
		//CaptureMouse(e);
		return HANDLED_FAILURE;
	}

        Point pos = Point(0, 0);
	int xw = 0, yw = 0;
	bool paint = true;
	bool visible = true;
        void (*callback)(UIWidget*, Event, void*) = NULL;
        //bool handle_events[EVENT_LAST] = {0};
};

struct UILight : UIWidget {
        UILight(Point position) : UIWidget(position, 16, 16) {}
	virtual void Draw(Screen* scr);
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
	
private:
	void Draw(Screen* scr, Pixel hilite, Pixel shadow, Pixel bg);
	char* label;
	bool pressed = false;
};

struct UIToggle : UIButton {
        UIToggle(Point position, int xw, int yw)
            : UIButton(position, xw, yw),
              light(position.From(10, yw / 2 - 8)) {
		callback = UIToggle::default_callback;
	}

	void Toggle() { light.Toggle(); }
        void On() { light.On(); }
        void Off() { light.Off(); }
	
        virtual void Draw(Screen *scr);
	virtual bool Handle(Event e);

	// TODO: Should this default callback exist? Maybe just let
	// the user define the callback.
        static void default_callback(UIWidget *w, Event e, void* data) {
                auto toggle = (UIToggle *)w;
                toggle->Toggle();
		(void) data;
        }

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
	
protected:
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

struct UIPixelScanner : UIPixelSelector {
        UIPixelScanner(Point position, int xw, int yw, int zoom)
		: UIPixelSelector(position, xw, yw, zoom) {}
	bool Toggle();
	void On(); // TODO: Implement.
	void Off(); // TODO: Implement.
        bool ScanMode() { return scan_mode; }
        virtual bool Handle(Event e);
	virtual void Draw(Screen* scr);
	
private:
	void Scan();
	bool scan_mode = false;
};

struct HexFloat : UIWidget {
        HexFloat() : UIWidget(Point(0, 0), 48 + 25 + 2, 25 + 2) { Set(WHITE); }
	HexFloat(Point p) : UIWidget(p, 48 + 25 + 2, 25 + 2) { Set(WHITE); }
	void Draw(Screen* scr);
	void Set(Pixel c);
	
	char color_s[8];
	
private:
	void DrawFont(Screen* scr, bool *gylph, int len, int x0, int y0);
};

void UIDelegate(Event e, UIWidget* w[], int n);
void UIDraw(Screen *scr, UIWidget *w[], int n);

void SetOwner(UIWidget* w, ...);
void ReleaseOwner(UIWidget* w);
void ReleaseOwner(UIWidget* w, int e, ...);

#endif // _UI_H_
