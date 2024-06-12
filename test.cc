#include "os.h"
#include "ui.h"

Screen* scr;
UI_DEFINE_GLOBALS("Tester", 1000, 1000);

UIToggle *wToggle;
UIButton *wButton;
UIWidget* ui_root = new UIWidget(NULL, Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));

CALLBACK(UIToggle, cbToggle) {
	w->Toggle();
	wButton->visible = !(wButton->visible);
	scr->DrawFill(UI_SURFACE_BG, wButton->r);
}

CALLBACK(UIButton, cbButton) {
	wToggle->Resize(10, 10);
	wToggle->Draw(scr);
}

HANDLER(UIButton, moveButton) {
	static auto prev_pointer = pointer;
	static auto entry = false;

	if (!mouse_buttons[0] || !w->Pressed()) {
		entry = false;
		return HANDLED_FAILURE;
	}

	if (!entry)
		prev_pointer = pointer;

	auto dp = Point(pointer.x - prev_pointer.x,
			pointer.y - prev_pointer.y);
	scr->DrawFill(UI_SURFACE_BG, w->r);
	w->Push(dp);
	prev_pointer = pointer;
	entry = true;

	return HANDLED_SUCCESS;
}

#include <cstdio>

int AppMain(int argc, char** argv) {
	Event e;

	{ // GUI Tree
		auto panel = new UISurface(ui_root, ui_root->r);
		wToggle = new UIToggle(panel, Point(0, 0), 100, 50);
		wButton = new UIButton(panel, Point(400, 400), 500, 100);

		wToggle->callback = cbToggle;
		wButton->callback = cbButton;
		wButton->posthook[EVENT_MOUSE_MOVE] = moveButton;
		wButton->posthook[EVENT_MOUSE_BUTTON] = moveButton;
	}

	scr = new Screen(screen, SCREEN_WIDTH, SCREEN_HEIGHT);

	while ((e = GetEvent()) != EVENT_QUIT) {
		if (e == EVENT_NULL)
			continue;

		//DEBUG_MOUSE;
		printf("Event: %d\n", e);
		
		UIDelegate(e, ui_root);
		UIDraw(scr, ui_root);
		UpdateWindow();
	}

	return 0;
}
