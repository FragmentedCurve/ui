#include <ui.h>
#include <vector>
#include <iostream>
#include <filesystem>
#include <dirent.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#include "icon_next.c"
#include "icon_prev.c"

using namespace std;

const char* SCREEN_TITLE  = "Image Viewer";
const int   SCREEN_WIDTH  = 640;
const int   SCREEN_HEIGHT = 480;
UIPixel* screen;

// Widget IDs
enum {
	NEXT_IMAGE,
	PREV_IMAGE,
};

struct FileList {
	static FileList* load(filesystem::path path) {
		struct dirent* entry;

		DIR* dir = opendir(path.c_str());
		if (!dir) {
			return NULL;
		}

		auto result = new FileList();
		auto files = new vector<filesystem::path>();
		while (entry = readdir(dir), entry) {
			if (entry->d_type == DT_REG && entry->d_name[0] != '.') {
				files->push_back(path / filesystem::path(entry->d_name));
				delete entry;
			}
		}
		closedir(dir);
		result->files = files;
		return result;
	}

	int next() {
		direction = 1;
		return rotate();
	}

	int prev() {
		direction = -1;
		return rotate();
	}

	filesystem::path filename() {
		return files->at(index);
	}

	int rotate() {
		index += direction;
		if (index >= files->size() && direction > 0) {
			index = 0;
		} else if (index <= 0 && direction < 0) {
			index = files->size() - 1;
		}
		return index;
	}

private:
	int index = 0;
	int direction = 1;
	vector<filesystem::path>* files;
};

UIBitmap load_image(const char* filename) {
	UIBitmap img{0, 0, 0, 0};
	auto* data = stbi_load(filename, &img.xw, &img.yw, NULL, 4);
	if (!data) {
		goto error;
	}
	img.data = (UIPixel*) data;
error:
	return img;
}

int UIMain(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
		return -1;
	}

	auto rootpath = filesystem::path(argv[1]);

	if (rootpath.has_filename()) {
		rootpath = rootpath.remove_filename();
	}
	auto files = FileList::load(rootpath.c_str());

	UIRawInput s;
	UIScreen* scr = new UIScreen(screen, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH_MAX);
	UIWidget* root = new UIPanel(69 , UIRect(0, 0, SCREEN_WIDTH, 50));
	root->Children(
		new UIButton(PREV_IMAGE, UIRect(10, 5, 50, 40)),
		new UIButton(NEXT_IMAGE, UIRect(70, 5, 50, 40)));

	((UIButton*)root->childtail)->icon = UIBitmap{
		(int) icon_next.width,
		(int) icon_next.height,
		UI_GREEN,
		(UIPixel*) icon_prev.pixel_data
	};

	((UIButton*)root->childhead)->icon = UIBitmap{
		(int) icon_prev.width,
		(int) icon_prev.height,
		UI_GREEN,
		(UIPixel*) icon_next.pixel_data
	};

	UIBitmap image;
	while (s = UINativeState(), !s.halt) {
		UIReaction out = UIImpacted(s, root);
		if (out.clicked) {
			if(out.clicked->id == NEXT_IMAGE) {
				files->next();
			} else if (out.clicked->id == PREV_IMAGE) {
				files->prev();
			} else {
				continue;
			}

			memset(scr->pixels, 0, SCREEN_WIDTH_MAX * SCREEN_HEIGHT_MAX * sizeof(UIPixel));
			while (image = load_image(files->filename().c_str()), !image.data) {
				// TODO: Stop looping if no images found.
				files->rotate();
			}
			image.Draw(scr, UIPoint(0, 60));
		}

		UIDraw(scr, root);
		UINativeUpdate();
	}

	return 0;
}
