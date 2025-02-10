#include "ui.h"

#include <cstdarg>
#include <cassert>

/*
  TODO

  - Bitmap drawing and manipulation.

  - Builtin fonts.

  - Implement machine states.  

  - Reimplement all handlers based on machine states.

  - Divide headers into ui_core.h, ui_widgets.h, ui_platform.h, ui_style.h and maybe ui_assets.h.
 */
#include <iostream>

UIOutput UIDelegate(UIState state, UIWidget* root) {
	UIOutput out = {
		.pressed = NULL,
		.clicked = NULL,
	};

	static UIState pstate; // Previous state
	static UIOutput pout;  // Previous output

	// TODO: General for more than left click Handle primary
	// pointer clicks
	if (state.m[0] && !pstate.m[0]) {
		// Something was pressed.
		out.pressed = root->Hit(state.pointer);
		if (out.pressed) {
			out.pressed->pressed = true;
			out.pressed->HandlePress(state.pointer);
		}
	} else if (!state.m[0] && pstate.m[0]) {
		// Something might've been clicked.
		if (pout.pressed && (root->Hit(state.pointer) == pout.pressed)) {
			out.clicked = pout.pressed;
			out.clicked->HandleClick(state.pointer);
		}

		// Otherwise, no click, only a release
		if (pout.pressed) {
			pout.pressed->pressed = false;
		}
	} else {
		// Previously pressed persists
		out.pressed = pout.pressed;
	}

	// TODO: Should drags be a property and handled here?

	if (state.screen_width != pstate.screen_width || state.screen_height != pstate.screen_height) {
		// Screen was resized.
		// TODO: Implement layout logic & classes
		int dx = state.screen_width - pstate.screen_width;
		int dy = state.screen_height - pstate.screen_height;
		root->Resize(dx, dy);
	}

	pstate = state;
	pout = out;
	return out;
}

void UIDraw(Screen* scr, UIWidget* root) {
	if (!root || !root->visible)
		return;

	root->Draw(scr);
	scr = scr->Subset(root->r);

	for (UIWidget* walk = root->childhead; walk; walk = walk->next) {
		UIDraw(scr, walk);
	}
}

void UILight::Draw(Screen* scr) {
	scr->DrawFill(on ? UI_LIGHT_ON : UI_LIGHT_OFF, r.From(2, 2).Resize(-2, -2));

	if (!on) {
		// Draw a thick shadow when the light is off.
		scr->DrawFill((on ? UI_LIGHT_ON : UI_LIGHT_OFF) << 1, r.Height(4));  // Left
		scr->DrawFill((on ? UI_LIGHT_ON : UI_LIGHT_OFF) << 1, r.Width(4));   // Top
	}

	// Top hilite
	scr->DrawHLine(UI_SURFACE_SHADOW, r.p, r.xw - 1);                   // Outer
	scr->DrawHLine(UI_SURFACE_SHADOW, r.p.From(0, 1), r.xw - 2);        // Inner

	// Left hilite
	scr->DrawVLine(UI_SURFACE_SHADOW, r.p, r.yw - 1);                   // Outer
	scr->DrawVLine(UI_SURFACE_SHADOW, r.p.From(1, 0), r.yw - 2);        // Inner

	// Bottom shadow
	scr->DrawHLine(UI_SURFACE_HILITE, r.p.From(1, r.yw - 1), r.xw - 1); // Outer
	scr->DrawHLine(UI_SURFACE_HILITE, r.p.From(2, r.yw - 2), r.xw - 2); // Inner

	// Right shadow
	scr->DrawVLine(UI_SURFACE_HILITE, r.p.From(r.xw - 1, 1), r.yw - 1); // Outer
	scr->DrawVLine(UI_SURFACE_HILITE, r.p.From(r.xw - 2, 2), r.yw - 2); // Inner
}

void UIToggle::Draw(Screen* scr) {
	UIButton::Draw(scr);
	light.Draw(scr->Subset(r));
}

void HexFloat::Set(Pixel c) {
	const char* hexmap = "0123456789abcdef";
	char* buf = color_s;
	
	// Remove bytes that aren't RGB
	c &= 0x00ffffff;

	// Initialize string to "#000000"
	((int*)buf)[0] = *((int*)"#000");
	((int*)buf)[1] = *((int*)"000");
	
	buf = buf + 6;

	while (c) {
		*buf = hexmap[c % 0x10];
		c /= 0x10;
		buf--;
	}
}

// TODO: Clean this up. It was quickly hacked out just to make it work.
void HexFloat::DrawFont(Screen* scr, bool* gylph, int len, int x0, int y0, Pixel c) {
	auto zoom = 2;
	for (int i = 0; i < len; i++) {
		auto x = x0 + zoom * (i % 3);
		auto y = y0 + zoom * (i / 3);

		if (gylph[i])
		  scr->DrawFill(c /*UI_TEXT_FG*/, Rect(Point(x, y), Point(x + zoom, y + zoom)));
	}
}

void HexFloat::Draw(Screen* scr) {
	// TODO: Clean this up. It was quickly hacked out just to make it work.
	scr->DrawFill(UI_TEXT_BG, r);
	scr->DrawRect(UI_TEXT_BORDER, r);

	for (int i = 1; i < (int)sizeof(color_s) - 1; i++) {
		int c = color_s[i] - '0';
		if (color_s[i] >= 'a')
			c = color_s[i] - 'a' + 0xa;

		Pixel color;
		if (i == 1 || i == 2)
			color = UI_RGB(0xc0, 0, 0); // 0xc00000;
		else if (i == 3 || i == 4)
			color = UI_RGB(0, 0xc0, 0); // 0x00c000;
		else
			color = UI_RGB(0, 0, 0xc0); // 0xc0;

		DrawFont(scr, gylphs[c], sizeof(gylphs[c]), r.p.x + (5 + 8 * i), r.p.y + 8, color);
	}
}

