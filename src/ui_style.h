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

#define __RGB(r, g, b) ((UIPixel)(((r) & 0xff) << 16 | ((g) & 0xff) << 8 | ((b) & 0xff)))
#define __BGR(r, g, b) ((UIPixel)(((b) & 0xff) << 16 | ((g) & 0xff) << 8 | ((r) & 0xff)))

#define UI_RGB(r, g, b) __RGB(r, g, b)

enum UIColors {
	UI_DARKEST        = UI_RGB(   0,    0,    0),
	UI_LIGHTEST       = UI_RGB(0xff, 0xff, 0xff),
	UI_SURFACE_BG     = UI_RGB(0xde, 0xdb, 0xde), // TODO: Rename this, "BG" isn't properly descriptive.
	UI_SURFACE_FG     = UI_RGB(0xe8, 0xe8, 0xe8), // TODO: Rename for the same reasons above.
	UI_SURFACE_HILITE = UI_RGB(0xf0, 0xf0, 0xf0),
	UI_SURFACE_SHADOW = UI_RGB(0xa0, 0xa0, 0xa0),
	UI_SURFACE_BORDER = UI_RGB(   0,    0,    0),
	UI_TEXT_BG        = UI_RGB(0xff, 0xff, 0xff),
	UI_TEXT_FG        = UI_RGB(   0,    0,    0),
	UI_TEXT_BORDER    = UI_RGB(0x2e, 0x2e, 0x2e),
	UI_LIGHT_BORDER   = UI_RGB(0x2e, 0x2e, 0x2e),
	UI_LIGHT_ON       = UI_RGB(0xff, 0xd1, 0x66), // TODO: Potential alternative default (green): 0x00cd00
	UI_LIGHT_OFF      = UI_RGB(0xde, 0xdb, 0xde),
};

#define UI_WHITE UI_RGB(255, 255, 255)
#define UI_BLACK UI_RGB(  0,   0,   0)
#define UI_RED   UI_RGB(255,   0,   0)
#define UI_GREEN UI_RGB(  0, 255,   0)
#define UI_BLUE  UI_RGB(  0,   0, 255)

extern bool gylphs[][15];
