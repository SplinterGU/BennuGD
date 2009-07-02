## Process this mod_effectss with automake to produce Makemod_effectss.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_effects.la

mod_effects_la_SOURCES = mod_effects.c
mod_effects_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include -I. -I../libgrbase/
mod_effects_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_effects_la_LIBADD = $(COMMON_LDFLAGS) -L$(BGD_PATH)/bgdrtm/src/.libs -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_effects.la

