#include "os.h"
#include "ui.h"

#include <cstdlib> // TODO: This is for exit(). Remove later.

bool mouse_buttons[] = {false, false};
Point pointer; // Updated from EVENT_MOUSE_MOVE or EVENT_MOUSE_BUTTON.
Pixel* screen; // Set by the platform.

const char* WINDOW_TITLE = "PixelGrab";

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
	new UIPixelSelector(Point(0, 0), SCREEN_WIDTH, GRID_HEIGHT, PIXEL_ZOOM),
};

static bool scan_mode = false;
static auto grid = (UIPixelSelector*) wstack[3];

CALLBACK(UIButton, cbExit) {
	exit(0);
}

static void Scan() {
	auto p = pointer.From(-grid->cols / 2, -grid->rows / 2);

	for (int i = 0; i < grid->cols; i++) {
		for (int j = 0; j < grid->rows; j++) {
			int x = p.x + i;
			int y = p.y + j;

			grid->pixels[INDEX(grid->cols, i, j)] = GetPixel(x, y);
		}
	}
	grid->paint = true;
}

static void ScanOn() {
	scan_mode = true;
	GrabMouse();
	SetOwner(grid, EVENT_MOUSE_MOVE, EVENT_MOUSE_BUTTON, EVENT_NULL);
}

static void ScanOff() {
	scan_mode = false;
	ReleaseMouse();
	ReleaseOwner(grid, EVENT_MOUSE_MOVE, EVENT_MOUSE_BUTTON, EVENT_NULL);
	auto t = (UIToggle*) wstack[1];
	t->Off();
}

static void ToggleScan() {
	if (scan_mode)
		ScanOff();
	else
		ScanOn();
}

HANDLER(UIPixelSelector, hScan) {
	if (scan_mode) {
		if (e == EVENT_MOUSE_BUTTON && mouse_buttons[0]) {
			ScanOff();
			return HANDLED_SUCCESS;
		} else if (e == EVENT_MOUSE_MOVE) {
			Scan();
			return HANDLED_SUCCESS;
		}
	}
	return HANDLED_FAILURE;
}

CALLBACK(UIToggle, cbScan) {
	ToggleScan();
	w->On();
}

CALLBACK(UIPixelSelector, cbGrid) {
	auto hf = (HexFloat*) wstack[0];

	if (e == EVENT_MOUSE_BUTTON) {
		Console(hf->color_s);
		Console("\n");
		ToClipboard(hf->color_s);
	}
	hf->Set(w->Get());
}

// Optimizations for scanning.
static void fast_scan(Screen* scr, Event e) {
	auto hf = (HexFloat*) wstack[0];

	if ((e == EVENT_MOUSE_MOVE && PendingEvents() <= 1)) {
		// Avoid drawing every movement
		Scan();
		grid->Draw(scr);
		hf->Set(grid->Get());
		hf->Draw(scr);
	} else if (e == EVENT_MOUSE_BUTTON && mouse_buttons[0]) {
		// We're exiting scan mode
		ScanOff();
		UIDraw(scr, wstack, NUMBER_OF(UIWidget *, wstack));
	}
}

int AppMain(int argc, char **argv) {
	Event e;

	auto scr = new Screen(screen, SCREEN_WIDTH, SCREEN_HEIGHT);

	{ // Draw cool squares in lower right corner.
		auto r = Rect(SCREEN_WIDTH - 15, SCREEN_HEIGHT - 15, 10, 10);
		scr->DrawFill(UI_SURFACE_BG, Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));

		scr->DrawFill(RED, r);
		scr->DrawFill(GREEN, r.From(-11, 0));
		scr->DrawFill(BLUE, r.From(0, -11));

		scr->DrawRect(UI_DARKEST, r);
		scr->DrawRect(UI_DARKEST, r.From(-11, 0));
		scr->DrawRect(UI_DARKEST, r.From(0, -11));
	}

	// Callbacks
	wstack[1]->callback = cbScan;
	wstack[2]->callback = cbExit;
	wstack[3]->callback = cbGrid;

	// Hooks
	wstack[3]->prehook[EVENT_MOUSE_MOVE] = hScan;
	wstack[3]->prehook[EVENT_MOUSE_BUTTON] = hScan;
	
	// Draw inital screen so it's not blank on startup
	UIDraw(scr, wstack, NUMBER_OF(UIWidget *, wstack));
	UpdateWindow();
	
	while ((e = GetEvent()) != EVENT_QUIT) {
		if (e == EVENT_NULL || e == EVENT_LAST)
			continue;

		if (e == EVENT_UPDATE_WINDOW) {
			UpdateWindow();
			continue;
		}

		if (scan_mode) {
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
