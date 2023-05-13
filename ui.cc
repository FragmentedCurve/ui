#include "ui.h"
#include "os.h"

#include "font.h"

#include "common.h" // TODO: REMOVE ME, HERE FOR DEBUGGING

#include <cstdarg>
#include <cassert>

/*
  TODO

  - Smooth out how a widget gets drawn. Depending on the this->paint
    bool feels sketchy but it might be good enough.

  - Make gaining ownership of an event more robust and safe.

  - Provide more consistency across widget classes. For example, if a
    widget has a toggle state, provide the same interface for toggling
    across all toggle-capable widgets.

  - Write helper functions and macros for common tasks performed by
    widgets.

  - Bitmap drawing and manipulation.

  - Builtin fonts.
 */

UIWidget *event_owner[EVENT_LAST] = {0};

void SetOwner(UIWidget *w, Event e) {
	assert(event_owner[e] == NULL || event_owner[e] == w);
	event_owner[e] = w;
}

void SetOwner(UIWidget* w, ...) {
	// Event gets promated to int when passed as a variadic
	// argument.
	int e;
	va_list ap;

	va_start(ap, w);
        while ((e = va_arg(ap, int)) != EVENT_NULL) {
		assert(event_owner[e] == NULL || event_owner[e] == w);
		event_owner[e] = w;
        }
	va_end(ap);
}


void ReleaseOwner(UIWidget* w) {
	for (int i = 0; i < EVENT_LAST; i++)
		if (event_owner[i] == w)
			event_owner[i] = NULL;
}

void ReleaseOwner(UIWidget *w, Event e) {
        assert(event_owner[e] == NULL || event_owner[e] == w);
        event_owner[e] = NULL;
}

void ReleaseOwner(UIWidget* w, int e, ...) {
	// Event gets promated to int when passed as a variadic
	// argument.
	va_list ap;

	assert(event_owner[e] == NULL || event_owner[e] == w);
	event_owner[e] = NULL;

	va_start(ap, e);
        while ((e = va_arg(ap, int)) != EVENT_NULL) {
		assert(event_owner[e] == NULL || event_owner[e] == w);
		event_owner[e] = NULL;
        }
	va_end(ap);
}

#define SET_OWNER(...) SetOwner(this, __VA_ARGS__, EVENT_NULL)
#define RELEASE_OWNER(...) ReleaseOwner(this, __VA_ARGS__, EVENT_NULL)

/*
bool UIWidget::CaptureMouse(Event e) {
        if (e != EVENT_MOUSE_BUTTON)
                return false;

	auto any_pressed = false;
	for (int i = 0; i < NUMBER_OF(bool, mouse_buttons); i++)
		any_pressed = any_pressed || mouse_buttons[i];

        if (!any_pressed) {
                RELEASE_OWNER(EVENT_MOUSE_MOVE, EVENT_MOUSE_BUTTON);
                return true;
        }

	if (Hit(pointer))
		SET_OWNER(EVENT_MOUSE_MOVE, EVENT_MOUSE_BUTTON);

        return false;
}
*/

void UIDelegate(Event e, UIWidget* w[], int n) {
	// Catch programming mistakes.
	assert(e != EVENT_NULL);
	assert(e != EVENT_LAST);
	
	// If the event has an owner, ignore normal processing and
	// delegate directly to the owner.
	if (event_owner[e]) {
		event_owner[e]->Handle(e);
		return;
	}

        for (int i = 0; i < n; i++) {		
		bool result = w[i]->Handle(e);
		// If event style is chaining, continue through the
		// loop.
		if (result == HANDLED_SUCCESS)
			return;
	}
}

void UIDraw(Screen *scr, UIWidget *w[], int n) {
        for (int i = 0; i < n; i++) {
		w[i]->Draw(scr);
        }
}