void UIButton::Draw(Screen* scr) {
	if (pressed)
		Draw(scr, UI_SURFACE_SHADOW, UI_SURFACE_HILITE, UI_SURFACE_FG);
	else
		Draw(scr, UI_SURFACE_HILITE, UI_SURFACE_SHADOW, UI_SURFACE_BG);
}

void UIButton::Draw(Screen* scr, Pixel hilite, Pixel shadow, Pixel bg) {
	scr->DrawFill(bg, r);

	// Top hilite
	scr->DrawHLine(hilite, r.p.From(0, 1), r.xw - 2);        // Outer
	scr->DrawHLine(hilite, r.p.From(0, 2), r.xw - 3);        // Inner

	// Left hilite
	scr->DrawVLine(hilite, r.p.From(1, 0), r.yw - 2);        // Outer
	scr->DrawVLine(hilite, r.p.From(2, 0), r.yw - 3);        // Inner

	// Bottom shadow
	scr->DrawHLine(shadow, r.p.From(2, r.yw - 2), r.xw - 2); // Outer
	scr->DrawHLine(shadow, r.p.From(3, r.yw - 3), r.xw - 3); // Inner

	// Right shadow
	scr->DrawVLine(shadow, r.p.From(r.xw - 2, 2), r.yw - 2); // Outer
	scr->DrawVLine(shadow, r.p.From(r.xw - 3, 3), r.yw - 3); // Inner

	// Border
	scr->DrawRect(UI_DARKEST, r);
}

UIPixelGrid::UIPixelGrid(UIHandle id, Point position, int xw, int yw, int zoom) : UIWidget(id, position, xw, yw) {
	cols = xw / zoom;
	rows = yw / zoom;
	this->zoom = zoom;
	pixels = new Pixel[cols * rows];
	for (int i = 0; i < cols * rows; i++)
		pixels[i] = UI_LIGHTEST;
}

UIPixelGrid::~UIPixelGrid() {
	delete[] pixels;
}

Point UIPixelGrid::CellPosition(int x, int y) {
	auto p0 = r.p.From(x * zoom, y * zoom);	
	return p0;
}

void UIPixelGrid::Draw(Screen* scr) {
	for (int i = 0; i < cols; i++)
		for (int j = 0; j < rows; j++)
			DrawCell(scr, i, j);
}

void UIPixelGrid::DrawCell(Screen* scr, int x, int y) {
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= cols) x = cols - 1;
	if (y >= rows) y = rows - 1;

	auto p0 = CellPosition(x, y);
	auto p1 = p0.From(zoom, zoom);

	// Fill square
	scr->DrawFill(pixels[UI_INDEX(cols, x, y)], Rect(p0, p1));

	// Draw border
	scr->DrawHLine(UI_DARKEST, p0, zoom);
	scr->DrawVLine(UI_DARKEST, p0, zoom);

	if (x == cols - 1)
		scr->DrawVLine(UI_DARKEST, p0.From(zoom - 1, 0), zoom);
	if (y == rows - 1)
		scr->DrawHLine(UI_DARKEST, p0.From(0, zoom - 1), zoom);
}

void UIPixelSelector::DrawSelection(Screen* scr) {
	// Weird inverted color
	auto c = (UI_WHITE ^ pixels[UI_INDEX(cols, select.x, select.y)]) | UI_RED;

	auto p0 = CellPosition(select.x, select.y);
	auto p1 = p0.From(zoom + 1, zoom + 1);

	auto q0 = p0.From(-1, -1);
	auto q1 = p1.From(1, 1);

	// Boundary corrections
	if (select.x == cols - 1)
		p1.x = q1.x -= 2;

	if (select.y == rows - 1)
		p1.y = q1.y -= 2;

	if (select.x == 0)
		q0.x++;

	if (select.y == 0)
		q0.y++;

	scr->DrawRect(c, Rect(p0, p1));
	scr->DrawRect(c, Rect(q0, q1));
}

void UIPixelSelector::EraseSelection(Screen* scr) {
	DrawCell(scr, prev_select.x, prev_select.y);
	DrawCell(scr, prev_select.x - 1, prev_select.y - 1);
	DrawCell(scr, prev_select.x, prev_select.y - 1);
	DrawCell(scr, prev_select.x + 1, prev_select.y - 1);
	DrawCell(scr, prev_select.x + 1, prev_select.y);
	DrawCell(scr, prev_select.x + 1, prev_select.y + 1);
	DrawCell(scr, prev_select.x, prev_select.y + 1);
	DrawCell(scr, prev_select.x - 1, prev_select.y + 1);
	DrawCell(scr, prev_select.x - 1, prev_select.y);
}

void UIPixelSelector::HandleClick(Point p) {
/*
	auto p = pointer.From(r.p);
	p.x /= zoom;
	p.y /= zoom;

	// Make sure we're not beyond grid boundaries
	if (p.x < 0) p.x = 0;
	if (p.y < 0) p.y = 0;
	if (p.x >= cols) p.x = cols - 1;
	if (p.y >= rows) p.y = rows - 1;

	prev_select = select;
	select = p;
*/
}

void UIPixelSelector::Draw(Screen* scr) {
	EraseSelection(scr);
	DrawSelection(scr);

	UIPixelGrid::Draw(scr);
	DrawSelection(scr);
}
