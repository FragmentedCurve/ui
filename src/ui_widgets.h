#ifndef _UI_WIDGETS_H_
#define _UI_WIDGETS_H_

struct UISurface : UIWidget {
	UISurface(UIHandle id, UIRect r) : UIWidget(id, r) {}

	virtual void Draw(UIScreen* scr) {
		scr->DrawFill(UI_SURFACE_SHADOW, r);
	}
};

struct UIPanel : UIWidget {
	UIPanel(UIHandle id, UIRect r) : UIWidget(id, r) {}

	virtual void Draw(UIScreen* scr) {
		scr->DrawFill(UI_SURFACE_BG, r);
	}
};

struct UILight : UIWidget {
	UILight(UIHandle id, UIPoint position) : UIWidget(id, position, 16, 16) {}
	UILight(UIHandle id, UIRect r) : UIWidget(id, r) {}
	virtual void Draw(UIScreen *scr);

	void On() { on = true; }
	void Off() { on = false; }
	void Toggle() { on = !on; }
	bool State() { return on; }

	// Properties
	bool on = false;
};

struct UIButton : UIWidget {
	UIButton(UIHandle id, UIPoint position, int xw, int yw) : UIWidget(id, position, xw, yw) {}
	void Draw(UIScreen* scr);

	// Properties
	char* caption;

private:
	void Draw(UIScreen* scr, UIPixel hilite, UIPixel shadow, UIPixel bg);
};

struct UIToggle : UIButton {
	UIToggle(UIHandle id, UIPoint p, int xw, int yw)
		: UIButton(id, p, xw, yw), light(id, UIRect(8, 8, 16, yw - 16)) {
	}

	virtual void Move(int x, int y) {
		UIWidget::Move(x, y);
		light.Move(x, y);
	}

	virtual void Resize(int xw, int yw) {
		UIWidget::Resize(xw, yw);
		light.Resize(0, yw);
	}

	virtual void HandleClick(UIPoint p) {
		Toggle();
	}

	void Toggle() { light.Toggle(); }
	void On() { light.On(); }
	void Off() { light.Off(); }

	virtual void Draw(UIScreen *scr);
	UILight light;
};

struct UIPixelGrid : UIWidget {
	UIPixelGrid(UIHandle id, UIPoint position, int xw, int yw , int zoom);
	virtual ~UIPixelGrid();
	void Draw(UIScreen* scr);
	UIPixel Get(int x, int y) { return pixels[UI_INDEX(cols, x, y)]; }
	void Set(UIPixel c, int x, int y) { pixels[UI_INDEX(cols, x, y)] = c; }

//protected:
	void DrawCell(UIScreen* scr, int x, int y);
	UIPoint CellPosition(int x, int y);

	int cols, rows, zoom;
	UIPixel* pixels;
};

struct UIPixelSelector : UIPixelGrid {
	UIPixelSelector(UIHandle id, UIPoint position, int xw, int yw, int zoom)
		: UIPixelGrid(id, position, xw, yw, zoom) {
		select = prev_select = UIPoint(cols / 2, rows / 2);
	}
	void Select(int x, int y);
	UIPixel Get() { return UIPixelGrid::Get(select.x, select.y); }
	virtual void Draw(UIScreen* scr);
	virtual void HandleClick(UIPoint p);

protected:
	UIPoint select, prev_select;

private:
	void DrawSelection(UIScreen* scr);
	void EraseSelection(UIScreen* scr);
	bool pressed = false;
};

struct HexFloat : UIWidget {
	HexFloat(UIHandle id) : UIWidget(id, UIPoint(0, 0), 48 + 25 + 2, 25 + 2) { Set(UI_LIGHTEST); }
	HexFloat(UIHandle id, UIPoint p) : UIWidget(id, p, 48 + 25 + 2, 25 + 2) { Set(UI_LIGHTEST); }
	void Draw(UIScreen* scr);
	void Set(UIPixel c);

	char color_s[8];

private:
	void DrawFont(UIScreen* scr, bool *gylph, int len, int x0, int y0, UIPixel c);
};

#endif // _UI_WIDGETS_H_
