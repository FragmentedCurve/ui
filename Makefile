SRC = main.cc ui.cc draw.cc
SRC_X11 = $(SRC) os_x11.cc

DEBUG_FLAGS = -ggdb -O0 -fsanitize=address -D _DEBUG -pg
CPPFLAGS =  -lX11 -lXext -Wall -Wno-switch $(DEBUG_FLAGS)

all: pixelgrab

pixelgrab: $(SRC_X11)
	$(CXX) $(CPPFLAGS) -o $@ $(SRC_X11)

clean:
	rm -f pixelgrab
