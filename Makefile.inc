# This source code is released into the public domain.

LOCALBASE?=	/usr/local
PREFIX?=	${LOCALBASE}
BINDIR?=	${PREFIX}/sbin
ETCDIR?=	${PREFIX}/etc
SHAREDIR?=	${PREFIX}/share
MANDIR?=	${SHAREDIR}/man/man
DBDIR?=		/var/db/lfjail
TESTSDIR?=	${PREFIX}/tests/lfjail

TOPDIR=		${.CURDIR:H}

CXXSTD=		c++23
CXXFLAGS+=	-W -Wall -Werror \
		-I${TOPDIR}/include \
		-I${TOPDIR}/liblfjail

MK_AUTO_OBJ=	yes
MAKEOBJDIRPREFIX=
MAKEOBJDIR:=${TOPDIR}/build/${.CURDIR:T}

# Allow installing as non-root.
SHAREOWN!=	id -nu
SHAREGRP!=	id -ng
BINOWN!=	id -nu
BINGRP!=	id -ng
MANOWN!=	id -nu
MANGRP!=	id -ng

.include <auto.obj.mk>
