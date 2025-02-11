#include "ui.h"

const char* WINDOW_TITLE = "test";
const int SCREEN_WIDTH = 2000;
const int SCREEN_HEIGHT = 2000;

Pixel* screen;

#include <cstdio>

enum { TOGGLE, BUTTON };

int UIMain(int argc, char** argv) {
	UIRawInput state;

	// GUI Tree
	UIWidget* root = new UISurface(-1, Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
	UIButton* b = new UIButton(BUTTON, Point(400, 400), 500, 100);
	UIToggle* t = new UIToggle(TOGGLE, Point(10, 10), 100, 50);

	root->Children(t, b);

	Screen* scr = new Screen(screen, 1000, 1000, SCREEN_WIDTH);

	while (state = UINativeState(), !state.halt) {
		UIReaction out = UIImpacted(state, root);

		if (out.clicked) {
			switch (out.clicked->id) {
			case TOGGLE: {
				b->visible = !b->visible;
			} break;

			case BUTTON: {
				t->Resize(10, 10);
			} break;
			}
		} else if (out.pressed) {
			if (out.pressed->id == BUTTON) {
				b->Push(state.dpointer);
			}
		}

		UIDraw(scr, root);
		UINativeUpdate();
	}

	return 0;
}
