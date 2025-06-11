# This source code is released into the public domain.

.include "Makefile.inc"

SUBDIR=		liblfjail lfjail tests
TOPDIR=		${.CURDIR}
.export TOPDIR

HAS_TESTS=
SUBDIR+= tests

.include <bsd.subdir.mk>
