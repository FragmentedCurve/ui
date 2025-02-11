EXAMPLES=dtmf pixelgrab
CXXFLAGS+= -I/usr/local/include -fPIC -Wall

.ifdef DEBUG
CXXFLAGS+= -ggdb -O0 -pg
.endif

EXAMPLES:=${EXAMPLES:@.EX.@examples/${.EX.}/${.EX.}@}

all: ui examples
ui: src/libui.a
examples: ${EXAMPLES}

src/libui.a:
	${MAKE} CXXFLAGS+="${CXXFLAGS}" -C src

${EXAMPLES}: ui
	${MAKE} CXXFLAGS+="${CXXFLAGS} -I${.CURDIR}/src" -C ${.TARGET:H}

clean:
.for d in ${EXAMPLES:H}
	${MAKE} -C ${d} clean
	${MAKE} -C src clean
.endfor

.PHONY: ui pixelgrab dtmf