void UILight::Draw(Screen *scr) {
	scr->DrawFill(on ? UI_LIGHT_ON : UI_LIGHT_OFF, pos.From(2, 2), pos.From(xw - 2, yw - 2));

        if (!on) {
		// Draw a thick shadow when the light is off.
                scr->DrawFill((on ? UI_LIGHT_ON : UI_LIGHT_OFF) << 1, pos.From(0, 0), pos.From(xw, 4)); // Left
                scr->DrawFill((on ? UI_LIGHT_ON : UI_LIGHT_OFF) << 1, pos.From(0, 0), pos.From(4, yw)); // Top
        }

        // Top hilite
	scr->DrawHLine(UI_SURFACE_SHADOW, pos.From(0, 0), xw - 1);      // Outer
	scr->DrawHLine(UI_SURFACE_SHADOW, pos.From(0, 1), xw - 2);      // Inner

	// Left hilite
	scr->DrawVLine(UI_SURFACE_SHADOW, pos.From(0, 0), yw - 1);      // Outer
	scr->DrawVLine(UI_SURFACE_SHADOW, pos.From(1, 0), yw - 2);      // Inner

	// Bottom shadow
	scr->DrawHLine(UI_SURFACE_HILITE, pos.From(1, yw - 1), xw - 1); // Outer
	scr->DrawHLine(UI_SURFACE_HILITE, pos.From(2, yw - 2), xw - 2); // Inner

	// Right shadow
	scr->DrawVLine(UI_SURFACE_HILITE, pos.From(xw - 1, 1), yw - 1); // Outer
	scr->DrawVLine(UI_SURFACE_HILITE, pos.From(xw - 2, 2), yw - 2); // Inner
}

void UIToggle::Draw(Screen *scr) {
        UIButton::Draw(scr);
        light.Draw(scr);
}

bool UIToggle::Handle(Event e) {
	return UIButton::Handle(e);
}

void HexFloat::Set(Pixel c) {
	const char* hexmap = "0123456789abcdef";
	char* buf = color_s;
	
	// Remove bytes that aren't RGB
	c &= 0x00ffffff;

	// Initialize string to "#000000"
	((int*)buf)[0] = *((int*)"#000");
	((int*)buf)[1] = *((int*)"000");
	
	buf = buf + 6;

        while (c) {
                *buf = hexmap[c % 0x10];
                c /= 0x10;
		buf--;
        }

	paint = true;
}

// TODO: Clean this up. It was quickly hacked out just to make it work.
void HexFloat::DrawFont(Screen* scr, bool *gylph, int len, int x0, int y0) {
	auto zoom = 2;
        for (int i = 0; i < len; i++) {
		auto x = x0 + zoom * (i % 3);
		auto y = y0 + zoom * (i / 3);

                if (gylph[i])
			scr->DrawFill(UI_TEXT_FG, Point(x, y), Point(x + zoom, y + zoom));
        }
}

void HexFloat::Draw(Screen* scr) {
	if (!paint)
		return;
	
	// TODO: Clean this up. It was quickly hacked out just to make it work.
	scr->DrawFill(UI_TEXT_BG, pos, pos.From(xw, yw));
	scr->DrawRect(UI_TEXT_BORDER, pos, pos.From(xw, yw));
	
	for (int i = 1; i < (int)sizeof(color_s) - 1; i++) {
		int c = color_s[i] - '0';
		if (color_s[i] >= 'a')
			c = color_s[i] - 'a' + 0xa;
		DrawFont(scr, gylphs[c], sizeof(gylphs[c]), pos.x + (5 + 8 * i), pos.y + 8);
	}

	paint = false;
}

void UIButton::Draw(Screen *scr) {
	if (pressed)
		Draw(scr, UI_SURFACE_SHADOW, UI_SURFACE_HILITE, UI_SURFACE_FG);
	else
		Draw(scr, UI_SURFACE_HILITE, UI_SURFACE_SHADOW, UI_SURFACE_BG);
}

