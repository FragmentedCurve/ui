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
#include <cstdlib>
#include <cstring>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

#include "ui_core.h"

static Display* display;
static Window main_window;
static XImage* image;
static const char* clipboard_text;

static UIRawInput raw_input;

void UINativeConsole(const char *s) {
	printf("%s", s);
	fflush(stdout);
}

void UINativeToClipboard(const char* s) {
	clipboard_text = s;
	XSetSelectionOwner(display, XA_PRIMARY, main_window, CurrentTime);
}

void UINativeGrabMouse() {
	XGrabPointer(
		display, DefaultRootWindow(display), True,
		PointerMotionMask | ButtonPressMask,
		GrabModeAsync, GrabModeAsync,
		None, None, CurrentTime);
}

void UINativeReleaseMouse() {
	XUngrabPointer(display, CurrentTime);
}

UIPixel UINativeGetPixel(int x, int y) {
	// TODO: This is too slow. If we zoom rectangle is larger,
	// this function causes a lot of lag.
	XWindowAttributes attr;
	UIPixel c;

	XImage i = {
		.width      = 1,
		.height     = 1,
		.format     = ZPixmap,
		.data       = (char*) &c,
		.bitmap_pad = 32,
		.depth      = 24,
		.bytes_per_line = sizeof(UIPixel),
		.bits_per_pixel = 32,
	};

	XInitImage(&i);
	XGetWindowAttributes(display, DefaultRootWindow(display), &attr);

	// Return black if out of bounds
	if (x < 0 || y < 0 || x >= attr.width || y >= attr.height)
		return UIPixel(0);

	(void) XGetSubImage(
			    display,
			    DefaultRootWindow(display),
			    x, y,
			    1, 1,
			    AllPlanes, ZPixmap,
			    &i,
			    0, 0);

	return c;
}

void UINativeUpdate() {
	XShmPutImage(display, main_window,
		DefaultGC(display, DefaultScreen(display)),
		image, 0, 0, 0, 0,
		raw_input.screen_width, raw_input.screen_height, 0);
}

int PendingEvents() {
	return XPending(display);
}

static void FillSelectionRequest(XEvent event) {
	Atom targets[] = {
		XInternAtom(display, "TARGETS", false),
		XA_STRING,
		XInternAtom(display, "UTF8_STRING", false)
	};

	XSelectionEvent e = {
		.type = SelectionNotify,
		.display = display,
		.requestor = event.xselectionrequest.requestor,
		.selection = event.xselectionrequest.selection,
		.target = event.xselectionrequest.target,
		.property = event.xselectionrequest.property ? event.xselectionrequest.property : XA_PRIMARY,
		.time = event.xselectionrequest.time
	};

	if (e.target == targets[0]) {
		XChangeProperty(display, e.requestor, e.property,
				e.target, 8, PropModeReplace,
				(const unsigned char *)targets,
				sizeof(targets));
	} else if (e.target == targets[1] || e.target == targets[2]) {
		XChangeProperty(display, e.requestor, e.property,
				e.target, 8, PropModeReplace,
				(const unsigned char *)clipboard_text,
				strlen(clipboard_text));
	}

	XSendEvent(display, e.requestor, false, 0,(XEvent *)&e);
}

