SRC = main.cc ui.cc draw.cc
CPPFLAGS = -Wall -Wno-switch

ifdef WINDOWS
SRC += os_win32.cc
CXX = x86_64-w64-mingw32-g++
CPPFLAGS += -mwindows -municode -static
else
ifdef MACOS
SRC += os_mac.cc
else
SRC += os_x11.cc
CPPFLAGS += -lX11 -lXext 
endif
endif

ifdef DEBUG
#CPPFLAGS += -ggdb -O0 -fsanitize=asan -D _DEBUG -pg
CPPFLAGS += -ggdb -O0 -D _DEBUG -pg
else
CPPFLAGS += -O2
endif

all: pixelgrab

pixelgrab: $(SRC)
	$(CXX) $(CPPFLAGS) -o $@ $(SRC)

clean:
	rm -f pixelgrab
