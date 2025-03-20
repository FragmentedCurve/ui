PROG_CXX=	imageviewer
SRCS=		main.cc
MAN=
CLEANFILES+=	stb_image.h stb_image_resize2.h

main.cc: stb_image.h stb_image_resize2.h

stb_image.h:
	${FETCH} https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h

stb_image_resize2.h:
	${FETCH} https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image_resize2.h

.include <bsd.prog.mk>
