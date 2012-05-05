## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_joy.la

libmod_joy_la_SOURCES = mod_joy.c
libmod_joy_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I../libjoy
libmod_joy_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_joy_la_LIBADD = $(COMMON_LDFLAGS) -L../libjoy/.libs -ljoy -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_joy.la

