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

#include <cstdio>

struct UIBitmap {
	void Draw(UIScreen* scr, UIPoint p) {
		// TODO: Come back to this and implement a fully
		// featured UIBitmap. It should include channels,
		// bitblitzing, masking, blending, and resizing.

		for (auto i = 0; i < xw; i++) {
			for (auto j = 0; j < yw; j++) {
				if ((data[UI_INDEX(xw, i, j)] & 0xffffff) != mask) {
					scr->pixels[UI_INDEX(scr->pitch, p.x + i, p.y + j)] = data[UI_INDEX(xw, i, j)];
				}
			}
		}
	}

	int xw        = 0;
	int yw        = 0;
	UIPixel mask  = UI_GREEN; // TODO: What's the best mask implementation?
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
