#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

#include "common.h"

static Display* display;
static Window main_window;
static XImage* image;
static const char* clipboard_text;

void Console(const char* s) {
	printf("%s", s);
}

void ToClipboard(const char* s) {
	clipboard_text = s;
	XSetSelectionOwner(display, XA_PRIMARY, main_window, CurrentTime);
}

void GrabMouse() {
	XGrabPointer(
		display, DefaultRootWindow(display), True,
		PointerMotionMask | ButtonPressMask,
		GrabModeAsync, GrabModeAsync,
		None, None, CurrentTime);
}

void ReleaseMouse() {
	XUngrabPointer(display, CurrentTime);
}

Pixel GetPixel(int x, int y) {
	XWindowAttributes attr;
	XGetWindowAttributes(display, DefaultRootWindow(display), &attr);

	// Return black if out of bounds
	if (x < 0 || y < 0 || x >= attr.width || y >= attr.height)
		return BLACK;
	
	XImage *i = XGetImage(display, DefaultRootWindow(display), x, y, 1, 1,
			AllPlanes, ZPixmap);
	Pixel pixel = XGetPixel(i, 0, 0);
	XDestroyImage(i);
	return pixel;
}

void UpdateWindow() {
	XShmPutImage(display, main_window,
		DefaultGC(display, DefaultScreen(display)),
		image, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
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
		.property = event.xselectionrequest.property ? event.xselectionrequest.property	: XA_PRIMARY,
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

static Event XEventToEvent(XEvent event) {
        switch (event.type) {
        case ClientMessage:
		// Assume the window was closed.
		return QUIT;
        case ButtonPress:
		if (event.xbutton.button == 1)
			return MOUSE_LEFT;
		else if (event.xbutton.button == 3)
			return MOUSE_RIGHT;
		else
			return NONE;
        case MotionNotify:
		return MOUSE_MOVE;
        case KeyPress:
		return KEY_PRESS;
        case SelectionRequest:
		FillSelectionRequest(event);
		return NONE;
        }

        return NONE;
}

Event PeekEvent() {
	XEvent event;

	XNextEvent(display, &event);
	XPutBackEvent(display, &event);

	return XEventToEvent(event);
}

Event GetEvent() {
	XEvent xevent;
	XNextEvent(display, &xevent);
	auto event = XEventToEvent(xevent);

	// TODO: Optimize this so NONE events aren't returned.

        switch (event) {
        case MOUSE_LEFT:
        case MOUSE_RIGHT: {
                pointer.x = xevent.xbutton.x;
                pointer.y = xevent.xbutton.y;
        } break;
        case MOUSE_MOVE: {
                pointer.x = xevent.xmotion.x;
                pointer.y = xevent.xmotion.y;
        } break;
        }

	return event;
}

int main(int argc, char** argv) {
	int status;
	XShmSegmentInfo shmseg;

	display = XOpenDisplay(NULL);
        if (!display) {
		fprintf(stderr, "Cannot open displayplay.\n");
		exit(1);
        }

        { // Create windows
                main_window = XCreateSimpleWindow(
			display, RootWindow(display, 0), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 5,
			BlackPixel(display, 0), WhitePixel(display, 0));
                XStoreName(display, main_window, WINDOW_TITLE);
                XResizeWindow(display, main_window, SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        { // Initialize event listening
                Atom del_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
                XSetWMProtocols(display, main_window, &del_window, 1);
                XSelectInput(display, main_window, ExposureMask | ButtonPressMask | KeyPressMask);
        }

        { // Setup Shm for quick rendering
                if (XShmQueryExtension(display)) {
			image = XShmCreateImage(
				display, DefaultVisual(display, DefaultScreen(display)),
				DefaultDepth(display, DefaultScreen(display)), ZPixmap,
				NULL, &shmseg, SCREEN_WIDTH, SCREEN_HEIGHT);
			shmseg.shmid = shmget(
				IPC_PRIVATE,
				SCREEN_WIDTH * SCREEN_HEIGHT * DefaultDepth(display, DefaultScreen(display)),
				IPC_CREAT | 0777);
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
        status = AppMain(argc, argv);

	// Cleanup
	XShmDetach(display, &shmseg);
	XDestroyWindow(display, main_window);
	XCloseDisplay(display);

	return status;
}
