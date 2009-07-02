## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_ffi.la

mod_ffi_la_SOURCES = mod_ffi.c
mod_ffi_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include -I/usr/local/lib/libffi-3.0.8/include
mod_ffi_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_ffi_la_LIBADD = $(COMMON_LDFLAGS) -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm -L/usr/local/lib -lffi

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_ffi.la

