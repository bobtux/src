#	$OpenBSD: Makefile,v 1.15 2017/02/12 13:15:50 krw Exp $

.include <bsd.own.mk>

.PATH:	${.CURDIR}/../../../sbin/dhclient

CLIENT_PATH='"PATH=/usr/bin:/usr/sbin:/bin:/sbin"'
CPPFLAGS+= -I${.CURDIR}/../../../sbin/dhclient
SRCS=	dhclient.c clparse.c dispatch.c bpf.c options.c \
	conflex.c log.c kroute.c packet.c \
	tables.c parse.c privsep.c
PROG=	dhclient
LDADD+=	-lutil
DPADD+= ${LIBUTIL}

.include <bsd.prog.mk>
.include <bsd.subdir.mk>
