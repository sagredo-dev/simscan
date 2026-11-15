SUBDIRS = cdb

CDB_LIBS = cdb/cdb.a cdb/buffer.a cdb/unix.a cdb/byte.a cdb/alloc.a

simscanbindir = @qmaildir@/bin
simscanbin_PROGRAMS = simscan simscanmk

simscan_SOURCES = simscan.c
simscan_LDADD = $(CDB_LIBS)

simscanmk_SOURCES = simscanmk.c
simscanmk_LDADD = $(CDB_LIBS)

install-simscanbinPROGRAMS:
	$(INSTALL_PROGRAM) simscan @qmaildir@/bin/simscan
	$(INSTALL_PROGRAM) simscanmk @qmaildir@/bin/simscanmk
	$(INSTALL) -m 750 -d @workdir@
	chown @ENABLE_USER@ @workdir@ @qmaildir@/bin/simscan
	chmod 4711 @qmaildir@/bin/simscan
