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

	// GUI Tree
	(root = new UIVBox(-1))
		->Children(
			// Row 1
			(new UIHBox(-1))
			->Children(
				new UIButton(1),
				new UIButton(2),
				new UIButton(3)),
			// Row 2
			(new UIHBox(-1))
			->Children(
				new UIButton(4),
				new UIButton(5),
				new UIButton(6)),

			// Row 3
			(new UIHBox(-1))
			->Children(
				new UIButton(7),
				new UIButton(8),
				new UIButton(9)),
			// Row 4
			(new UIHBox(-1))
			->Children(
				new UIButton(ASTERISK),
				new UIButton(0),
				new UIButton(POUND)));
	root->fill_screen = true;

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
