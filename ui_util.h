#ifndef _UI_UTIL_H_
#define _UI_UTIL_H_

#define INDEX(xw, x, y) ((xw) * (y) + (x))
#define NUMBER_OF(type, array) (sizeof(array)/sizeof(type))

#define HANDLED_FAILURE false
#define HANDLED_SUCCESS true

#ifndef NULL
#define NULL 0
#endif

#define UNUSED(var) (void) var;

#define CALLBACK(widget, func) \
	static void callback_ ## func (widget *w, Event e, void* data);	\
	void func (UIWidget* w, Event e, void* data) { return callback_ ## func ((widget *) w, e, data); } \
	static void callback_ ## func (widget *w, Event e, void* data)
	
#define HANDLER(widget, func) \
	static bool handler_ ## func (widget *w, Event e);		\
	bool func (UIWidget* w, Event e) { return handler_ ## func ((widget *) w, e); } \
	static bool handler_ ## func (widget *w, Event e)


#define UI_DEFINE_GLOBALS(title, win_width, win_height)	 \
	Point pointer;					 \
	bool mouse_buttons[2];				 \
	Pixel* screen;					 \
	int SCREEN_WIDTH   = (win_width);		 \
	int SCREEN_HEIGHT  = (win_height);		 \
	const char* WINDOW_TITLE = (title);


#ifdef _DEBUG
#include <cstdio>
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#define DEBUG_MOUSE                                                            \
  DEBUG("mouse_buttons = {%d, %d}; pointer = {%d, %d}\n", mouse_buttons[0],          \
        mouse_buttons[1], pointer.x, pointer.y)
#endif

#endif // _UI_UTIL_H_
