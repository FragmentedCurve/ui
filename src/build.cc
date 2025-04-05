// TODO: Implement debug flags

#include <cassert>
#define UIASSERT assert // TODO: Turn on for debug builds only

#include "ui.h"

// TODO: Select backend at compile-time
#include "os_x11.cc"

#include "ui_core.cc"
#include "ui_font.cc"
#include "ui_widgets.cc"
