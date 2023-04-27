SRC = main.cc ui.cc draw.cc splash.cc
SRC_X11 = $(SRC) os_x11.cc

DEBUG_FLAGS = -ggdb -O0  -pg -fsanitize=address
CPPFLAGS =  -lX11 -lXext -Wall -Wno-switch $(DEBUG_FLAGS)

all: pixelgrab

pixelgrab: $(SRC_X11)
	$(CXX) $(CPPFLAGS) -o $@ $(SRC_X11)

clean:
	rm -f pixelgrab
