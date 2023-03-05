#ifndef _OS_H_
#define _OS_H_

#include "common.h"

// Platform Dependent, implemented in os_*.cc

Pixel GetPixel(int x, int y);
void UpdateWindow();
void ToClipboard(const char* s);
Event GetEvent();
Event PeekEvent();
int PendingEvents();
void GrabMouse();
void ReleaseMouse();
void Console(const char* s);

#endif // _OS_H_
