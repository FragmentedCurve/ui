CXXFLAGS+=	-std=c++17 -Wall -pedantic -O2 -I ${.CURDIR}/src -I /usr/local/include
CXXFLAGS_amd64+=	-msse2 -mavx2
CXXFLAGS+=	${CXXFLAGS_${MACHINE}}
LDFLAGS+=	-L ${.CURDIR}/src -L /usr/local/lib

FETCH=fetch

.export CXXFLAGS
.export LDFLAGS
.export FETCH

all: build-all

build:
	${MAKE} -C ${.CURDIR}/src

build-all: build
	${MAKE} -C ${.CURDIR}/examples

clean:
	${MAKE} -C ${.CURDIR}/src clean
	${MAKE} -C ${.CURDIR}/examples clean

.PHONY: build build-all clean