void UIButton::Draw(Screen *scr, Pixel hilite, Pixel shadow, Pixel bg) {
	scr->DrawFill(bg, pos, pos.From(xw, yw));

	// Top hilite
	scr->DrawHLine(hilite, pos.From(0, 1), xw - 2);      // Outer
	scr->DrawHLine(hilite, pos.From(0, 2), xw - 3);      // Inner

	// Left hilite
	scr->DrawVLine(hilite, pos.From(1, 0), yw - 2);      // Outer
	scr->DrawVLine(hilite, pos.From(2, 0), yw - 3);      // Inner

	// Bottom shadow
	scr->DrawHLine(shadow, pos.From(2, yw - 2), xw - 2); // Outer
	scr->DrawHLine(shadow, pos.From(3, yw - 3), xw - 3); // Inner

	// Right shadow
	scr->DrawVLine(shadow, pos.From(xw - 2, 2), yw - 2); // Outer
	scr->DrawVLine(shadow, pos.From(xw - 3, 3), yw - 3); // Inner

	// Border
	scr->DrawRect(BLACK, pos, pos.From(xw, yw));
}

bool UIButton::Handle(Event e) {
        if (e != EVENT_MOUSE_BUTTON && e != EVENT_MOUSE_MOVE)
                return HANDLED_FAILURE;

        if (Hit(pointer)) {
                if (!pressed && mouse_buttons[0]) {
			pressed = true;
			SET_OWNER(EVENT_MOUSE_MOVE, EVENT_MOUSE_BUTTON);
                } else if (pressed && !mouse_buttons[0]) {
			ReleaseOwner(this);
                        pressed = false;
                        if (callback)
				callback(this, e, NULL);
                }
		return HANDLED_SUCCESS;
        } else {
                if (pressed && mouse_buttons[0]) {
			return HANDLED_SUCCESS;
                } else if (pressed && !mouse_buttons[0]) {
			ReleaseOwner(this);
			pressed = false;
			return HANDLED_SUCCESS;
                }
        }

        return HANDLED_FAILURE;
}

UIPixelGrid::UIPixelGrid(Point position, int xw, int yw, int zoom) : UIWidget(position, xw, yw) {
	cols = xw / zoom;
	rows = yw / zoom;
	this->zoom = zoom;
	pixels = new Pixel[cols * rows];
        for (int i = 0; i < cols * rows; i++)
		pixels[i] = WHITE;
}

UIPixelGrid::~UIPixelGrid() {
	delete[] pixels;
}

Point UIPixelGrid::CellPosition(int x, int y) {
	auto p0 = pos.From(x * zoom, y * zoom);	
	return p0;
}

void UIPixelGrid::Draw(Screen *scr) {
	if (!paint)
		return;
	
        for (int i = 0; i < cols; i++)
                for (int j = 0; j < rows; j++)
                        DrawCell(scr, i, j);

	paint = false;
}

void UIPixelGrid::DrawCell(Screen* scr, int x, int y) {
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= cols) x = cols - 1;
	if (y >= rows) y = rows - 1;

	//auto p0 = pos.From(x * zoom, y * zoom);
	auto p0 = CellPosition(x, y);
	auto p1 = p0.From(zoom, zoom);
	
        // Fill square
	scr->DrawFill(pixels[INDEX(cols, x, y)], p0, p1);
	
	// Draw border
	scr->DrawHLine(BLACK, p0, zoom);
	scr->DrawVLine(BLACK, p0, zoom);

	if (x == cols - 1)
		scr->DrawVLine(BLACK, p0.From(zoom - 1, 0), zoom);
	if (y == rows - 1)
		scr->DrawHLine(BLACK, p0.From(0, zoom - 1), zoom);
}

void UIPixelSelector::DrawSelection(Screen *scr) {
	// Weird inverted color
	auto c = (0xffffff ^ pixels[INDEX(cols, select.x, select.y)]) | 0xff0000;
       
	auto p0 = CellPosition(select.x, select.y);
	auto p1 = p0.From(zoom + 1, zoom + 1);

	auto q0 = p0.From(-1, -1);
	auto q1 = p1.From(1, 1);

	// Boundary corrections
        if (select.x == cols - 1)
		p1.x = q1.x -= 2;

        if (select.y == rows - 1)
		p1.y = q1.y -= 2;

	if (select.x == 0)
		q0.x++;

	if (select.y == 0)
		q0.y++;
	
        scr->DrawRect(c, p0, p1);
	scr->DrawRect(c, q0, q1);
}