UIRawInput UINativeState() {
	XEvent event;

	{ // Grab window dimensions
		XWindowAttributes attr;
		(void) XGetWindowAttributes(display, main_window, &attr);
		raw_input.screen_width = attr.width;
		raw_input.screen_height = attr.height;
	}

	{ // Grab cursor position
		int x, y;
		unsigned int mask;
		int root_x, root_y;
		Window root_return;
		Window child_return;

		if (XQueryPointer(display, main_window, &root_return, &child_return, &root_x, &root_y, &x, &y, &mask)) {
			raw_input.dpointer = UIPoint(x - raw_input.pointer.x, y - raw_input.pointer.y);
			raw_input.pointer.x = x;
			raw_input.pointer.y = y;
		} else {
			raw_input.dpointer = UIPoint(0, 0);
		}
	}

	XNextEvent(display, &event);
	switch (event.type) {
	case ClientMessage: {
		// Assume the window was closed.
		raw_input.halt = true;
	} break;
	case ButtonPress: {
		if (event.xbutton.button == 1)
			raw_input.m[0] = true;
		else if (event.xbutton.button == 3)
			raw_input.m[1] = true;
	} break;
	case ButtonRelease: {
		if (event.xbutton.button == 1)
			raw_input.m[0] = false;
		else if (event.xbutton.button == 3)
			raw_input.m[1] = false;
	} break;
	case KeyPress: {
		raw_input.keys[event.xkey.keycode] = true;
	} break;
	case KeyRelease:
		raw_input.keys[event.xkey.keycode] = false;
		break;
	case SelectionRequest:
		FillSelectionRequest(event);
		break;
	}

	return raw_input;
}

// TODO: Automate detection and setting of max dimensions.
int SCREEN_WIDTH_MAX  = 4500;
int SCREEN_HEIGHT_MAX = 3000;

int main(int argc, char** argv) {
	int status;
	XShmSegmentInfo shmseg; // TODO: Clean this up at exit (not when returning from main).

	display = XOpenDisplay(NULL);
	if (!display) {
		fprintf(stderr, "Cannot open displayplay.\n");
		exit(1);
	}

	{ // Create windows
		main_window = XCreateSimpleWindow(
			display,
			RootWindow(display, 0),
			0, 0,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			0 /* border */,
			BlackPixel(display, 0),
			WhitePixel(display, 0));
		XStoreName(display, main_window, SCREEN_TITLE);
		XResizeWindow(display, main_window, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

	/*
	  FIXME: kwin_x11 (KDE's window manager) crashes if we try to
	         set the window size.
	*/

	/*
	{ // Set window size attributes
	XSizeHints sizeHints;

		sizeHints.flags |= PMinSize | PMaxSize;
		sizeHints.min_width = sizeHints.max_width = SCREEN_WIDTH;
		sizeHints.min_height = sizeHints.max_height = SCREEN_HEIGHT;

		XSetWMNormalHints(display, main_window, &sizeHints);
	}
	*/

	{ // Initialize event listening
		Atom del_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
		XSetWMProtocols(display, main_window, &del_window, 1);
		XSelectInput(display, main_window,
			PointerMotionMask
			| ExposureMask
			| ButtonPressMask
			| ButtonReleaseMask
			| ButtonMotionMask
			| KeyPressMask
			| KeyReleaseMask);
	}

	{ // Setup Shm for quick rendering
		if (XShmQueryExtension(display)) {
			image = XShmCreateImage(
				display, DefaultVisual(display, DefaultScreen(display)),
				DefaultDepth(display, DefaultScreen(display)), ZPixmap,
				NULL, &shmseg, SCREEN_WIDTH_MAX, SCREEN_HEIGHT_MAX);

			shmseg.shmid = shmget(
				IPC_PRIVATE,
				SCREEN_WIDTH_MAX * SCREEN_HEIGHT_MAX * DefaultDepth(display, DefaultScreen(display)),
				IPC_CREAT | 0777);

			if (shmseg.shmid <= -1) {
				perror("shmget() failed");
				exit(1);
			}

			shmseg.shmaddr = image->data = (char *)shmat(shmseg.shmid, 0, 0);
			shmseg.readOnly = false;
			XShmAttach(display, &shmseg);
			screen = (uint32_t*) shmseg.shmaddr;
		} else {
			// TODO: Use slow XPutImage.
			fprintf(stderr, "XShm not available.\n");
			exit(1);
		}
	}

	// Display window
	XMapWindow(display, main_window);

	// Run Pixel Grab
	status = UIMain(argc, argv);

	// Cleanup
	XShmDetach(display, &shmseg);
	XFree(image);
	XDestroyWindow(display, main_window);
	XCloseDisplay(display);

	if (shmctl(shmseg.shmid, IPC_RMID, NULL) == -1) {
		perror("shmctl");
	}

	return status;
}
