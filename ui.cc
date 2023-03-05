#include "ui.h"
#include "os.h"

PixelGrid::PixelGrid(Point position, int xw, int yw, int zoom) {
	cols = xw / zoom;
	rows = yw / zoom;

	this->xw = xw;
	this->yw = yw;
	this->zoom = zoom;

	select = Point(cols / 2, rows / 2);
      
	pos = position;
	pixels = new uint32_t[cols * rows];
}

PixelGrid::~PixelGrid() {
	delete[] pixels;
}

void PixelGrid::Scan() {
	auto p = Point(pointer.x - (cols / 2), pointer.y - (rows / 2));

	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			int x = p.x + i;
			int y = p.y + j;

			pixels[INDEX(cols, i, j)] = GetPixel(x, y);
		}
        }
}

void PixelGrid::DrawSelection(Screen *scr) {
	// Weird inverted color
	auto c = (0xffffff ^ pixels[INDEX(cols, select.x, select.y)]) | 0xff0000;

	// Top
	scr->DrawHLine(c, Point(select.x * zoom, select.y * zoom), zoom); // Inner
	scr->DrawHLine(c, Point(select.x * zoom - 1, select.y * zoom - 1), zoom + 2); // Outer
	
	// Right
	scr->DrawVLine(c, Point(select.x * zoom + zoom, select.y * zoom), zoom); // Inner
	scr->DrawVLine(c, Point(select.x * zoom + zoom + 1, select.y * zoom -1), zoom + 2); // Outer

	// Bottom
	scr->DrawHLine(c, Point(select.x * zoom + 1, select.y * zoom + zoom), zoom); // Inner
	scr->DrawHLine(c, Point(select.x * zoom, select.y * zoom + zoom + 1), zoom + 2); // Outer

	// Left
	scr->DrawVLine(c, Point(select.x * zoom, select.y * zoom + 1), zoom);
	scr->DrawVLine(c, Point(select.x * zoom - 1, select.y * zoom), zoom + 2);
}

void PixelGrid::Draw(Screen* scr) {
	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			Point p0 = Point(i * zoom, j * zoom);
			Point p1 = Point(i * zoom + zoom, j * zoom + zoom);
			Pixel c = pixels[INDEX(cols, i, j)];
			scr->DrawFill(c, p0, p1);
			scr->DrawHLine(0, Point(0, j * zoom), xw);
		}
		scr->DrawVLine(0, Point(i * zoom, 0), yw);
        }

	// Draw right & bottom boundaries
	scr->DrawVLine(0, Point(pos.x + xw - 1, 0), yw);
	scr->DrawHLine(0, Point(0, pos.y + yw - 1), xw);

	// Highlight selected pixel
	DrawSelection(scr);		
}

void PixelGrid::Click(int x, int y) {
        if (x < pos.x || x > pos.x + xw)
                return;

        if (y < pos.y || y > pos.y + yw)
                return;

	x /= zoom;
	y /= zoom;

	if (x >= cols)
		x = cols - 1;

	if (y >= rows)
		y = rows - 1;
	
	select = Point(x, y);
}

Pixel PixelGrid::Get() {
	return pixels[INDEX(cols, select.x, select.y)];
}
