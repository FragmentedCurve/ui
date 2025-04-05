EXAMPLES=	imageviewer grabbutton layout dtmf

all:
.for dir in ${EXAMPLES}
	${MAKE} -C ${dir}
.endfor

clean:
.for dir in ${EXAMPLES}
	${MAKE} -C ${dir} clean
.endfor

.PHONY: clean
