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

#include "ui.h"

/*
  TODO

  - Bitmap drawing and manipulation.

  - Builtin fonts.
 */

void UILight::Draw(UIScreen* scr) {
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

void UIToggle::Draw(UIScreen* scr) {
	UIButton::Draw(scr);
	light.Draw(scr->Subset(r));
}

void HexFloat::Set(UIPixel c) {
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
void HexFloat::DrawFont(UIScreen* scr, bool* gylph, int len, int x0, int y0, UIPixel c) {
	auto zoom = 2;
	for (int i = 0; i < len; i++) {
		auto x = x0 + zoom * (i % 3);
		auto y = y0 + zoom * (i / 3);

		if (gylph[i])
			scr->DrawFill(c /*UI_TEXT_FG*/, UIRect(UIPoint(x, y), UIPoint(x + zoom, y + zoom)));
	}
}

void HexFloat::Draw(UIScreen* scr) {
	// TODO: Clean this up. It was quickly hacked out just to make it work.
	scr->DrawFill(UI_TEXT_BG, r);
	scr->DrawRect(UI_TEXT_BORDER, r);

	for (int i = 1; i < (int)sizeof(color_s) - 1; i++) {
		int c = color_s[i] - '0';
		if (color_s[i] >= 'a')
			c = color_s[i] - 'a' + 0xa;

		UIPixel color;
		if (i == 1 || i == 2)
			color = UI_RGB(0xc0, 0, 0); // 0xc00000;
		else if (i == 3 || i == 4)
			color = UI_RGB(0, 0xc0, 0); // 0x00c000;
		else
			color = UI_RGB(0, 0, 0xc0); // 0xc0;

		DrawFont(scr, gylphs[c], sizeof(gylphs[c]), r.p.x + (5 + 8 * i), r.p.y + 8, color);
	}
}

void UIButton::Draw(UIScreen* scr) {
	if (pressed || pressed_key)
		Draw(scr, UI_SURFACE_SHADOW, UI_SURFACE_HILITE, UI_SURFACE_FG);
	else
		Draw(scr, UI_SURFACE_HILITE, UI_SURFACE_SHADOW, UI_SURFACE_BG);
}

void UIButton::Draw(UIScreen* scr, UIPixel hilite, UIPixel shadow, UIPixel bg) {
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

UIPixelGrid::UIPixelGrid(UIHandle id, UIRect r, int zoom) : UIWidget(id, r) {
	cols = r.xw / zoom;
	rows = r.yw / zoom;
	this->zoom = zoom;
	pixels = new UIPixel[cols * rows];
	for (int i = 0; i < cols * rows; i++)
		pixels[i] = UI_LIGHTEST;
}

UIPixelGrid::~UIPixelGrid() {
	delete[] pixels;
}

UIPoint UIPixelGrid::CellPosition(int x, int y) {
	auto p0 = r.p.From(x * zoom, y * zoom);
	return p0;
}

void UIPixelGrid::Draw(UIScreen* scr) {
	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			DrawCell(scr, i, j);
		}
	}
}

void UIPixelGrid::DrawCell(UIScreen* scr, int x, int y) {
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= cols) x = cols - 1;
	if (y >= rows) y = rows - 1;

	auto p0 = CellPosition(x, y);
	auto p1 = p0.From(zoom, zoom);

	// Fill square
	scr->DrawFill(pixels[UI_INDEX(cols, x, y)], UIRect(p0, p1));

	// Draw border
	scr->DrawHLine(UI_DARKEST, p0, zoom);
	scr->DrawVLine(UI_DARKEST, p0, zoom);

	if (x == cols - 1)
		scr->DrawVLine(UI_DARKEST, p0.From(zoom - 1, 0), zoom);
	if (y == rows - 1)
		scr->DrawHLine(UI_DARKEST, p0.From(0, zoom - 1), zoom);
}

void UIPixelSelector::DrawSelection(UIScreen* scr) {
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

	scr->DrawRect(c, UIRect(p0, p1));
	scr->DrawRect(c, UIRect(q0, q1));
}

void UIPixelSelector::EraseSelection(UIScreen* scr) {
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

/* TODO: Implement
void UIPixelSelector::HandlePress(UIPoint p) {
	if (!pressed)
		return;
	//printf("%d %d", p.x, p.y);
}
*/

void UIPixelSelector::HandleClick(UIPoint p) {
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

void UIPixelSelector::Draw(UIScreen* scr) {
	EraseSelection(scr);
	DrawSelection(scr);

	UIPixelGrid::Draw(scr);
	DrawSelection(scr);
}
