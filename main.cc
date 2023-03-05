#include "os.h"
#include "ui.h"

#include "font.h"

bool scan_mode = false;
Point pointer; // Updated from MOUSE_MOVE or MOUSE_CLICK.
Pixel* screen; // Set by the platform.

#define PHI 1.618 // Golden ratio, because why not?
int PIXEL_ZOOM = 16; 
const int SCAN_HEIGHT = 25;
const int SCAN_WIDTH = SCAN_HEIGHT * PHI;
const int SCREEN_HEIGHT =  PIXEL_ZOOM * SCAN_HEIGHT;
const int SCREEN_WIDTH = PIXEL_ZOOM * SCAN_WIDTH;

extern Pixel* splash;

#define newline() Console("\n")

// Skip reporting motion events to avoid wasted drawing.
static void SkipMotionQueue() {
	while (PeekEvent() == MOUSE_MOVE && PendingEvents() > 1)
		GetEvent();
}

void ToString(Pixel c, char buf[8]) {
	const char* hexmap = "0123456789abcdef";

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
static void DrawFont(Screen* scr, bool *gylph, int len, int x0, int y0) {
	auto zoom = 2;
        for (int i = 0; i < len; i++) {
		auto x = x0 + zoom * (i % 3);
		auto y = y0 + zoom * (i / 3);

                if (gylph[i])
			scr->DrawFill(BLACK, Point(x, y), Point(x + zoom, y + zoom));
        }
}

int AppMain(int argc, char **argv) {
	Event e;

	char color_s[8];
	
	auto scr = new Screen(screen, SCREEN_WIDTH, SCREEN_HEIGHT);
	auto grid = new PixelGrid(Point(0, 0), SCREEN_WIDTH, SCREEN_HEIGHT, PIXEL_ZOOM);

	// Clear 
	((int*)color_s)[0] = 0;
	((int*)color_s)[1] = 0;
	
        // White out the window
	scr->DrawFill(WHITE, Point(0, 0), Point(scr->xw, scr->yw));
        //UpdateWindow();

	// Initialize with #FFFFFF
	ToString(WHITE, color_s);

	// Draw splash screen
        for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
		scr->pixels[i] = splash[i];
        
        while ((e = GetEvent()) != QUIT) {
                switch (e) {
                case MOUSE_MOVE: {
			// We're not scanning the screen right now
			if (!scan_mode)
				break;

			// Avoid costly drawing that won't be seen
			SkipMotionQueue();

			grid->Scan();
			grid->Draw(scr);

			// Update the pixel value as we're scanning
			// the screen
			ToString(grid->Get(), color_s);
                } break;

                case MOUSE_RIGHT: {
                        if (!scan_mode) {
                                GrabMouse();
                                scan_mode = true;
                        }
                } break;

                case MOUSE_LEFT: {
                        if (scan_mode) {
                                ReleaseMouse();
                                ToString(grid->Get(), color_s);
				ToClipboard(color_s);
				Console(color_s);
				newline();
                                scan_mode = false;
                        } else {
                                grid->Click(pointer.x, pointer.y);
                                grid->Draw(scr);
				ToString(grid->Get(), color_s);
				ToClipboard(color_s);
				Console(color_s);
				newline();
                        }
                } break;

                case KEY_PRESS: {
			ReleaseMouse();
			scan_mode = false;
                } break;
                }

		// TODO: Clean this up. It was quickly hacked out just to make it work.
		scr->DrawFill(0x2e2e2e, Point(0,0), Point(48 + 25 + 2, 25 + 2));
		scr->DrawFill(WHITE, Point(0,0), Point(48 + 25, 25));
                for (int i = 1; i < (int) sizeof(color_s) - 1; i++) {
			int c = color_s[i] - '0';
			if (color_s[i] >=  'a')
				c = color_s[i] - 'a' + 0xa;
                        DrawFont(scr, gylphs[c], sizeof(gylphs[c]), 5 + 8 * i, 5);
                }

		UpdateWindow();
        }

	delete scr;
	delete grid;
	
	return 0;
}
