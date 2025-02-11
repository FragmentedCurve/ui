all: ui dtmf pixelgrab
ui: src/libui.a
dtmf: examples/dtmf/dtmf
pixelgrab: examples/pixelgrab/pixelgrab

src/libui.a:
	${MAKE} -C src

examples/dtmf/dtmf: ui
	${MAKE} -C examples/dtmf

examples/pixelgrab/pixelgrab: ui
	${MAKE} -C examples/pixelgrab

clean:
	${MAKE} -C examples/pixelgrab clean
	${MAKE} -C examples/dtmf clean
	${MAKE} -C src clean

.PHONY: ui pixelgrab dtmf
