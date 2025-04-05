CXXFLAGS+=	 -fPIC -std=c++17 -Wall -pedantic -I ${.CURDIR}/src -I /usr/local/include
CXXFLAGS_amd64+=	-msse2 -mavx2
CXXFLAGS+=	${CXXFLAGS_${MACHINE}}

.ifdef DEBUG
CXXFLAGS+=	-ggdb -pg -O0 -DUI_INTERNAL_DEBUG
.else
CXXFLAGS+=	-O3 -s
.endif

SRCS!=	find ${.CURDIR}/src/ -type f -depth 1 -name '*.cc' -or -name '*.h'

all: libui.o libui.so

libui.o: ${SRCS}
	${CXX} ${CXXFLAGS} -c -o libui.o ${.CURDIR}/src/build.cc

libui.so: ${SRCS}
	${CXX} ${CXXFLAGS} -shared -o libui.so ${.CURDIR}/src/build.cc

clean:
	rm -f libui.so libui.o

.PHONY: clean
