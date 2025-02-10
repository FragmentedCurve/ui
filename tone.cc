#include "ui.h"

const char* WINDOW_TITLE = "DTMF";
const int SCREEN_WIDTH   = 480;
const int SCREEN_HEIGHT  = 4 * (SCREEN_WIDTH / 3);
Pixel* screen;

#include <cstdio>

#define ASTERISK 10
#define POUND    11

int UIMain(int argc, char** argv) {
	UIState s;
	UIWidget* root;
	Screen* scr = new Screen(screen, SCREEN_WIDTH, SCREEN_HEIGHT);

	UIWidget* b;
	{ // GUI Tree
		auto button_w = SCREEN_WIDTH / 3;
		auto button_r = Rect(0, 0, button_w, button_w);

		(root = new UISurface(-1, Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)))
			->Children(
				// Row 1
				new UIButton(1, button_r.p, button_r.xw, button_r.yw),
				new UIButton(2, button_r.p.From(button_w, 0), button_r.xw, button_r.yw),
				new UIButton(3, button_r.p.From(button_w, 0).From(button_w, 0), button_r.xw, button_r.yw),

				// Row 2
				new UIButton(3, button_r.p.From(0, button_w), button_r.xw, button_r.yw),
				(b = new UIButton(4, button_r.p.From(button_w, button_w), button_r.xw, button_r.yw)),
				new UIButton(5, button_r.p.From(button_w, button_w).From(button_w, 0), button_r.xw, button_r.yw),

				// Row 3
				new UIButton(7, button_r.p.From(0, button_w * 2), button_r.xw, button_r.yw),
				new UIButton(8, button_r.p.From(button_w, button_w * 2), button_r.xw, button_r.yw),
				new UIButton(9, button_r.p.From(button_w, button_w * 2).From(button_w, 0), button_r.xw, button_r.yw),

				// Row 4
				new UIButton(ASTERISK, button_r.p.From(0, button_w * 3), button_r.xw, button_r.yw),
				new UIButton(0, button_r.p.From(button_w, button_w * 3), button_r.xw, button_r.yw),
				new UIButton(POUND, button_r.p.From(button_w, button_w * 3).From(button_w, 0), button_r.xw, button_r.yw));

	}

	while (s = UIGetState(), !s.halt) {
		UIOutput out = UIDelegate(s, root);

		if (out.clicked && out.clicked->id > -1) {
			//out.clicked->visible = false;
		}

/*
		switch (out.clicked->id) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			break;
		}
*/

		UIDraw(scr, root);
		UpdateWindow();
	}

	return 0;
}
