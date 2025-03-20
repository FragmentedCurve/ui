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

#pragma once

struct UIBitmap {
	void Draw(UIScreen* scr, UIPoint p) {
		for (auto i = 0; i < xw; i++) {
			for (auto j = 0; j < yw; j++) {
				switch (channels) {
				case 1: {
				} break;
				case 2: {
				} break;
				case 3: {
					scr->pixels[UI_INDEX(scr->pitch, p.x + i, p.y + j)] = data[UI_INDEX(xw, i, j)];
				} break;
				case 4: {
					// TODO: Alpha blending.
					scr->pixels[UI_INDEX(scr->pitch, p.x + i, p.y + j)] = data[UI_INDEX(xw, i, j)];
				} break;
				default:
					UIASSERT("Invalid channel quantity in UIBitmap");
				}

			}
		}
	}

	int xw        = 0;
	int yw        = 0;
	int channels  = 3;
	UIPixel mask  = UI_WHITE; // TODO: What's the best mask implementation?
	UIPixel* data = NULL;     // TODO: Should this me a unique pointer?
};


struct UIFont {
	UIFont(UIBitmap* font);

	void Draw(UIScreen* scr, const char* s);
	void Draw(UIScreen* scr, UIRect r, const char* s);

	int size;
	int space_word;
	int space_letter;
	int justify;
};
