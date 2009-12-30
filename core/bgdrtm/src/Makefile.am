## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libbgdrtm.la

libbgdrtm_la_SOURCES = dcbr.c sysprocs.c strings.c dirs.c copy.c misc.c instance.c interpreter.c ../../common/debug.c ../../common/b_crypt.c ../../common/files.c ../../common/varspace_file.c ../../common/xctype.c
libbgdrtm_la_CFLAGS = $(COMMON_CFLAGS) -I../../include -I../include -D__BGDRTM__ -DDEBUG
libbgdrtm_la_LDFLAGS = -s -module -no-undefined -avoid-version -export-all-symbols
libbgdrtm_la_LIBADD = $(COMMON_LDFLAGS) $(ZLIBS) $(CRYPT_LDFLAGS)

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libbgdrtm.la

