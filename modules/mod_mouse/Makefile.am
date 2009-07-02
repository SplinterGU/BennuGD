## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_mouse.la

mod_mouse_la_SOURCES = mod_mouse.c
mod_mouse_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include
mod_mouse_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_mouse_la_LIBADD = $(COMMON_LDFLAGS)

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_mouse.la

