##############################################################################
# the ultimate goal is to build libxrb.la.  all the source is compiled
# directly into it, except for the parsers, which are each compiled
# into their own libraries and then included in libxrb.la.
##############################################################################

lib_LTLIBRARIES = \
    libxrb.la

libxrb_la_SOURCES = \
    core/xrb_compiletimeasserts.cpp \
    core/xrb_event.cpp \
    core/xrb_eventhandler.cpp \
    core/xrb_eventqueue.cpp \
    core/xrb_framehandler.cpp \
    core/xrb_resourcelibrary.cpp \
    core/xrb_serializer.cpp \
    core/xrb_signalhandler.cpp \
    core/xrb_singletons.cpp \
    \
    core/serializers/xrb_binaryfileserializer.cpp \
    \
    engine2/xrb_engine2_compound.cpp \
    engine2/xrb_engine2_entity.cpp \
    engine2/xrb_engine2_events.cpp \
    engine2/xrb_engine2_object.cpp \
    engine2/xrb_engine2_objectlayer.cpp \
    engine2/xrb_engine2_polygon.cpp \
    engine2/xrb_engine2_quadtree.cpp \
    engine2/xrb_engine2_sprite.cpp \
    engine2/xrb_engine2_visibilityquadtree.cpp \
    engine2/xrb_engine2_world.cpp \
    \
    gui/xrb_gui_events.cpp \
    gui/xrb_sizeproperties.cpp \
    gui/xrb_widget.cpp \
    gui/xrb_widgetbackground.cpp \
    gui/xrb_widgetskin.cpp \
    gui/xrb_widgetskinhandler.cpp \
    \
    gui/widgets/xrb_button.cpp \
    gui/widgets/xrb_cellpaddingwidget.cpp \
    gui/widgets/xrb_checkbox.cpp \
    gui/widgets/xrb_dialog.cpp \
    gui/widgets/xrb_filedialog.cpp \
    gui/widgets/xrb_filepanel.cpp \
    gui/widgets/xrb_label.cpp \
    gui/widgets/xrb_layout.cpp \
    gui/widgets/xrb_lineedit.cpp \
    gui/widgets/xrb_modalwidget.cpp \
    gui/widgets/xrb_progressbar.cpp \
    gui/widgets/xrb_spacerwidget.cpp \
    gui/widgets/xrb_textwidget.cpp \
    gui/widgets/xrb_widgetstack.cpp \
    \
    input/xrb_input.cpp \
    input/xrb_input_events.cpp \
    input/xrb_key.cpp \
    input/xrb_keyrepeater.cpp \
    \
    math/xrb_math.cpp \
    math/xrb_matrix2.cpp \
    math/xrb_polynomial.cpp \
    math/xrb_simpletransform2.cpp \
    math/xrb_transform2.cpp \
    math/xrb_vector.cpp \
    \
    primitive/xrb_color.cpp \
    primitive/xrb_gltexture.cpp \
    primitive/xrb_ntuple.cpp \
    primitive/xrb_rect.cpp \
    primitive/xrb_texture.cpp \
    \
    render/xrb_engine2_worldview.cpp \
    render/xrb_engine2_worldviewwidget.cpp \
    render/xrb_font.cpp \
    render/xrb_fontface.cpp \
    render/xrb_gl.cpp \
    render/xrb_render.cpp \
    render/xrb_rendercontext.cpp \
    render/xrb_screen.cpp \
    render/xrb_screencoord.cpp \
    \
    util/xrb_bitcache.cpp \
    util/xrb_bitcachedfile.cpp \
    util/xrb_characterfilter.cpp \
    util/xrb_frameratecalculator.cpp \
    util/xrb_indentformatter.cpp \
    util/xrb_tokenizer.cpp \
    util/xrb_transformation.cpp \
    util/xrb_utf8.cpp \
    util/xrb_util.cpp

# the compression stuff is temporarily out of service
#    core/serializers/xrb_compressionserializer.cpp \
#    util/xrb_bitcachedcompression.cpp \
#    util/xrb_huffman.cpp \
#

