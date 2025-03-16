all:
	${MAKE} -C ${.CURDIR}/src
	${MAKE} -C ${.CURDIR}/examples

clean:
	${MAKE} -C ${.CURDIR}/src clean
	${MAKE} -C ${.CURDIR}/examples clean

.PHONY: all clean
