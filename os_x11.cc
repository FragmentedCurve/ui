#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

#include "ui_common.h"

static Display* display;
static Window main_window;
static XImage* image;
static const char* clipboard_text;

static auto mouse_grabbed = false;

void Console(const char *s) {
	printf("%s", s);
	fflush(stdout);
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
	// TODO: This is too slow. If we zoom rectangle is larger,
	// this function causes a lot of lag.
	XWindowAttributes attr;
	Pixel c;

	XImage i = {
		.width      = 1,
		.height     = 1,
		.format     = ZPixmap,
		.data       = (char*) &c,
		.bitmap_pad = 32,
		.depth      = 24,        
		.bytes_per_line = sizeof(Pixel),
		.bits_per_pixel = 32,
	};
	
	XInitImage(&i);
	XGetWindowAttributes(display, DefaultRootWindow(display), &attr);
	
	// Return black if out of bounds
	if (x < 0 || y < 0 || x >= attr.width || y >= attr.height)
		return Pixel(0);
		
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

static Event XEventToEvent(XEvent event) {
	switch (event.type) {
	case ClientMessage:
		// Assume the window was closed.
		return EVENT_QUIT;
	case ButtonPress:
		if (event.xbutton.button == 1)
			mouse_buttons[0] = true;
		else if (event.xbutton.button == 3)
			mouse_buttons[1] = true;
		else
			return EVENT_NULL;

		pointer.x = event.xbutton.x;
		pointer.y = event.xbutton.y;

		return EVENT_MOUSE_BUTTON;
	case ButtonRelease:
		if (event.xbutton.button == 1)
			mouse_buttons[0] = false;
		else if (event.xbutton.button == 3)
			mouse_buttons[1] = false;
		else
			return EVENT_NULL;

		pointer.x = event.xbutton.x;
		pointer.y = event.xbutton.y;

		return EVENT_MOUSE_BUTTON;
	case MotionNotify:
		pointer.x = event.xmotion.x;
		pointer.y = event.xmotion.y;
		return EVENT_MOUSE_MOVE;
	case KeyPress:
		return EVENT_KEY_PRESS;
	case SelectionRequest:
		FillSelectionRequest(event);
		return EVENT_NULL;
	case Expose:
		return EVENT_UPDATE_WINDOW;
	}

	return EVENT_NULL;
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
	return XEventToEvent(xevent);
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
		main_window = XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 5, BlackPixel(display, 0), WhitePixel(display, 0));
		XStoreName(display, main_window, WINDOW_TITLE);
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
		XSelectInput(display, main_window, ExposureMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | KeyPressMask | KeyReleaseMask);
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
