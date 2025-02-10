#include "ui.h"

const char* WINDOW_TITLE = "test";
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

Pixel* screen;

#include <cstdio>

enum { TOGGLE, BUTTON };

int UIMain(int argc, char** argv) {
	UIState state;

	// GUI Tree
	UIWidget* root = new UISurface(-1, Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
	UIButton* b = new UIButton(BUTTON, Point(400, 400), 500, 100);
	UIToggle* t = new UIToggle(TOGGLE, Point(10, 10), 100, 50);

	root->Children(t, b);

	Screen* scr = new Screen(screen, SCREEN_WIDTH, SCREEN_HEIGHT);

	while (state = UIGetState(), !state.halt) {
		UIOutput out = UIDelegate(state, root);

		if (out.clicked) {
			Console("HERE I AM \n");
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

		//printf("%d %d\n", state.dpointer.x, state.dpointer.y);
		UIDraw(scr, root);
		UpdateWindow();
	}

	return 0;
}
