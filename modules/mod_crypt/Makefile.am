## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_crypt.la

mod_crypt_la_SOURCES = mod_crypt.c
mod_crypt_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include
mod_crypt_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_crypt_la_LIBADD = $(COMMON_LDFLAGS) -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_crypt.la

