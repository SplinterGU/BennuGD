## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_sys.la

mod_sys_la_SOURCES = mod_sys.c
mod_sys_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include
mod_sys_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_sys_la_LIBADD = $(COMMON_LDFLAGS) -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_sys.la

