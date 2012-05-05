## Process this mod_effectss with automake to produce Makemod_effectss.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_effects.la

libmod_effects_la_SOURCES = mod_effects.c
libmod_effects_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include -I. -I../libgrbase/
libmod_effects_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_effects_la_LIBADD = $(COMMON_LDFLAGS) -L$(BGD_PATH)/bgdrtm/src/.libs -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_effects.la

