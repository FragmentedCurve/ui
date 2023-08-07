#include "ui.h"
#include "os.h"

#include "font.h"

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

static bool HandleWidget(Event e, UIWidget* w) {
	bool result = HANDLED_FAILURE;

	if (w->prehook[e])
		result = w->prehook[e](w, e) || result;
	
	if (w->hook[e])
		result = w->hook[e](w, e) || result;
	else
		result = w->Handle(e);

	if (w->posthook[e])
		result = w->posthook[e](w, e) || result;

	// TODO: This boolean result doesn't clearly convey the
	// status. UIHandler's return type needs to help clarify what
	// action should be taken next.
	//
	// For example, should there be a method of a prehook
	// preventing further handler execution?
	
	return result;
}

UIWidget* UIDelegate(Event e, UIWidget* w[], int n) {
	// Catch programming mistakes.
	assert(e != EVENT_NULL);
	assert(e != EVENT_LAST);
	
	// If the event has an owner, ignore normal processing and
	// delegate directly to the owner.
	if (event_owner[e]) {
		HandleWidget(e, event_owner[e]);
		return event_owner[e];
	}

	for (int i = 0; i < n; i++) {
		if (!w[i]->visible)
			continue;

		if ((e == EVENT_MOUSE_BUTTON || e == EVENT_MOUSE_MOVE) && !w[i]->Hit(pointer))
			continue;

		if (HandleWidget(e, w[i]) == HANDLED_SUCCESS)
			return w[i];
	}

	return NULL;
}

/*
struct Machine {
	int mouse[2];
	int keys[127];
	Point pointer;
};

void newUIDelegate(UIWidget* w[], int n) {
	static struct Machine* prevstate = NULL;

	if (!prevstate) {
		prevstate = new Machine;
	}
	
	for (int i = 0; i < n; i++) {
	}
}
*/

void UIDraw(Screen* scr, UIWidget *w[], int n) {
	while (--n >= 0)
		if (w[n]->visible) w[n]->Draw(scr);
}

void UILight::Draw(Screen* scr) {
	scr->DrawFill(on ? UI_LIGHT_ON : UI_LIGHT_OFF, r.From(2, 2).Resize(-2, -2));

	if (!on) {
		// Draw a thick shadow when the light is off.
		scr->DrawFill((on ? UI_LIGHT_ON : UI_LIGHT_OFF) << 1, r.Height(4));  // Left
		scr->DrawFill((on ? UI_LIGHT_ON : UI_LIGHT_OFF) << 1, r.Width(4));   // Top
	}

	// Top hilite
	scr->DrawHLine(UI_SURFACE_SHADOW, r.p, r.xw - 1);                   // Outer
	scr->DrawHLine(UI_SURFACE_SHADOW, r.p.From(0, 1), r.xw - 2);        // Inner

	// Left hilite
	scr->DrawVLine(UI_SURFACE_SHADOW, r.p, r.yw - 1);                   // Outer
	scr->DrawVLine(UI_SURFACE_SHADOW, r.p.From(1, 0), r.yw - 2);        // Inner

	// Bottom shadow
	scr->DrawHLine(UI_SURFACE_HILITE, r.p.From(1, r.yw - 1), r.xw - 1); // Outer
	scr->DrawHLine(UI_SURFACE_HILITE, r.p.From(2, r.yw - 2), r.xw - 2); // Inner

	// Right shadow
	scr->DrawVLine(UI_SURFACE_HILITE, r.p.From(r.xw - 1, 1), r.yw - 1); // Outer
	scr->DrawVLine(UI_SURFACE_HILITE, r.p.From(r.xw - 2, 2), r.yw - 2); // Inner
}

void UIToggle::Draw(Screen* scr) {
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
}

// TODO: Clean this up. It was quickly hacked out just to make it work.
void HexFloat::DrawFont(Screen* scr, bool *gylph, int len, int x0, int y0, Pixel c) {
	auto zoom = 2;
	for (int i = 0; i < len; i++) {
		auto x = x0 + zoom * (i % 3);
		auto y = y0 + zoom * (i / 3);

		if (gylph[i])
		  scr->DrawFill(c /*UI_TEXT_FG*/, Rect(Point(x, y), Point(x + zoom, y + zoom)));
	}
}

