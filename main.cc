#include "os.h"
#include "ui.h"

#include <cstdio>
#include <cstdlib> // TODO: This is for exit(). Remove later.

#define PHI 1.618 // Golden ratio, because why not?
int PIXEL_ZOOM = 16;
const int SCAN_HEIGHT = 25;
const int SCAN_WIDTH = SCAN_HEIGHT * PHI;
const int GRID_HEIGHT = PIXEL_ZOOM * SCAN_HEIGHT;

UI_DEFINE_GLOBALS("PixelGrab",                    // Window Title
		  PIXEL_ZOOM * SCAN_WIDTH,        // Window width
		  PIXEL_ZOOM * SCAN_HEIGHT + 50); // Window  height

static bool scan_mode = false;
// TODO: REMOVE grid
//static auto grid = (UIPixelSelector*) wstack[3];

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
	Logo(UIWidget* parent, Rect r) : UIWidget(parent, r) {}
	
	void Draw(Screen* scr) {
		auto pad = 1;
		auto width = 10; //r.xw / 2 - pad;
		auto box = Rect(0, 0, width, width);
		
		scr->DrawFill(RED, box.From(width + pad, width + pad).From(r.p));
		scr->DrawFill(GREEN, box.From(0, width + pad).From(r.p));
		scr->DrawFill(BLUE, box.From(width + pad, 0).From(r.p));
		
		scr->DrawRect(UI_DARKEST, box.From(width + pad, width + pad).From(r.p));
		scr->DrawRect(UI_DARKEST, box.From(0, width + pad).From(r.p));
		scr->DrawRect(UI_DARKEST, box.From(width + pad, 0).From(r.p));
	}
};

int AppMain(int argc, char **argv) {
	Event e;
	UIWidget* root;

	auto scr = new Screen(screen, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	{ // GUI Tree
		root = new UIWidget(NULL, Point(0, 0), SCREEN_WIDTH, SCREEN_HEIGHT);
		
		auto colorlabel = new HexFloat(root, Point(0, 0));
		auto grid = new UIPixelSelector(root, Point(0, 0), root->r.xw, GRID_HEIGHT, PIXEL_ZOOM);

		auto panel = new UIPanel(root, Rect(0, grid->r.yw, SCREEN_WIDTH, SCREEN_HEIGHT - grid->r.yw));
		new UIToggle(panel, Point(5, 5), 100, 40);
		new UIButton(panel, Point(110, 5), 100, 40);

		new Logo(panel, Rect(panel->r.xw - (panel->r.yw / 2),
					panel->r.yw / 2,
					panel->r.yw / 2,
					panel->r.yw / 2));
	}

	/*
	// Callbacks
	wstack[1]->callback = cbScan;
	wstack[2]->callback = cbExit;
	wstack[3]->callback = cbGrid;

	// Hooks
	wstack[3]->prehook[EVENT_MOUSE_MOVE] = hScan;
	wstack[3]->prehook[EVENT_MOUSE_BUTTON] = hScan;
	*/
	
	// Draw inital screen so it's not blank on startup
	UIDraw(scr, root);
	UpdateWindow();

	while ((e = GetEvent()) != EVENT_QUIT) {
		if (e == EVENT_NULL || e == EVENT_LAST)
			continue;

		if (e == EVENT_UPDATE_WINDOW) {
			UpdateWindow();
			continue;
		}

		if (scan_mode) {
			//fast_scan(scr, e);
		} else {
			UIDelegate(e, root);
			UIDraw(scr, root);
		}

		UpdateWindow();
	}

	delete scr;
	return 0;
}
