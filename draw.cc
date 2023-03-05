#include "ui.h"

Screen::Screen(int xw, int yw) {
	this->xw = xw;
	this->yw = yw;
	pixels = new uint32_t[xw * yw];
}

Screen::Screen(uint32_t* pixels, int xw, int yw) {
	this->xw = xw;
	this->yw = yw;
	this->pixels = pixels;
}

void Screen::DrawHLine(Pixel c, Point p, int width) {
  	// Completely off the screen
	if (p.x > xw || p.y < 0 || p.y > xw)
		return;

	// Clip top
        if (p.x < 0) {
		width += p.x;
                p.x = 0;
        }

        // Clip bottom
        if (p.x + width > xw)
		width = xw - p.x;

	while (--width >= 0)
		pixels[INDEX(xw, p.x + width, p.y)] = c;
}

void Screen::DrawVLine(Pixel c, Point p, int height) {
	// Completely off the screen
	if (p.y > yw || p.x < 0 || p.x > xw)
		return;

	// Clip top
        if (p.y < 0) {
		height += p.y;
                p.y = 0;
        }

        // Clip bottom
        if (p.y + height > yw)
		height = yw - p.y;
	
        while (--height >= 0)
		pixels[INDEX(xw, p.x, p.y + height)] = c;
}

void Screen::DrawFill(Pixel c, Point p0, Point p1) {
	int xw = p1.x - p0.x;
	int yw = p1.y - p0.y;

	while (--yw >= 0)
		DrawHLine(c, Point(p0.x, p0.y + yw), xw);
}
