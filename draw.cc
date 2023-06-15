#include "ui.h"

Screen::Screen(int xw, int yw): Screen(new uint32_t[xw * yw], xw, yw) {}
Screen::Screen(uint32_t* pixels, int xw, int yw) : xw(xw), yw(yw), pixels(pixels)  {}

void Screen::DrawHLine(Pixel c, Point p, int width) {
	DrawHLine(c, p, width, Rect(0, 0, xw, yw));
}

void Screen::DrawHLine(Pixel c, Point p, int width, Rect clip) {
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
		pixels[INDEX(xw, p.x + width, p.y)] = c;
}

void Screen::DrawVLine(Pixel c, Point p, int height) {
	DrawVLine(c, p, height, Rect(0, 0, xw, yw));
}

void Screen::DrawVLine(Pixel c, Point p, int height, Rect clip) {
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
		pixels[INDEX(xw, p.x, p.y + height)] = c;
}

void Screen::DrawFill(Pixel c, Point p0, Point p1) {
	DrawFill(c, Rect(p0, p1));
}

void Screen::DrawFill(Pixel c, Rect r) {
	for (int i = 0; i < r.yw; i++)
		DrawHLine(c, r.p.From(0, i), r.xw);
}

void Screen::DrawRect(Pixel c, Point p0, Point p1) {
	DrawRect(c, Rect(p0, p1));
}

void Screen::DrawRect(Pixel c, Rect r) {
	// Top
	DrawHLine(c, r.p, r.xw);
	// Bottom
	DrawHLine(c, r.p.From(0, r.yw - 1), r.xw);
	// Left
	DrawVLine(c, r.p, r.yw);
	// Right
	DrawVLine(c, r.p.From(r.xw - 1, 0), r.yw);
}
