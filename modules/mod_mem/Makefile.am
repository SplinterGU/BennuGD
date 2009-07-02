## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_mem.la

mod_mem_la_SOURCES = mod_mem.c
mod_mem_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include
mod_mem_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_mem_la_LIBADD = $(COMMON_LDFLAGS)

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_mem.la

