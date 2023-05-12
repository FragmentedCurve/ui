#include "os.h"
#include "ui.h"

#include <cstdlib> // TODO: This is for exit(). Remove later.

bool mouse_buttons[] = {false, false};
Point pointer; // Updated from MOUSE_MOVE or MOUSE_CLICK.
Pixel* screen; // Set by the platform.

#define PHI 1.618 // Golden ratio, because why not?
int PIXEL_ZOOM = 16; 
const int SCAN_HEIGHT = 25;
const int SCAN_WIDTH = SCAN_HEIGHT * PHI;
const int GRID_HEIGHT = PIXEL_ZOOM * SCAN_HEIGHT;
const int SCREEN_HEIGHT = PIXEL_ZOOM * SCAN_HEIGHT + 50;
const int SCREEN_WIDTH = PIXEL_ZOOM * SCAN_WIDTH;

#define newline() Console("\n")

auto p_panel = Point(5, SCREEN_HEIGHT - 40 - 5);

bool scan_mode = false;

Widget *wstack[] = {
	new HexFloat(p_panel.From(100, 0).From(110, 0).From(210, 5)),
	new UIToggle(p_panel, 100, 40),
	new UIToggle(p_panel.From(110, 0), 100, 40),
	new UIButton(p_panel.From(220, 0), 100, 40),
	new UIPixelScanner(Point(0, 0), SCREEN_WIDTH, GRID_HEIGHT, PIXEL_ZOOM),
};

static void callback_exit(Widget *w, Event e) {
	exit(0);
}

static void callback_select_pixel(Widget *w, Event e) {
	auto grid = (UIPixelScanner*) w;
	auto hf = (HexFloat*) wstack[0];
	auto toggle = (UIToggle*) wstack[1];

	scan_mode = grid->ScanMode();
        if (!scan_mode) {
		//Console(hf->color_s);
		//newline();
		ToClipboard(hf->color_s);
		toggle->Off();
        }
	hf->Set(grid->Get());
}

static void callback_scan(Widget *w, Event e) {
	auto b = (UIToggle*) w;
	auto grid = (UIPixelScanner*) wstack[4];
	
	b->Toggle();
	grid->Toggle();
}

// Skip reporting motion events to avoid wasted drawing.
static void SkipMotionQueue() {
	while (PeekEvent() == EVENT_MOUSE_MOVE && PendingEvents() > 1)
		GetEvent();
}

int AppMain(int argc, char **argv) {
	Event e;

	auto scr = new Screen(screen, SCREEN_WIDTH, SCREEN_HEIGHT);
	
        { // Draw cool squares in lower right corner.
		auto p = Point(SCREEN_WIDTH - 5, SCREEN_HEIGHT - 5);
		scr->DrawFill(0xdedbde, Point(0, 0), Point(SCREEN_WIDTH, SCREEN_HEIGHT));
		scr->DrawFill(BLACK, p.From(-10, -10), p);
		scr->DrawRect(BLACK, p.From(-11, 0).From(-10, -10), p.From(-11, 0));
		scr->DrawRect(BLACK, p.From(0, -11).From(-10, -10), p.From(0, -11));
        }
	
        wstack[1]->callback = callback_scan;
        wstack[3]->callback = callback_exit;
	wstack[4]->callback = callback_select_pixel;

        while ((e = GetEvent()) != EVENT_QUIT) {
		if (scan_mode)
			SkipMotionQueue();
		UIDelegate(e, wstack, NUMBER_OF(Widget*, wstack));
		UIDraw(scr, wstack, NUMBER_OF(Widget*, wstack));
		UpdateWindow();
        }

	delete scr;
	
	return 0;
}
