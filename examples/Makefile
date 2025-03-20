EXAMPLES=	imageviewer grabbutton layout calc dtmf
LDADD=		-l:libui.a -lX11 -lXext

.export LDADD

all:
.for dir in ${EXAMPLES}
	${MAKE} -C ${dir}
.endfor

clean:
.for dir in ${EXAMPLES}
	${MAKE} -C ${dir} clean
.endfor

.PHONY: all clean