void UIPixelSelector::EraseSelection(Screen* scr) {
	DrawCell(scr, prev_select.x, prev_select.y);
	DrawCell(scr, prev_select.x - 1, prev_select.y - 1);
	DrawCell(scr, prev_select.x, prev_select.y - 1);
	DrawCell(scr, prev_select.x + 1, prev_select.y - 1);
	DrawCell(scr, prev_select.x + 1, prev_select.y);
	DrawCell(scr, prev_select.x + 1, prev_select.y + 1);
	DrawCell(scr, prev_select.x, prev_select.y + 1);
	DrawCell(scr, prev_select.x - 1, prev_select.y + 1);
	DrawCell(scr, prev_select.x - 1, prev_select.y);
}

bool UIPixelSelector::Handle(Event e) {
	if (e != EVENT_MOUSE_BUTTON && e != EVENT_MOUSE_MOVE)
		return HANDLED_FAILURE;

	// Ignore left click if we're not already engaged.
	if (!pressed && !Hit(pointer))
		return HANDLED_FAILURE;
	
	// Filter for left click only
	if (!mouse_buttons[0] && !pressed)
		return HANDLED_FAILURE;

        if (mouse_buttons[0] && !pressed) {
		// Left button pressed
                SET_OWNER(EVENT_MOUSE_BUTTON, EVENT_MOUSE_MOVE);
                pressed = true;
        } else if (!mouse_buttons[0] && pressed) {
		// Left button released
                RELEASE_OWNER(EVENT_MOUSE_BUTTON, EVENT_MOUSE_MOVE);
                pressed = false;
        }

	auto p = pointer.From(pos);
	p.x /= zoom;
	p.y /= zoom;

	// Make sure we're not beyond grid boundaries
	if (p.x < 0) p.x = 0;
	if (p.y < 0) p.y = 0;
	if (p.x >= cols) p.x = cols - 1;
	if (p.y >= rows) p.y = rows - 1;

	prev_select = select;
	select = p;

	// Only callback when left click is released
	//if (callback && !pressed)
	if (callback)
		callback(this, e, NULL);
	
	return HANDLED_SUCCESS;
}

void UIPixelSelector::Draw(Screen *scr) {
	if (!paint) {
                // Only paint selection
                EraseSelection(scr);
		DrawSelection(scr);
		return;
        }

	UIPixelGrid::Draw(scr);
	DrawSelection(scr);
	paint = false;
}


bool UIPixelScanner::Toggle() {
	scan_mode = !scan_mode;

        if (scan_mode) {
		GrabMouse();
		SET_OWNER(EVENT_MOUSE_MOVE, EVENT_MOUSE_BUTTON);
        } else {
		ReleaseMouse();
		RELEASE_OWNER(EVENT_MOUSE_MOVE, EVENT_MOUSE_BUTTON);
        }

	return scan_mode;
}

bool UIPixelScanner::Handle(Event e) {
        if (scan_mode) {
                if (e == EVENT_MOUSE_BUTTON && mouse_buttons[0]) {
                        Toggle();
			if (callback)
				callback(this, e, NULL);
			return HANDLED_SUCCESS;
                } else if (e == EVENT_MOUSE_MOVE) {
			Scan();
			return HANDLED_SUCCESS;
                } else {
			return HANDLED_FAILURE;
                }
        }
	return UIPixelSelector::Handle(e);
}

void UIPixelScanner::Draw(Screen *scr) {
	UIPixelSelector::Draw(scr);
}

void UIPixelScanner::Scan() {
	auto p = pointer.From(-cols / 2, -rows / 2);

	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			int x = p.x + i;
			int y = p.y + j;

			pixels[INDEX(cols, i, j)] = GetPixel(x, y);
		}
        }
	paint = true;
}