void HexFloat::Draw(Screen* scr) {
	// TODO: Clean this up. It was quickly hacked out just to make it work.
	scr->DrawFill(UI_TEXT_BG, r);
	scr->DrawRect(UI_TEXT_BORDER, r);

	for (int i = 1; i < (int)sizeof(color_s) - 1; i++) {
		int c = color_s[i] - '0';
		if (color_s[i] >= 'a')
			c = color_s[i] - 'a' + 0xa;

		Pixel color;
		if (i == 1 || i == 2)
			color = RGB(0xc0, 0, 0); // 0xc00000;
		else if (i == 3 || i == 4)
			color = RGB(0, 0xc0, 0); // 0x00c000;
		else
			color = RGB(0, 0, 0xc0); // 0xc0;

		DrawFont(scr, gylphs[c], sizeof(gylphs[c]), r.p.x + (5 + 8 * i), r.p.y + 8, color);
	}
}

void UIButton::Draw(Screen* scr) {
	if (pressed)
		Draw(scr, UI_SURFACE_SHADOW, UI_SURFACE_HILITE, UI_SURFACE_FG);
	else
		Draw(scr, UI_SURFACE_HILITE, UI_SURFACE_SHADOW, UI_SURFACE_BG);
}

void UIButton::Draw(Screen* scr, Pixel hilite, Pixel shadow, Pixel bg) {
	scr->DrawFill(bg, r);

	// Top hilite
	scr->DrawHLine(hilite, r.p.From(0, 1), r.xw - 2);        // Outer
	scr->DrawHLine(hilite, r.p.From(0, 2), r.xw - 3);        // Inner

	// Left hilite
	scr->DrawVLine(hilite, r.p.From(1, 0), r.yw - 2);        // Outer
	scr->DrawVLine(hilite, r.p.From(2, 0), r.yw - 3);        // Inner

	// Bottom shadow
	scr->DrawHLine(shadow, r.p.From(2, r.yw - 2), r.xw - 2); // Outer
	scr->DrawHLine(shadow, r.p.From(3, r.yw - 3), r.xw - 3); // Inner

	// Right shadow
	scr->DrawVLine(shadow, r.p.From(r.xw - 2, 2), r.yw - 2); // Outer
	scr->DrawVLine(shadow, r.p.From(r.xw - 3, 3), r.yw - 3); // Inner

	// Border
	scr->DrawRect(UI_DARKEST, r);
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
		pixels[i] = UI_LIGHTEST;
}

UIPixelGrid::~UIPixelGrid() {
	delete[] pixels;
}

Point UIPixelGrid::CellPosition(int x, int y) {
	auto p0 = r.p.From(x * zoom, y * zoom);	
	return p0;
}

void UIPixelGrid::Draw(Screen* scr) {
	for (int i = 0; i < cols; i++)
		for (int j = 0; j < rows; j++)
			DrawCell(scr, i, j);
}

void UIPixelGrid::DrawCell(Screen* scr, int x, int y) {
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= cols) x = cols - 1;
	if (y >= rows) y = rows - 1;

	auto p0 = CellPosition(x, y);
	auto p1 = p0.From(zoom, zoom);

	// Fill square
	scr->DrawFill(pixels[INDEX(cols, x, y)], Rect(p0, p1));

	// Draw border
	scr->DrawHLine(UI_DARKEST, p0, zoom);
	scr->DrawVLine(UI_DARKEST, p0, zoom);

	if (x == cols - 1)
		scr->DrawVLine(UI_DARKEST, p0.From(zoom - 1, 0), zoom);
	if (y == rows - 1)
		scr->DrawHLine(UI_DARKEST, p0.From(0, zoom - 1), zoom);
}

void UIPixelSelector::DrawSelection(Screen* scr) {
	// Weird inverted color
	auto c = (WHITE ^ pixels[INDEX(cols, select.x, select.y)]) | RED;

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

	scr->DrawRect(c, Rect(p0, p1));
	scr->DrawRect(c, Rect(q0, q1));
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

	auto p = pointer.From(r.p);
	p.x /= zoom;
	p.y /= zoom;

	// Make sure we're not beyond grid boundaries
	if (p.x < 0) p.x = 0;
	if (p.y < 0) p.y = 0;
	if (p.x >= cols) p.x = cols - 1;
	if (p.y >= rows) p.y = rows - 1;

	prev_select = select;
	select = p;

	if (callback)
		callback(this, e, NULL);

	return HANDLED_SUCCESS;
}

void UIPixelSelector::Draw(Screen* scr) {
	EraseSelection(scr);
	DrawSelection(scr);

	UIPixelGrid::Draw(scr);
	DrawSelection(scr);
}