# the mapeditor is temporarily out of service
#    mapeditor2/xrb_mapeditor2_compound.cpp \
#    mapeditor2/xrb_mapeditor2_compoundentity.cpp \
#    mapeditor2/xrb_mapeditor2_compoundvertex.cpp \
#    mapeditor2/xrb_mapeditor2_entity.cpp \
#    mapeditor2/xrb_mapeditor2_entitypropertiespanel.cpp \
#    mapeditor2/xrb_mapeditor2_mainwidget.cpp \
#    mapeditor2/xrb_mapeditor2_object.cpp \
#    mapeditor2/xrb_mapeditor2_objectlayer.cpp \
#    mapeditor2/xrb_mapeditor2_objectpropertiespanel.cpp \
#    mapeditor2/xrb_mapeditor2_polygon.cpp \
#    mapeditor2/xrb_mapeditor2_sprite.cpp \
#    mapeditor2/xrb_mapeditor2_spriteentity.cpp \
#    mapeditor2/xrb_mapeditor2_visibilityquadtree.cpp \
#    mapeditor2/xrb_mapeditor2_world.cpp \
#    mapeditor2/xrb_mapeditor2_worldview.cpp \
#    \
#

libxrb_la_LIBADD = \
    libdatafileparser.la \
    liblinecalculatorparser.la

##############################################################################
# the intermediate libraries shouldn't be intalled.
##############################################################################

noinst_LTLIBRARIES = \
    libdatafileparser.la \
    liblinecalculatorparser.la

##############################################################################
# these headers will be installed in $(prefix)/include/xrb/
##############################################################################

include_HEADERS = \
    core/xrb.h \
    core/xrb_assert.h \
    core/xrb_debug.h \
    core/xrb_delete.h \
    core/xrb_endian.h \
    core/xrb_enums.h \
    core/xrb_event.h \
    core/xrb_eventhandler.h \
    core/xrb_eventqueue.h \
    core/xrb_eventqueue.tcpp \
    core/xrb_framehandler.h \
    core/xrb_resourcelibrary.h \
    core/xrb_resourcelibrary.tcpp \
    core/xrb_serializer.h \
    core/xrb_serializer.tcpp \
    core/xrb_signalhandler.h \
    core/xrb_singletons.h \
    core/xrb_statemachine.h \
    core/xrb_statemachine.tcpp \
    core/xrb_types.h \
    \
    core/serializers/xrb_binaryfileserializer.h \
    \
    engine2/xrb_engine2_compound.h \
    engine2/xrb_engine2_entity.h \
    engine2/xrb_engine2_enums.h \
    engine2/xrb_engine2_events.h \
    engine2/xrb_engine2_object.h \
    engine2/xrb_engine2_objectlayer.h \
    engine2/xrb_engine2_physicshandler.h \
    engine2/xrb_engine2_polygon.h \
    engine2/xrb_engine2_quadtree.h \
    engine2/xrb_engine2_sprite.h \
    engine2/xrb_engine2_types.h \
    engine2/xrb_engine2_visibilityquadtree.h \
    engine2/xrb_engine2_world.h \
    \
    gui/xrb_gui_events.h \
    gui/xrb_sizeproperties.h \
    gui/xrb_widget.h \
    gui/xrb_widgetbackground.h \
    gui/xrb_widgetskin.h \
    gui/xrb_widgetskinhandler.h \
    \
    gui/widgets/xrb_button.h \
    gui/widgets/xrb_cellpaddingwidget.h \
    gui/widgets/xrb_checkbox.h \
    gui/widgets/xrb_dialog.h \
    gui/widgets/xrb_filedialog.h \
    gui/widgets/xrb_filepanel.h \
    gui/widgets/xrb_label.h \
    gui/widgets/xrb_layout.h \
    gui/widgets/xrb_lineedit.h \
    gui/widgets/xrb_modalwidget.h \
    gui/widgets/xrb_progressbar.h \
    gui/widgets/xrb_radiobutton.h \
    gui/widgets/xrb_radiobutton.tcpp \
    gui/widgets/xrb_spacerwidget.h \
    gui/widgets/xrb_textwidget.h \
    gui/widgets/xrb_toolbarbutton.h \
    gui/widgets/xrb_toolbarbutton.tcpp \
    gui/widgets/xrb_valueedit.h \
    gui/widgets/xrb_valueedit.tcpp \
    gui/widgets/xrb_valuelabel.h \
    gui/widgets/xrb_valuelabel.tcpp \
    gui/widgets/xrb_widgetstack.h \
    \
    input/xrb_input.h \
    input/xrb_input_events.h \
    input/xrb_key.h \
    input/xrb_keyrepeater.h \
    \
    math/xrb_math.h \
    math/xrb_matrix2.h \
    math/xrb_polynomial.h \
    math/xrb_simpletransform2.h \
    math/xrb_transform2.h \
    math/xrb_vector.h \
    \
    primitive/xrb_bitarray.h \
    primitive/xrb_bitarray.tcpp \
    primitive/xrb_circularqueue.h \
    primitive/xrb_circularqueue.tcpp \
    primitive/xrb_color.h \
    primitive/xrb_gltexture.h \
    primitive/xrb_ntuple.h \
    primitive/xrb_rect.h \
    primitive/xrb_texture.h \
    \
    render/xrb_engine2_worldview.h \
    render/xrb_engine2_worldviewwidget.h \
    render/xrb_font.h \
    render/xrb_fontface.h \
    render/xrb_gl.h \
    render/xrb_render.h \
    render/xrb_rendercontext.h \
    render/xrb_screen.h \
    render/xrb_screencoord.h \
    \
    util/xrb_bitcache.h \
    util/xrb_bitcachedfile.h \
    util/xrb_characterfilter.h \
    util/xrb_frameratecalculator.h \
    util/xrb_indentformatter.h \
    util/xrb_reference.h \
    util/xrb_tokenizer.h \
    util/xrb_transformation.h \
    util/xrb_utf8.h \
    util/xrb_util.h \
    util/xrb_validator.h \
    \
    parsers/xrb_datafile.h \
    parsers/xrb_datafilevalue.h \
    parsers/xrb_linecalculator.h

