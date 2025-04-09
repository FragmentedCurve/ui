#include <ui.h>

const char* SCREEN_TITLE  = "Graphics Demo";
const int   SCREEN_WIDTH  = 480;
const int   SCREEN_HEIGHT = SCREEN_WIDTH;
UIPixel* screen;

int UIMain(int argc, char** argv) {
	UIRawInput s;
	UIScreen* scr = new UIScreen(screen, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH_MAX);

	while (s = UINativeState(), !s.halt) {
		// TODO: Do something
		UINativeUpdate();
	}

	return 0;
}
