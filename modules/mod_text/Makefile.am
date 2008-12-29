## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_text.la

mod_text_la_SOURCES = mod_text.c
mod_text_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I. -I../libtext/ -I../libfont/ -I../libgrbase/
mod_text_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_text_la_LIBADD = $(LIBADD_DL) -lSDL -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm -L../libtext/.libs  -L../libfont/.libs -L../libgrbase/.libs -ltext -lfont -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_text.la
