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

struct UIFillBox : UIWidget {
	UIFillBox(UIHandle id, UIRect r) : UIWidget(id, r) {
		fill_screen = true;
	}

	virtual UIWidget* SetSize(int xw, int yw) {
		UIWidget::SetSize(xw, yw);
		if (childhead) {
			childhead->SetSize(xw, yw);
		}
		return this;
	}
};

struct UIHBox : UIWidget {
	UIHBox(UIHandle id) : UIWidget(id) {}
	UIHBox(UIHandle id, UIRect r) : UIWidget(id, r) {}

	virtual UIWidget* SetSize(int xw, int yw) {
		UIWidget::SetSize(xw, yw);
		int count = 0;

		for (UIWidget* w = childhead; w; w = w->next) {
			count++;
		}

		auto child_width = r.xw / count;
		auto child_r = UIRect(0, 0, child_width, r.yw);

		for (UIWidget* w = childtail; w; w = w->prev) {
			w->r = child_r;
			w->SetSize(child_r.xw, child_r.yw);
			child_r = child_r.From(child_width, 0);
		}

		return this;
	}
};

struct UIVBox : UIWidget {
	UIVBox(UIHandle id) : UIWidget(id) {}
	UIVBox(UIHandle id, UIRect r) : UIWidget(id, r) {}

	virtual UIWidget* SetSize(int xw, int yw) {
		UIWidget::SetSize(xw, yw);
		int count = 0;

		for (UIWidget* w = childhead; w; w = w->next) {
			count++;
		}

		auto child_height = r.yw / count;
		auto child_r = UIRect(0, 0, r.xw, child_height);

		for (UIWidget* w = childtail; w; w = w->prev) {
			w->r = child_r;
			w->SetSize(child_r.xw, child_r.yw);
			child_r = child_r.From(0, child_height);
		}

		return this;
	}
};

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
	UIButton(UIHandle id) : UIWidget(id) {}
	UIButton(UIHandle id, UIRect r) : UIWidget(id, r) {}
	void Draw(UIScreen* scr);

	// Properties
	char* caption = NULL;
	UIBitmap icon = {0}; // TODO: Different sizes?
};

struct UIToggle : UIButton {
  	UIToggle(UIHandle id)
		: UIButton(id), light(id, UIRect(8, 8, 16, r.yw - 16)) {
	}

	UIToggle(UIHandle id, UIRect r)
		: UIButton(id, r), light(id, UIRect(8, 8, 16, r.yw - 16)) {
	}

	virtual void Move(int x, int y) {
		UIWidget::Move(x, y);
		light.Move(x, y);
	}

	virtual void Resize(int dx, int dy) {
		UIWidget::Resize(dx, dy);
		light.Resize(0, dy);
	}

	virtual UIWidget* SetSize(int xw, int yw) {
		light.r.yw = yw - 16;
		return UIWidget::SetSize(xw, yw);
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
	UIPixelGrid(UIHandle id, UIRect r, int zoom);
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
	UIPixelSelector(UIHandle id, UIRect r, int zoom)
		: UIPixelGrid(id, r, zoom) {
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
	HexFloat(UIHandle id) : UIWidget(id, UIRect(0, 0, 48 + 25 + 2, 25 + 2)) { Set(UI_LIGHTEST); }
	HexFloat(UIHandle id, UIPoint p) : UIWidget(id, UIRect(p, 48 + 25 + 2, 25 + 2)) { Set(UI_LIGHTEST); }
	void Draw(UIScreen* scr);
	void Set(UIPixel c);

	char color_s[8];

private:
	void DrawFont(UIScreen* scr, bool *gylph, int len, int x0, int y0, UIPixel c);
};
