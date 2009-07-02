## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = librender.la

librender_la_SOURCES = librender.c g_fade.c g_frame.c g_instance.c g_object.c g_rects.c g_screen.c \
                       scaler_hq2x.c scaler_normal.c scaler_scale2x.c scaler_scanline.c
librender_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I. -I../libgrbase/ -I../libvideo/ -I../libblit/
librender_la_LDFLAGS = -s -module -no-undefined -avoid-version
librender_la_LIBADD = $(COMMON_LDFLAGS) -lSDL -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm -L../libvideo/.libs -lvideo -L../libblit/.libs -lblit -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = librender.la
