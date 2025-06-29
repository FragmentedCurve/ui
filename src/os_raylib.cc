/*
 * Copyright (c) 2025 Paco Pascal <me@pacopascal.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <cstdio>
#include <raylib.h>

static UIRawInput raw_input;
static Texture screen_texture;

void UINativeConsole(const char *s) {
	printf("%s", s);
	fflush(stdout);
}

void UINativeToClipboard(const char* s) {
	SetClipboardText(s);
}

void UINativeUpdate() {
	auto r = Rectangle{0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	auto v = Vector2{0, 0};

	BeginDrawing();
	DrawTexture(screen_texture, 0, 0, WHITE);
	EndDrawing();
}

UIRawInput UINativeState() {
	// TODO: Implement
	raw_input.halt = WindowShouldClose();
	return raw_input;
}

// TODO: Automate detection and setting of max dimensions.
int SCREEN_WIDTH_MAX  = 4500;
int SCREEN_HEIGHT_MAX = 3000;

int main(int argc, char** argv) {
	// Allocate screen's framebuffer.
	if (screen = new UIPixel[SCREEN_WIDTH_MAX * SCREEN_HEIGHT_MAX], !screen) {
		UINativeConsole("Failed to allocate framebuffer.\n");
		return -1;
	}

	// Open Window
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE);

	{ // Prepare texture for drawing screen.
		Image image;

		image.width   = SCREEN_WIDTH_MAX;
		image.height  = SCREEN_HEIGHT_MAX;
		image.mipmaps = 1;
		image.format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
		image.data    = screen;

		screen_texture = LoadTextureFromImage(image);
	}

	SetTargetFPS(60);

	// Run Pixel Grab
	auto status = UIMain(argc, argv);

	// Close window and exit process
	UnloadTexture(screen_texture);
	CloseWindow();
	return status;
}
