#include <ui.h>

const char* SCREEN_TITLE  = "Layout Test";
const int   SCREEN_WIDTH  = 480;
const int   SCREEN_HEIGHT = SCREEN_WIDTH;
UIPixel* screen;

int UIMain(int argc, char** argv) {
	UIWidget* root;
	UIRawInput s;
	auto* scr = new UIScreen(screen, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH_MAX);

	(root = new UIHBox(-1, UIRect()))
		->Children(
			(new UIVBox(-1, UIRect()))
			->Children(
				new UIButton(-1, UIRect()),
				new UIToggle(-1, UIRect())),
			new UIButton(-1, UIRect()),
			new UIButton(-1, UIRect()))->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	root->resize = true;

	while (s = UINativeState(), !s.halt) {
		UIReaction out = UIImpacted(s, root);
		scr->xw = s.screen_width;
		scr->yw = s.screen_height;
		UIDraw(scr, root);
		UINativeUpdate();
	}

	return 0;
}
