#include "ui.h"

#include <cstdio>

UIReaction UIImpacted(UIRawInput state, UIWidget* root) {
	UIReaction out = {
		.pressed = NULL,
		.clicked = NULL,
	};

	static UIRawInput pstate; // Previous state
	static UIReaction pout;   // Previous output

	// TODO: General for more than left click Handle primary
	// pointer clicks
	if (state.m[0] && !pstate.m[0]) {
		// Something was pressed.
		out.pressed = root->Hit(state.pointer);
		if (out.pressed) {
			out.pressed->pressed = true;
			out.pressed->HandlePress(state.pointer);
		}
	} else if (!state.m[0] && pstate.m[0]) {
		// Something might've been clicked.
		if (pout.pressed && (root->Hit(state.pointer) == pout.pressed)) {
			out.clicked = pout.pressed;
			out.clicked->HandleClick(state.pointer);
		}

		// Otherwise, no click, only a release
		if (pout.pressed) {
			pout.pressed->pressed = false;
		}
	} else {
		// Previously pressed persists
		out.pressed = pout.pressed;
	}

	// TODO: Should drags be a property and handled here?

	if (state.screen_width != pstate.screen_width || state.screen_height != pstate.screen_height) {
		// Screen was resized.
		if (root->fill_screen) {
			root->SetSize(state.screen_width, state.screen_height);
			root->Move(0, 0);
		}
	}

	pstate = state;
	pout = out;
	return out;
}

void UIDraw(UIScreen* scr, UIWidget* root) {
	// TODO: Only draw what needs to be repainted. We waste too
	// much time drawing everything.
	if (!root || !root->visible)
		return;

	root->Draw(scr);
	scr = scr->Subset(root->r);

	for (UIWidget* walk = root->childhead; walk; walk = walk->next) {
		UIDraw(scr, walk);
	}
}
