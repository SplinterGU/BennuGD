bin_PROGRAMS = bgdi

.rc.o:
	$(WINDRES) $< -I../../include -o $@

INCLUDES = $(COMMON_CFLAGS) -I../../include -I../include -I../../bgdrtm/include -DDEBUG -D__BGDI__

bgdi_LDADD = $(COMMON_LDFLAGS) -L../../bgdrtm/src/.libs -lbgdrtm

bgdi_LDFLAGS = --as-needed

SOURCE_FILES = main.c

noinst_HEADERS = ../include/bgdi.h \
                 ../../bgdrtm/include/bgdrtm.h \
                 ../../include/files.h ../../include/xctype.h \
                 ../../include/offsets.h ../../include/pslang.h

if HAVE_WINDRES
bgdi_SOURCES = $(SOURCE_FILES) bgdi.rc
else
bgdi_SOURCES = $(SOURCE_FILES)
endif