# the compression stuff is temporarily out of service
#    core/serializers/xrb_compressionserializer.h \
#    util/xrb_bitcachedcompression.h \
#    util/xrb_huffman.h \
#

# the mapeditor is temporarily out of service
#    mapeditor2/xrb_mapeditor2_compound.h \
#    mapeditor2/xrb_mapeditor2_compoundentity.h \
#    mapeditor2/xrb_mapeditor2_compoundvertex.h \
#    mapeditor2/xrb_mapeditor2_compoundvertex.h \
#    mapeditor2/xrb_mapeditor2_entity.h \
#    mapeditor2/xrb_mapeditor2_entitypropertiespanel.h \
#    mapeditor2/xrb_mapeditor2_mainwidget.h \
#    mapeditor2/xrb_mapeditor2_object.h \
#    mapeditor2/xrb_mapeditor2_objectlayer.h \
#    mapeditor2/xrb_mapeditor2_objectpropertiespanel.h \
#    mapeditor2/xrb_mapeditor2_polygon.h \
#    mapeditor2/xrb_mapeditor2_sprite.h \
#    mapeditor2/xrb_mapeditor2_spriteentity.h \
#    mapeditor2/xrb_mapeditor2_visibilityquadtree.h \
#    mapeditor2/xrb_mapeditor2_world.h \
#    mapeditor2/xrb_mapeditor2_worldview.h \
#    \
#

##############################################################################
# parser intermediate libraries -- they're special because they need
# to be separate and have to have special flags for lex and yacc
##############################################################################

libdatafileparser_la_SOURCES = \
    parsers/datafile/xrb_datafileparser.ypp \
    parsers/datafile/xrb_datafilescanner.lpp \
    parsers/xrb_datafile.cpp \
    parsers/xrb_datafilevalue.cpp
libdatafileparser_la_YFLAGS = \
    -d -v -p DataFile_
libdatafileparser_la_LFLAGS = \
    --prefix DataFile_ --outfile=lex.yy.c

liblinecalculatorparser_la_SOURCES = \
    parsers/linecalculator/xrb_linecalculatorparser.ypp \
    parsers/linecalculator/xrb_linecalculatorscanner.lpp \
    parsers/xrb_linecalculator.cpp
liblinecalculatorparser_la_YFLAGS = \
    -d -v -p LineCalculator_
liblinecalculatorparser_la_LFLAGS = \
    --prefix LineCalculator_ --outfile=lex.yy.c

##############################################################################
# general preprocessor and linker flags
##############################################################################

AM_CPPFLAGS = \
    `$(FREETYPE_CONFIG) --cflags` \
    `$(SDL_CONFIG) --cflags` \
    -Wall \
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

# the map editor is temporarily out of service
#    -I$(top_srcdir)/lib/mapeditor2 \
#

AM_LDFLAGS = \
    `$(FREETYPE_CONFIG) --libtool` \
    `$(SDL_CONFIG) --libs`

