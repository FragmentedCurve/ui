PROG_CXX=	dtmf
SRCS=		main.cc
LDADD+=		-lpthread -lm
MAN=
CLEANFILES+= miniaudio.h

main.cc: miniaudio.h

miniaudio.h:
	fetch https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h

.include <bsd.prog.mk>
