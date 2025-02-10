#ifndef _UI_STYLE_H_
#define _UI_STYLE_H_

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

#endif // _UI_STYLE_H_
