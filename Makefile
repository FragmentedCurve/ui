CXXFLAGS+=	-O2 -I ${.CURDIR}/src -I /usr/local/include  -msse2 -mavx2
LDFLAGS+=	-L ${.CURDIR}/src -L /usr/local/lib

.export CXXFLAGS
.export LDFLAGS

all:
	${MAKE} -C ${.CURDIR}/src
	${MAKE} -C ${.CURDIR}/examples

clean:
	${MAKE} -C ${.CURDIR}/src clean
	${MAKE} -C ${.CURDIR}/examples clean

.PHONY: all clean
