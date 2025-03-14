EXAMPLES=	dtmf grabbutton layout #pixelgrab
CXXFLAGS=	-I/usr/local/include -I${.CURDIR}/src -Wall -fPIC -O2
LDFLAGS=	-L../../src -L/usr/local/lib

EXAMPLES:=	${EXAMPLES:@.EX.@${.CURDIR}/examples/${.EX.}/${.EX.}@}

.export CXXFLAGS
.export LDFLAGS

all: src/libui.a ${EXAMPLES}

src/libui.a:
	${MAKE} -C ${.CURDIR}/src

${EXAMPLES}: src/libui.a
	${MAKE} -C ${.TARGET:H}

clean:
.for d in ${EXAMPLES:H}
	${MAKE} -C ${d} clean
	${MAKE} -C ${.CURDIR}/src clean
.endfor

.PHONY: all clean
