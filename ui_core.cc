#include "ui.h"

UIWidget* UIWidget::Hit(UIPoint p) {
	if (!r.Hit(p) || !visible) {
		return NULL;
	}

	// this is the new parent so p must be relative to this.r
	p = UIPoint(p.x - r.p.x, p.y - r.p.y);

	for (UIWidget* walk = childtail; walk; walk = walk->prev) {
		UIWidget* hit = walk->Hit(p);
		if (hit) {
			return hit;
		}
	}

	return this;
}

UIWidget* UIWidget::Hit(int x, int y) {
	return Hit(UIPoint(x, y));
}


void UIWidget::Move(int x, int y) {
	r = UIRect(x, y, r.xw, r.yw);
}

void UIWidget::Move(UIPoint p) {
	Move(p.x, p.y);
}

void UIWidget::Push(int x, int y) {
	r = r.From(x, y);
}
	
void UIWidget::Push(UIPoint p) {
	Push(p.x, p.y);
}

void UIWidget::Resize(int dx, int dy) {
	r = r.Resize(dx, dy);
}

UIRect UIWidget::Abs() {
	UIRect result = r;
	for (UIWidget* walk = parent; walk; walk = walk->parent) {
		result = result.From(walk->r.p);
	}
	return result;
}

UIWidget* UIWidget::Parent(UIWidget* parent) {
	if (!parent) {
		return this;
	}

	if (parent->childhead) {
		next = parent->childhead;
		next->prev = this;
		parent->childhead = this;
		this->parent = parent;
	} else {
		next = NULL;
		prev = NULL;
		parent->childhead = this;
		parent->childtail = this;
		this->parent = parent;
	}

	return this;
}


#undef Children
UIWidget* UIWidget::__Children(UIWidget* w, ...) {
	if (!w) {
		return this;
	}
	w->Parent(this);

	va_list ap;
	va_start(ap, w);
	for (UIWidget* child = va_arg(ap, UIWidget*); child; child = va_arg(ap, UIWidget*)) {
		child->Parent(this);
	}
	va_end(ap);

	return this;
}


UIScreen::UIScreen(int xw, int yw): UIScreen(new uint32_t[xw * yw], xw, yw, xw) {}
UIScreen::UIScreen(uint32_t* pixels, int xw, int yw) : xw(xw), yw(yw), pitch(xw), pixels(pixels)  {}
UIScreen::UIScreen(uint32_t* pixels, int xw, int yw, int pitch) : xw(xw), yw(yw), pitch(pitch), pixels(pixels)  {}

void UIScreen::DrawHLine(UIPixel c, UIPoint p, int width) {
	DrawHLine(c, p, width, UIRect(0, 0, xw, yw));
}

void UIScreen::DrawHLine(UIPixel c, UIPoint p, int width, UIRect clip) {
	// Completely outside the clip area
	if (p.x >= clip.p.x + clip.xw || p.y < clip.p.y || p.y >= clip.p.y + yw)
		return;

	// Clip left
	if (p.x < clip.p.x) {
		width -= clip.p.x - p.x;
		p.x = clip.p.x;
	}

	// Clip right
	if (p.x + width > clip.p.x + clip.xw)
		width = clip.p.x + clip.xw - p.x;

	while (--width >= 0)
		pixels[UI_INDEX(pitch, p.x + width, p.y)] = c;
}

void UIScreen::DrawVLine(UIPixel c, UIPoint p, int height) {
	DrawVLine(c, p, height, UIRect(0, 0, xw, yw));
}

void UIScreen::DrawVLine(UIPixel c, UIPoint p, int height, UIRect clip) {
	// Completely outside the clip area
	if (p.y >= clip.p.y + clip.yw || p.x < clip.p.x || p.x >= clip.p.x + clip.xw)
		return;

	// Clip top
	if (p.y < clip.p.y) {
		height -= clip.p.y - p.y;
		p.y = clip.p.y;
	}

	// Clip bottom
	if (p.y + height > clip.p.y + clip.yw)
		height = clip.p.y + clip.yw - p.y;

	while (--height >= 0)
		pixels[UI_INDEX(pitch, p.x, p.y + height)] = c;
}

void UIScreen::DrawFill(UIPixel c, UIRect r) {
	for (int i = 0; i < r.yw; i++)
		DrawHLine(c, r.p.From(0, i), r.xw);
}

void UIScreen::DrawRect(UIPixel c, UIRect r) {
	DrawRect(c, r, UIRect(0, 0, xw, yw));
}

void UIScreen::DrawRect(UIPixel c, UIRect r, UIRect clip) {
	// Top
	DrawHLine(c, r.p, r.xw, clip);
	// Bottom
	DrawHLine(c, r.p.From(0, r.yw - 1), r.xw, clip);
	// Left
	DrawVLine(c, r.p, r.yw, clip);
	// Right
	DrawVLine(c, r.p.From(r.xw - 1, 0), r.yw, clip);
}

UIScreen* UIScreen::Subset(UIRect r) {
	return new UIScreen(pixels + UI_INDEX(pitch, r.p.x, r.p.y), r.xw, r.yw, pitch);
}

/* TODO: Implement Resize
void Screen::Resize(int xw, int xy) {
	if (xw < this->xw && yw< this->yw) {
		return;
	}

	if (xw * yw <= (this->xw + pitch) * this->yw) {
		this->xw = xw;
		this->xy = yw;
		this->pitch =
			// (pitch + xw)
	}

	if (xw < this->xw && yw < this->yw) {
		return;
	}
}
*/
