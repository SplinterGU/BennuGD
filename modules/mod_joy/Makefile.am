## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_joy.la

mod_joy_la_SOURCES = mod_joy.c
mod_joy_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I../libjoy
mod_joy_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_joy_la_LIBADD = $(COMMON_LDFLAGS) -L../libjoy/.libs -ljoy -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_joy.la

