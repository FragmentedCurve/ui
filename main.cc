#include "os.h"
#include "ui.h"

#include <cstdlib> // TODO: This is for exit(). Remove later.

bool mouse_buttons[] = {false, false};
Point pointer; // Updated from EVENT_MOUSE_MOVE or EVENT_MOUSE_BUTTON.
Pixel* screen; // Set by the platform.

#define PHI 1.618 // Golden ratio, because why not?
int PIXEL_ZOOM = 16; 
const int SCAN_HEIGHT = 25;
const int SCAN_WIDTH = SCAN_HEIGHT * PHI;
const int GRID_HEIGHT = PIXEL_ZOOM * SCAN_HEIGHT;
const int SCREEN_HEIGHT = PIXEL_ZOOM * SCAN_HEIGHT + 50;
const int SCREEN_WIDTH = PIXEL_ZOOM * SCAN_WIDTH;

static auto p_panel = Point(5, SCREEN_HEIGHT - 40 - 5);

UIWidget *wstack[] = {
	new HexFloat(Point(0, 0)),
	new UIToggle(p_panel, 100, 40),
	new UIButton(p_panel.From(110, 0), 100, 40),
	new UIPixelScanner(Point(0, 0), SCREEN_WIDTH, GRID_HEIGHT, PIXEL_ZOOM),
};

static void callback_exit(UIWidget *w, Event e, void* data) {
	exit(0);
	UNUSED(data);
}

static void callback_grid(UIWidget *w, Event e, void* data) {
	auto grid = (UIPixelScanner*) w;
	auto hf = (HexFloat*) wstack[0];
	auto toggle = (UIToggle*) wstack[1];
	auto prev_state = (bool*) data;
	
	if (!grid->ScanMode()) {
		if (e == EVENT_MOUSE_BUTTON && (!mouse_buttons[0] || prev_state)) {
			Console(hf->color_s);
			Console("\n");
			ToClipboard(hf->color_s);
			ReleaseOwner(grid);
		}
		toggle->Off();
	}
	hf->Set(grid->Get());
}

static void callback_scan(UIWidget *w, Event e, void* data) {
	auto b = (UIToggle*) w;
	auto grid = (UIPixelScanner*) wstack[3];

	grid->Toggle();
	b->Toggle();

	UNUSED(data);
}

// Optimizations for scanning.
static void fast_scan(Screen* scr, Event e) {
	auto grid = (UIPixelScanner*) wstack[3];
	auto hf = (HexFloat*) wstack[0];

	if ((e == EVENT_MOUSE_MOVE && PendingEvents() <= 1)) {
		// Avoid drawing every movement
		grid->Handle(e);
		grid->Draw(scr);
		hf->Set(grid->Get());
		hf->Draw(scr);
	} else if (e == EVENT_MOUSE_BUTTON && mouse_buttons[0]) {
		// We're exiting scan mode
		grid->Handle(e);
		UIDraw(scr, wstack, NUMBER_OF(UIWidget *, wstack));
	}
}

int AppMain(int argc, char **argv) {
	Event e;

	auto scr = new Screen(screen, SCREEN_WIDTH, SCREEN_HEIGHT);

	{ // Draw cool squares in lower right corner.
		auto r = Rect(SCREEN_WIDTH - 15, SCREEN_HEIGHT - 15, 10, 10);
		scr->DrawFill(UI_SURFACE_BG, Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));

		scr->DrawFill(0xff0000, r);
		scr->DrawFill(0x00ff00, r.From(-11, 0));
		scr->DrawFill(0x0000ff, r.From(0, -11));

		scr->DrawRect(UI_DARKEST, r);
		scr->DrawRect(UI_DARKEST, r.From(-11, 0));
		scr->DrawRect(UI_DARKEST, r.From(0, -11));
	}

	wstack[1]->callback = callback_scan;
	wstack[2]->callback = callback_exit;
	wstack[3]->callback = callback_grid;

	// Draw inital screen so it's not blank on startup
	UIDraw(scr, wstack, NUMBER_OF(UIWidget *, wstack));

	auto grid = (UIPixelScanner*) wstack[3];
	while ((e = GetEvent()) != EVENT_QUIT) {
		if (e == EVENT_NULL || e == EVENT_LAST)
			continue;

		if (e == EVENT_UPDATE_WINDOW) {
			UpdateWindow();
			continue;
		}

		if (grid->ScanMode()) {
			fast_scan(scr, e);
		} else {
			UIDelegate(e, wstack, NUMBER_OF(UIWidget *, wstack));
			UIDraw(scr, wstack, NUMBER_OF(UIWidget *, wstack));
		}

		UpdateWindow();
	}

	delete scr;

	return 0;
}
