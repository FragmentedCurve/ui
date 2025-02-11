#include "ui.h"

#include <cstdio>
#include <cstdlib> // TODO: This is for exit(). Remove later.

#define PHI 1.618 // Golden ratio, because why not?
int PIXEL_ZOOM = 16;
const int SCAN_HEIGHT = 25;
const int SCAN_WIDTH = SCAN_HEIGHT * PHI;
const int GRID_HEIGHT = PIXEL_ZOOM * SCAN_HEIGHT;

const char* WINDOW_TITLE = "PixelGrab";
const int SCREEN_WIDTH   = PIXEL_ZOOM * SCAN_WIDTH;
const int SCREEN_HEIGHT  = PIXEL_ZOOM * SCAN_HEIGHT + 50;

UIPoint pointer;
bool mouse_buttons[2];
UIPixel* screen;

static bool scan_mode = false;

/*
CALLBACK(UIButton, cbExit) {
	exit(0);
}

#include <ctime>

static void Scan() {
	auto p = pointer.From(-grid->cols / 2, -grid->rows / 2);

	time_t start, end, avg;
	
	for (int i = 0; i < grid->cols; i++) {
		for (int j = 0; j < grid->rows; j++) {
			int x = p.x + i;
			int y = p.y + j;

			start = clock();
			grid->pixels[INDEX(grid->cols, i, j)] = GetPixel(x, y);
			end = clock();
			avg += (end - start) / (grid->cols * grid->rows);
		}
	}

	//printf("AVG TIME PER PIXEL: %ld\n", avg);
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
*/

struct Logo : UIWidget {
	Logo(UIHandle id, UIRect r) : UIWidget(id, r) {}
	
	void Draw(UIScreen* scr) {
		auto pad = 1;
		auto width = 10; //r.xw / 2 - pad;
		auto box = UIRect(0, 0, width, width);
		
		scr->DrawFill(UI_RED, box.From(width + pad, width + pad).From(r.p));
		scr->DrawFill(UI_GREEN, box.From(0, width + pad).From(r.p));
		scr->DrawFill(UI_BLUE, box.From(width + pad, 0).From(r.p));

		scr->DrawRect(UI_DARKEST, box.From(width + pad, width + pad).From(r.p));
		scr->DrawRect(UI_DARKEST, box.From(0, width + pad).From(r.p));
		scr->DrawRect(UI_DARKEST, box.From(width + pad, 0).From(r.p));
	}
};

enum Handles {
	ID_ANON,
	ID_ROOT,
	ID_PANEL,
	ID_GRID,
	ID_BTOGGLE,
	ID_BEXIT,
};

int UIMain(int argc, char **argv) {
	UIRawInput state;
	UIWidget* root;

	auto scr = UIScreen(screen, SCREEN_WIDTH, SCREEN_HEIGHT);

	// GUI Tree
	root = new UIWidget(ID_ROOT, UIPoint(0, 0), SCREEN_WIDTH, SCREEN_HEIGHT);
	root->Children(
		new HexFloat(ID_ANON, UIPoint(0, 0)),
		new UIPixelSelector(ID_GRID, UIPoint(0, 0), root->r.xw, GRID_HEIGHT, PIXEL_ZOOM),
		(new UIPanel(ID_ANON, UIRect(0, GRID_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - GRID_HEIGHT)))
		->Children(
			new UIToggle(ID_BTOGGLE, UIPoint(5, 5), 100, 40),
			new UIButton(ID_BEXIT, UIPoint(110, 5), 100, 40),
			new Logo(ID_ANON, UIRect(SCREEN_WIDTH - (SCREEN_HEIGHT - GRID_HEIGHT) / 2,
						(SCREEN_HEIGHT - GRID_HEIGHT) / 2,
						(SCREEN_HEIGHT - GRID_HEIGHT) / 2,
						(SCREEN_HEIGHT - GRID_HEIGHT) / 2))));

	while (state = UINativeState(), !state.halt) {
		UIReaction out = UIImpacted(state, root);

		if (out.clicked) {
			switch (out.clicked->id) {
			case ID_BTOGGLE: {
				// TODO: Capture mouse
				UINativeConsole(((UIToggle*) out.clicked)->light.on ? "On\n" : "Off\n");
			} break;
			case ID_BEXIT: {
				goto halt;
			} break;
			}
		}

		UIDraw(&scr, root);
		UINativeUpdate();
	}

halt:
	return 0;
}
