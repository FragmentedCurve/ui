DEBUG=

SRC = main.cc ui_core.cc ui_widgets.cc ui_font.cc
CPPFLAGS = -Wall -Wno-switch -I/usr/local/include -L/usr/local/lib -ggdb -O0 -D _DEBUG -pg

ifdef WINDOWS
SRC += os_win32.cc
CXX = x86_64-w64-mingw32-g++
CPPFLAGS += -DBGR -mwindows -municode -static
else
ifdef MACOS
SRC += os_mac.cc
else
SRC += os_x11.cc
CPPFLAGS += -lX11 -lXext 
endif
endif

ifdef DEBUG
#CPPFLAGS += -ggdb -O0 -fsanitize=leak -D _DEBUG -pg
CPPFLAGS += -ggdb -O0 -D _DEBUG -pg
else
CPPFLAGS += -O2
endif

all: pixelgrab

pixelgrab: $(SRC)
	$(CXX) $(SRC) $(CPPFLAGS) -o $@

.PHONY: test

test:
	$(CXX)  test.cc ui_core.cc ui_widgets.cc ui_font.cc os_x11.cc $(CPPFLAGS) -o $@

tone:
	$(CXX)  tone.cc ui_core.cc ui_font.cc ui_widgets.cc os_x11.cc $(CPPFLAGS) -o $@

clean:
	rm -f pixelgrab tone test
