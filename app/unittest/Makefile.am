bin_PROGRAMS = \
    unittest

noinst_HEADERS = \
    unittest_base.h \
    unittest_controller.h \
    \
    tests/unittest_matrix2.h \
    tests/unittest_serializer.h \
    tests/unittest_simpletransform2.h \
    tests/unittest_transform2.h \
    tests/unittest_vector.h

unittest_SOURCES = \
    main.cpp \
    unittest_base.cpp \
    unittest_controller.cpp \
    \
    tests/unittest_matrix2.cpp \
    tests/unittest_serializer.cpp \
    tests/unittest_simpletransform2.cpp \
    tests/unittest_transform2.cpp \
    tests/unittest_vector.cpp

AM_CPPFLAGS = \
    `$(FREETYPE_CONFIG) --cflags` \
    `$(SDL_CONFIG) --cflags` \
    \
    -I$(top_srcdir)/lib/core \
    -I$(top_srcdir)/lib/core/serializers \
    -I$(top_srcdir)/lib/engine2 \
    -I$(top_srcdir)/lib/gui \
    -I$(top_srcdir)/lib/gui/widgets \
    -I$(top_srcdir)/lib/input \
    -I$(top_srcdir)/lib/mapeditor2 \
    -I$(top_srcdir)/lib/math \
    -I$(top_srcdir)/lib/parsers \
    -I$(top_srcdir)/lib/primitive \
    -I$(top_srcdir)/lib/render \
    -I$(top_srcdir)/lib/sound \
    -I$(top_srcdir)/lib/util \
    \
    -I$(top_srcdir)/app/unittest \
    -I$(top_srcdir)/app/unittest/tests

AM_LDFLAGS = \
    `$(FREETYPE_CONFIG) --libs` \
    `$(SDL_CONFIG) --libs`

unittest_LDADD = \
    $(LIBOBJS) \
    $(top_builddir)/lib/libxrb.la

