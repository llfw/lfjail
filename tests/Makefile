# This source code is released into the public domain.

ATF_TESTS_CXX=	string_utils \
		cexec
LDADD.cexec+=	-L${TOPDIR}/build/liblfjail -llfjail
LDADD+=		-lprivateatf-c++

.include <bsd.test.mk>
