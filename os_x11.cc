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

UIRawInput UINativeState() {
	XEvent event;
	int x, y;
	static UIRawInput s;

	Window root_return;
	Window child_return;
	int root_x, root_y;
	unsigned int mask;

	XWindowAttributes attr;

	(void) XGetWindowAttributes(display, main_window, &attr);

	s.screen_width = attr.width;
	s.screen_height = attr.height;

	if (XQueryPointer(display, main_window, &root_return, &child_return, &root_x, &root_y, &x, &y, &mask)) {
		s.dpointer = UIPoint(x - s.pointer.x, y - s.pointer.y);
		s.pointer.x = x;
		s.pointer.y = y;
	} else {
		s.dpointer = UIPoint(0, 0);
	}

	XNextEvent(display, &event);
	switch (event.type) {
	case ClientMessage: {
		// Assume the window was closed.
		s.halt = true;
	} break;
	case ButtonPress: {
		if (event.xbutton.button == 1)
			s.m[0] = true;
		else if (event.xbutton.button == 3)
			s.m[1] = true;
	} break;
	case ButtonRelease: {
		if (event.xbutton.button == 1)
			s.m[0] = false;
		else if (event.xbutton.button == 3)
			s.m[1] = false;
	} break;
	case KeyPress: {
		printf("KEYCODE PRESS: %d\n", event.xkey.keycode);
		s.keys[event.xkey.keycode] = true;
	} break;
	case KeyRelease:
		printf("KEYCODE RELEASE: %d\n", event.xkey.keycode);
		s.keys[event.xkey.keycode] = false;
		break;
	case SelectionRequest:
		FillSelectionRequest(event);
		break;
	}

	return s;
}

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
		XSelectInput(display, main_window,
			ResizeRedirectMask
			| PointerMotionMask
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
				NULL, &shmseg, SCREEN_WIDTH, SCREEN_HEIGHT);
			shmseg.shmid = shmget(
				IPC_PRIVATE,
				SCREEN_WIDTH * SCREEN_HEIGHT * DefaultDepth(display, DefaultScreen(display)),
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
