#ifndef _UI_H_
#define _UI_H_

#include "common.h"

#define INDEX(xw, x, y) ((xw) * (y) + (x))

struct Screen {
        Screen(int xw, int yw);
        Screen(uint32_t* pixels, int xw, int yw);

	void DrawHLine(Pixel c, Point p, int width);
	void DrawVLine(Pixel c, Point p, int height);
	void DrawFill(Pixel c, Point p0, Point p1);

	int xw, yw;
	uint32_t* pixels;
};

struct PixelGrid {
	PixelGrid(Point position, int xw, int yw, int zoom);
        ~PixelGrid();
	
        void Scan();
	void Draw(Screen* scr);
	void DrawSelection(Screen* scr);
	void Click(int x, int y);
	Pixel Get();

//private:
	Point pos, select;
	int xw, yw;
	int cols, rows, zoom;
	Pixel* pixels;
};

#endif // _UI_H_
