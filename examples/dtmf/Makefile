PROG_CXX=	dtmf
SRCS=		main.cc
LDADD=		-l:libui.a -lX11 -lXext -lpthread -lm
CXXFLAGS+=	-I../../src/ -I/usr/local/include -L../../src -L/usr/local/lib -msse2 -mavx2 -O3
MAN=
.include <bsd.prog.mk>
