# This source code is released into the public domain.

PROG_CXX=	lfjail
SRCS=		lfjail.cc \
		command_map.cc \
		config_store.cc \
		config_string.cc \
		config_value.cc \
		context.cc \
		c_config.cc \
		c_jail.cc \
		ensure_dir.cc \
		jail.cc \
		zfs.cc
MAN=		lfjail.8
CXXFLAGS+=	-DLFJAIL_DBDIR="\"${DBDIR}\"" \
		-I${LOCALBASE}/include \
		-I${TOPDIR}/liblfjail
LDADD+=		-L${LOCALBASE}/lib -Wl,-R${LOCALBASE}/lib -lucl \
		-L${TOPDIR}/build/liblfjail -llfjail

SED?=		sed
REPLACE=	sed	-e 's,__PREFIX__,${PREFIX},g' \
			-e 's,__CONFDIR__,${CONFDIR},g' \
			-e 's,__LIBDIR__,${LIBDIR},g' \
			-e 's,__BINDIR__,${BINDIR},g'

lfjail.8: lfjail.8.in
	${REPLACE} <${.ALLSRC} >${.TARGET}
CLEANFILES+=lfjail.8

beforeinstall:
	install -d ${DESTDIR}${BINDIR}
	install -d ${DESTDIR}${BINDIR}
	install -d ${DESTDIR}${MANDIR}8

.OBJDIR: build

.include <bsd.prog.mk>
