bin_PROGRAMS = bgdc

.rc.o:
	$(WINDRES) $< -I../../include -o $@

INCLUDES = $(COMMON_CFLAGS) -I../../include -I../include -D__BGDC__

bgdc_LDADD = $(COMMON_LDFLAGS)

bgdc_LDFLAGS = $(COMMON_LDFLAGS) $(ZLIBS) $(CRYPT_LDFLAGS) --as-needed

SOURCE_FILES = main.c main_core.c token.c identifier.c segment.c \
              constants.c strings.c varspace.c procedure.c codeblock.c \
              c_main.c c_data.c c_code.c c_debug.c dcbw.c \
              error.c \
              sysstub.c typedef.c \
              ../../common/debug.c \
              ../../common/b_crypt.c \
              ../../common/files.c \
              ../../common/xctype.c

if HAVE_WINDRES
bgdc_SOURCES = $(SOURCE_FILES) bgdc.rc
else
bgdc_SOURCES = $(SOURCE_FILES)
endif
