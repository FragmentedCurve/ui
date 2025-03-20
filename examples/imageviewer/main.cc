#include <ui.h>
#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#include "icon_next.c"
#include "icon_prev.c"

const char* SCREEN_TITLE  = "Image Viewer";
const int   SCREEN_WIDTH  = 640;
const int   SCREEN_HEIGHT = 480;
UIPixel* screen;

// Widget IDs
enum {
	NEXT_IMAGE,
	PREV_IMAGE,
};

UIBitmap* load_image(const char* filename) {
	auto img = new UIBitmap();
	auto* data = stbi_load(filename, &img->xw, &img->yw, NULL, 4);

	if (!data) {
		return NULL;
	}

	img->data = (UIPixel*) data;

	return img;
}

int UIMain(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
		return -1;
	}

	auto image = load_image(argv[1]);
	if (!image) {
		fprintf(stderr, "Failed to load: %s\n", argv[1]);
		return -1;
	}

	UIRawInput s;
	UIScreen* scr = new UIScreen(screen, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH_MAX);
	UIWidget* root = new UIPanel(69 , UIRect(0, 0, SCREEN_WIDTH, 50));
	root->Children(
		new UIButton(PREV_IMAGE, UIRect(10, 5, 50, 40)),
		new UIButton(NEXT_IMAGE, UIRect(70, 5, 50, 40)));

	((UIButton*)root->childtail)->icon = UIBitmap{
		(int) icon_next.width,
		(int) icon_next.height,
		4,
		UI_WHITE,
		(UIPixel*) icon_next.pixel_data
	};

	((UIButton*)root->childhead)->icon = UIBitmap{
		(int) icon_prev.width,
		(int) icon_prev.height,
		4,
		UI_WHITE,
		(UIPixel*) icon_prev.pixel_data
	};

	while (s = UINativeState(), !s.halt) {
		UIReaction out = UIImpacted(s, root);

		if (out.clicked) {
			if(out.clicked->id == NEXT_IMAGE) {
				printf("next\n");
			} else if (out.clicked->id == PREV_IMAGE) {
				printf("prev\n");
			}
		}

		image->Draw(scr, UIPoint(0, 60));
		UIDraw(scr, root);
		UINativeUpdate();
	}

	return 0;
}
