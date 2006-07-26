bin_PROGRAMS = \
    lesson00

lesson00_SOURCES = \
    lesson00_main.cpp

noinst_HEADERS =

AM_CPPFLAGS = \
    `$(FREETYPE_CONFIG) --cflags` \
    `$(SDL_CONFIG) --cflags` \
    \
    -I$(top_srcdir)/app/example/lesson00 \
    \
    -I$(top_srcdir)/lib/core \
    -I$(top_srcdir)/lib/core/serializers \
    -I$(top_srcdir)/lib/engine2 \
    -I$(top_srcdir)/lib/gui \
    -I$(top_srcdir)/lib/gui/widgets \
    -I$(top_srcdir)/lib/input \
    -I$(top_srcdir)/lib/math \
    -I$(top_srcdir)/lib/parsers \
    -I$(top_srcdir)/lib/primitive \
    -I$(top_srcdir)/lib/render \
    -I$(top_srcdir)/lib/sound \
    -I$(top_srcdir)/lib/util

# the mapeditor is temporarily out of service
#    -I$(top_srcdir)/lib/mapeditor2 \
#

AM_LDFLAGS = \
    `$(FREETYPE_CONFIG) --libs` \
    `$(SDL_CONFIG) --libs`

lesson00_LDADD = \
    $(LIBOBJS) \
    $(top_builddir)/lib/libxrb.la
