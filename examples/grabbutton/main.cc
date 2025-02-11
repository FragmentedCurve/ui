#include <ui.h>

const char* WINDOW_TITLE = "grabbutton";
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

UIPixel* screen;

#include <cstdio>

enum { TOGGLE, BUTTON };

int UIMain(int argc, char** argv) {
	// GUI Tree
	UIWidget* root = new UISurface(-1, UIRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
	UIButton* b = new UIButton(BUTTON, UIPoint(400, 400), 500, 100);
	UIToggle* t = new UIToggle(TOGGLE, UIPoint(10, 10), 100, 50);

	root->Children(t, b);

	UIScreen* scr = new UIScreen(screen, SCREEN_WIDTH, SCREEN_HEIGHT);

	for (UIRawInput state = UINativeState(); !state.halt; state = UINativeState()) {
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
