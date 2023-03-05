#ifndef _COMMON_H_
#define _COMMON_H_

#include <cstdint>

typedef uint32_t Pixel;

#define BLACK 0x000000
#define WHITE 0xffffff
#define RED   0xff0000
#define GREEN 0x00ff00
#define BLUE  0x0000ff
#define GRAY  0xdedbde

#define INVERT_PIXEL(c) ((WHITE ^ (c)) | RED)

enum Event {
	NONE,        // Who knows?
	QUIT,        // Quit message from WM
	MOUSE_LEFT,  // Left click
	MOUSE_RIGHT, // Right click
	MOUSE_MOVE,  // You figure this one out
	KEY_PRESS    // Any key was pressed
};

struct Point {
	int x, y;

        Point() { y = x = 0; }

        Point(int x, int y) {
		this->x = x;
		this->y = y;
        }
};

#define WINDOW_TITLE "PixelGrab"

// Provided to the platform by PixelGrab
extern Point pointer;
extern Pixel* screen;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
int AppMain(int argc, char **argv);

#endif // _COMMON_H_
