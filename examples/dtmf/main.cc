#include <ui.h>

const char* SCREEN_TITLE = "DTMF";
const int SCREEN_WIDTH   = 480;
const int SCREEN_HEIGHT  = 4 * (SCREEN_WIDTH / 3);
UIPixel* screen;

#include <cstdio>

#define ASTERISK 10
#define POUND    11

int UIMain(int argc, char** argv) {
	UIRawInput s;
	UIWidget* root;
	UIScreen* scr = new UIScreen(screen, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH_MAX);

	{ // GUI Tree
		auto button_w = SCREEN_WIDTH / 3;
		auto button_r = UIRect(0, 0, button_w, button_w);

		(root = new UIFillBox(-1, UIRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)))
			->Children(
				// Row 1
				(new UIHBox(-1, UIRect(0, 0, SCREEN_WIDTH, button_w)))
				->Children(
					new UIButton(1, button_r),
					new UIButton(2, button_r),
					new UIButton(3, button_r)),

				// Row 2
				new UIButton(4, button_r.From(0,            button_w)),
				new UIButton(5, button_r.From(button_w,     button_w)),
				new UIButton(6, button_r.From(button_w * 2, button_w)),

				// Row 3
				new UIButton(7, button_r.From(0,            button_w * 2)),
				new UIButton(8, button_r.From(button_w,     button_w * 2)),
				new UIButton(9, button_r.From(button_w * 2, button_w * 2)),

				// Row 4
				new UIButton(ASTERISK, button_r.From(0,            button_w * 3)),
				new UIButton(0,        button_r.From(button_w,     button_w * 3)),
				new UIButton(POUND,    button_r.From(button_w * 2, button_w * 3)));
	}

	root->resize = true;

	while (s = UINativeState(), !s.halt) {
		UIReaction out = UIImpacted(s, root);
		for (auto i = 10; i < 20; i++) {
			auto id = i < 19 ? i - 9 : 0;
			auto w = (UIButton*) root->Find(id);
			if (w) {
				w->pressed_key = s.keys[i];
				w->HandlePress(s.pointer);
			}
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
		UINativeUpdate();
	}

	return 0;
}
