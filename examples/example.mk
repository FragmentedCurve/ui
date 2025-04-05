CXXFLAGS+=	 -ggdb -pg -O0 -DUI_INTERNAL_DEBUG -std=c++17 -Wall -pedantic -O2 -I ../../src -I /usr/local/include -L/usr/local/lib -L../../
LDADD+=	-l:libui.o -lX11 -lXext

.if "${:!which fetch!}" != ""
FETCH?= fetch
.elif "${:!which curl!}" != ""
FETCH?= curl -O
.elif "${:!which wget!}" != ""
FETCH?= wget
.else
.error NO HTTP FETCH COMMAND FOUND
.endif

LIBUI_SRCS!=	find ../../src -type f -name '*.cc' -or -name '*.h'

.MAIN: ${PROG}

${PROG}: ${SRCS} ../../libui.o
	${CXX} ${CXXFLAGS} -o ${PROG} ${SRCS} ${LDADD}

../../libui.o: ${LIBUI_SRCS}
	${MAKE} -C ../../

clean:
	rm -f ${CLEANFILES} ${PROG}
