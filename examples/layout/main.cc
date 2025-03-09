#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <ui.h>
#include "dg.c"

const char* SCREEN_TITLE  = "Layout Test";
const int   SCREEN_WIDTH  = 480;
const int   SCREEN_HEIGHT = SCREEN_WIDTH;
UIPixel* screen;

int UIMain(int argc, char** argv) {
	UIWidget* root;

	UIToggle *t1, *t2, *t3, *t4;
	UIRawInput s;
	auto* scr = new UIScreen(screen, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH_MAX);

	srand(time(NULL));
	int key = rand() & 0xf;

#ifdef DEBUG
	key = 1;
#endif

	(root = new UIHBox(-1, UIRect()))
	  ->Children(
		     (t1 = new UIToggle(1, UIRect())),
		     (t2 = new UIToggle(2, UIRect())),
		     (t3 = new UIToggle(3, UIRect())),
		     (t4 = new UIToggle(4, UIRect())))->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	root->fill_screen = true;

	int pan = 0;
	int shift = 0;
	while (s = UINativeState(), !s.halt) {
		UIReaction out = UIImpacted(s, root);

		int guess = t1->light.on | (t2->light.on << 1) | (t3->light.on << 2) | (t4->light.on << 3);
		if (guess == key) {
			root->visible = false;
		}

		if (s.keys[65]) {
			root->visible = true;
			t1->Off();
			t2->Off();
			t3->Off();
			t4->Off();
		}

		if (!root->visible) {
			if (s.keys[116]) {
				pan += 100;
				if (pan > root->r.yw - 300)
					pan = root->r.yw - 300;
			} else if (s.keys[111]) {
				pan -= 100;
				if (pan < - __dumbgirlxc.height + 300)
					pan = - __dumbgirlxc.height + 300;
			}

			if (s.keys[114]) {
				shift += 100;
				if (shift >= root->r.xw)
					shift = root->r.xw;
			} else if (s.keys[113]) {
				shift -= 100;
				if (shift < 0)
					shift = 0;
			}

			for (auto i = 0; i < __dumbgirlxc.height + (pan < 0 ? pan : 0); i++) {
				for (auto j = 0; j < __dumbgirlxc.width; j++) {
					scr->pixels[(i + (pan >= 0 ? pan : 0)) * scr->pitch + (j + shift)] = UI_RGB(
						__dumbgirlxc.pixel_data[3 * ((i - (pan < 0 ? pan : 0)) * __dumbgirlxc.width + j)    ],
						__dumbgirlxc.pixel_data[3 * ((i - (pan < 0 ? pan : 0)) * __dumbgirlxc.width + j) + 1],
						__dumbgirlxc.pixel_data[3 * ((i - (pan < 0 ? pan : 0)) * __dumbgirlxc.width + j) + 2]);
				}
			}
		}

		scr->xw = s.screen_width;
		scr->yw = s.screen_height;
		UIDraw(scr, root);
		UINativeUpdate();
	}

	return 0;
}
