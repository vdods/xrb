##############################################################################
# subdir-objects causes object files to be created in the corresponding subdir
# foreign indicates this is not a GNU package so it won't halt if any
#         GNU-required files (e.g. NEWS) are not found
# 1.4 indicates the minimum automake version to proceed
##############################################################################

AUTOMAKE_OPTIONS = subdir-objects foreign 1.4

##############################################################################
# build targets and extra files to add to the distribution
##############################################################################

.PHONY: \
    datafile_parser \
    datafile_scanner \
    doc \
    lib \
    xml_parser \
    xml_scanner

doc:
	$(DOXYGEN) Doxyfile

lib: lib/libxrb.la

lib_LTLIBRARIES = \
    lib/libxrb.la

bin_PROGRAMS = \
    benchmark \
    disasteroids \
    lesson00 \
    lesson01 \
    lesson02 \
    lesson03 \
    lesson04 \
    lesson05 \
    lesson06

EXTRA_DIST = \
    bootstrap.sh \
    configure.ac \
    Doxyfile.in \
    INSTALL \
    LICENSE \
    README \
    TODO \
    xuqrijbuh.kdevelop \
    xuqrijbuh.kdevelop.filelist \
    xuqrijbuh.sln \
    app/example/disasteroids/disasteroids.vcproj \
    app/example/lesson/lesson00/lesson00.vcproj \
    app/example/lesson/lesson01/lesson01.vcproj \
    app/example/lesson/lesson02/lesson02.vcproj \
    app/example/lesson/lesson03/lesson03.vcproj \
    app/example/lesson/lesson04/lesson04.vcproj \
    app/example/lesson/lesson05/lesson05.vcproj \
    app/example/lesson/lesson06/lesson06.vcproj \
    doc/doxygen_pages/acknowledgements_and_credits.dox \
    doc/doxygen_pages/conventions.dox \
    doc/doxygen_pages/goals.dox \
    doc/doxygen_pages/lessons.dox \
    doc/doxygen_pages/main.dox \
    doc/doxygen_pages/math.dox \
    doc/doxygen_pages/portable_cpp_types.dox \
    doc/doxygen_pages/setup_linux.dox \
    doc/doxygen_pages/setup_macosx.dox \
    doc/doxygen_pages/setup_msvc.dox \
    doc/doxygen_pages/widget_baseclass.dox \
    lib/libxrb.vcproj

##############################################################################
# phony target alias for lib/libxrb.la for your convenience
##############################################################################

.PHONY: libxrb

libxrb: lib/libxrb.la

##############################################################################
# general preprocessor and linker flags
##############################################################################

if SDL_CONFIG_EXISTS
SDL_CONFIG_CPPFLAGS = `$(SDL_CONFIG) --cflags`
else
SDL_CONFIG_CPPFLAGS =
endif

if FREETYPE_CONFIG_EXISTS
FREETYPE_CONFIG_CPPFLAGS = `$(FREETYPE_CONFIG) --cflags`
else
FREETYPE_CONFIG_CPPFLAGS =
endif

if LIBPNG_CONFIG_EXISTS
LIBPNG_CONFIG_CPPFLAGS = `$(LIBPNG_CONFIG) --cflags`
else
LIBPNG_CONFIG_CPPFLAGS =
endif

AM_CPPFLAGS = \
    $(SDL_CONFIG_CPPFLAGS) \
    $(FREETYPE_CONFIG_CPPFLAGS) \
    $(LIBPNG_CONFIG_CPPFLAGS) \
    \
    -I$(top_srcdir)/lib/core \
    -I$(top_srcdir)/lib/engine2 \
    -I$(top_srcdir)/lib/engine2/circle \
    -I$(top_srcdir)/lib/gui \
    -I$(top_srcdir)/lib/gui/widgets \
    -I$(top_srcdir)/lib/input \
    -I$(top_srcdir)/lib/lvd \
    -I$(top_srcdir)/lib/math \
    -I$(top_srcdir)/lib/parse \
    -I$(top_srcdir)/lib/primitive \
    -I$(top_srcdir)/lib/render \
    -I$(top_srcdir)/lib/render/fonts \
    -I$(top_srcdir)/lib/sound \
    -I$(top_srcdir)/lib/system \
    -I$(top_srcdir)/lib/system/pals \
    -I$(top_srcdir)/lib/system/serializers \
    -I$(top_srcdir)/lib/util

if SDL_CONFIG_EXISTS
SDL_CONFIG_LDFLAGS = `$(SDL_CONFIG) --libs`
else
SDL_CONFIG_LDFLAGS =
endif

if FREETYPE_CONFIG_EXISTS
FREETYPE_CONFIG_LDFLAGS = `$(FREETYPE_CONFIG) --libtool`
else
FREETYPE_CONFIG_LDFLAGS =
endif

if LIBPNG_CONFIG_EXISTS
LIBPNG_CONFIG_LDFLAGS = `$(LIBPNG_CONFIG) --libs`
else
LIBPNG_CONFIG_LDFLAGS =
endif

AM_LDFLAGS = \
    $(SDL_CONFIG_LDFLAGS) \
    $(FREETYPE_CONFIG_LDFLAGS) \
    $(LIBPNG_CONFIG_LDFLAGS)

LDADD = $(LIBOBJS) $(top_builddir)/lib/libxrb.la

##############################################################################
# libxrb source and header files
##############################################################################

lib_libxrb_la_SOURCES = \
    lib/core/xrb_compiletimeasserts.cpp \
    \
    lib/engine2/xrb_engine2_animatedsprite.cpp \
    lib/engine2/xrb_engine2_compound.cpp \
    lib/engine2/xrb_engine2_entity.cpp \
    lib/engine2/xrb_engine2_events.cpp \
    lib/engine2/xrb_engine2_object.cpp \
    lib/engine2/xrb_engine2_objectlayer.cpp \
    lib/engine2/xrb_engine2_polygon.cpp \
    lib/engine2/xrb_engine2_quadtree.cpp \
    lib/engine2/xrb_engine2_sprite.cpp \
    lib/engine2/xrb_engine2_svgworldloader.cpp \
    lib/engine2/xrb_engine2_types.cpp \
    lib/engine2/xrb_engine2_visibilityquadtree.cpp \
    lib/engine2/xrb_engine2_world.cpp \
    lib/engine2/xrb_engine2_worldview.cpp \
    lib/engine2/xrb_engine2_worldviewwidget.cpp \
    \
    lib/engine2/circle/xrb_engine2_circle_collisionquadtree.cpp \
    lib/engine2/circle/xrb_engine2_circle_entity.cpp \
    lib/engine2/circle/xrb_engine2_circle_physicshandler.cpp \
    \
    lib/gui/xrb_containerwidget.cpp \
    lib/gui/xrb_gui_events.cpp \
    lib/gui/xrb_screen.cpp \
    lib/gui/xrb_screencoord.cpp \
    lib/gui/xrb_sizeproperties.cpp \
    lib/gui/xrb_widget.cpp \
    lib/gui/xrb_widgetbackground.cpp \
    lib/gui/xrb_widgetcontext.cpp \
    lib/gui/xrb_widgetskin.cpp \
    \
    lib/gui/widgets/xrb_button.cpp \
    lib/gui/widgets/xrb_cellpaddingwidget.cpp \
    lib/gui/widgets/xrb_checkbox.cpp \
    lib/gui/widgets/xrb_dialog.cpp \
    lib/gui/widgets/xrb_filedialog.cpp \
    lib/gui/widgets/xrb_filepanel.cpp \
    lib/gui/widgets/xrb_keyselectorbutton.cpp \
    lib/gui/widgets/xrb_label.cpp \
    lib/gui/widgets/xrb_layout.cpp \
    lib/gui/widgets/xrb_lineedit.cpp \
    lib/gui/widgets/xrb_modalwidget.cpp \
    lib/gui/widgets/xrb_progressbar.cpp \
    lib/gui/widgets/xrb_spacerwidget.cpp \
    lib/gui/widgets/xrb_textwidget.cpp \
    lib/gui/widgets/xrb_widgetstack.cpp \
    \
    lib/input/xrb_inputstate.cpp \
    lib/input/xrb_input_events.cpp \
    lib/input/xrb_key.cpp \
    lib/input/xrb_keymap.cpp \
    lib/input/xrb_keyrepeater.cpp \
    \
    lib/lvd/lvd_filoc.cpp \
    lib/lvd/lvd_xml.cpp \
    lib/lvd/lvd_xml_parser.cpp \
    lib/lvd/lvd_xml_parser.trison \
    lib/lvd/lvd_xml_scanner.cpp \
    lib/lvd/lvd_xml_scanner.reflex \
    \
    lib/math/xrb_math.cpp \
    lib/math/xrb_matrix2.cpp \
    lib/math/xrb_polynomial.cpp \
    lib/math/xrb_simpletransform2.cpp \
    lib/math/xrb_transform2.cpp \
    lib/math/xrb_vector.cpp \
    \
    lib/parse/xrb_parse_datafile.cpp \
    lib/parse/xrb_parse_datafile_parser.cpp \
    lib/parse/xrb_parse_datafile_parser.trison \
    lib/parse/xrb_parse_datafile_scanner.cpp \
    lib/parse/xrb_parse_datafile_scanner.reflex \
    lib/parse/xrb_parse_datafile_value.cpp \
    lib/parse/xrb_parse_filoc.cpp \
    lib/parse/xrb_parse_util.cpp \
    \
    lib/primitive/xrb_color.cpp \
    lib/primitive/xrb_ntuple.cpp \
    lib/primitive/xrb_rect.cpp \
    lib/primitive/xrb_texture.cpp \
    \
    lib/render/xrb_animation.cpp \
    lib/render/xrb_font.cpp \
    lib/render/xrb_gl.cpp \
    lib/render/xrb_gltexture.cpp \
    lib/render/xrb_gltextureatlas.cpp \
    lib/render/xrb_render.cpp \
    lib/render/xrb_rendercontext.cpp \
    \
    lib/render/fonts/xrb_asciifont.cpp \
    \
    lib/system/xrb_event.cpp \
    lib/system/xrb_eventhandler.cpp \
    lib/system/xrb_eventqueue.cpp \
    lib/system/xrb_framehandler.cpp \
    lib/system/xrb_resourcelibrary.cpp \
    lib/system/xrb_signalhandler.cpp \
    lib/system/xrb_singleton.cpp \
    \
    lib/system/pals/xrb_sdlpal.cpp \
    \
    lib/system/serializers/xrb_binaryfileserializer.cpp \
    \
    lib/util/xrb_characterfilter.cpp \
    lib/util/xrb_commandlineparser.cpp \
    lib/util/xrb_emptystring.cpp \
    lib/util/xrb_frameratecalculator.cpp \
    lib/util/xrb_indentformatter.cpp \
    lib/util/xrb_tokenizer.cpp \
    lib/util/xrb_transformation.cpp \
    lib/util/xrb_utf8.cpp \
    lib/util/xrb_util.cpp

##############################################################################
# these headers will be installed in $(prefix)/include/xrb/
##############################################################################

include_HEADERS = \
    lib/core/xrb.hpp \
    lib/core/xrb_assert.hpp \
    lib/core/xrb_debug.hpp \
    lib/core/xrb_delete.hpp \
    lib/core/xrb_types.hpp \
    \
    lib/engine2/xrb_engine2_animatedsprite.hpp \
    lib/engine2/xrb_engine2_compound.hpp \
    lib/engine2/xrb_engine2_entity.hpp \
    lib/engine2/xrb_engine2_enums.hpp \
    lib/engine2/xrb_engine2_events.hpp \
    lib/engine2/xrb_engine2_object.hpp \
    lib/engine2/xrb_engine2_objectlayer.hpp \
    lib/engine2/xrb_engine2_physicshandler.hpp \
    lib/engine2/xrb_engine2_polygon.hpp \
    lib/engine2/xrb_engine2_quadtree.hpp \
    lib/engine2/xrb_engine2_sprite.hpp \
    lib/engine2/xrb_engine2_svgworldloader.hpp \
    lib/engine2/xrb_engine2_types.hpp \
    lib/engine2/xrb_engine2_visibilityquadtree.hpp \
    lib/engine2/xrb_engine2_world.hpp \
    lib/engine2/xrb_engine2_worldview.hpp \
    lib/engine2/xrb_engine2_worldviewwidget.hpp \
    \
    lib/engine2/circle/xrb_engine2_circle_collisionquadtree.hpp \
    lib/engine2/circle/xrb_engine2_circle_entity.hpp \
    lib/engine2/circle/xrb_engine2_circle_physicshandler.hpp \
    lib/engine2/circle/xrb_engine2_circle_types.hpp \
    \
    lib/gui/xrb_containerwidget.hpp \
    lib/gui/xrb_gui_events.hpp \
    lib/gui/xrb_margins.hpp \
    lib/gui/xrb_screen.hpp \
    lib/gui/xrb_screencoord.hpp \
    lib/gui/xrb_sizeproperties.hpp \
    lib/gui/xrb_widget.hpp \
    lib/gui/xrb_widgetbackground.hpp \
    lib/gui/xrb_widgetcontext.hpp \
    lib/gui/xrb_widgetskin.hpp \
    \
    lib/gui/widgets/xrb_button.hpp \
    lib/gui/widgets/xrb_cellpaddingwidget.hpp \
    lib/gui/widgets/xrb_checkbox.hpp \
    lib/gui/widgets/xrb_dialog.hpp \
    lib/gui/widgets/xrb_filedialog.hpp \
    lib/gui/widgets/xrb_filepanel.hpp \
    lib/gui/widgets/xrb_keyselectorbutton.hpp \
    lib/gui/widgets/xrb_label.hpp \
    lib/gui/widgets/xrb_layout.hpp \
    lib/gui/widgets/xrb_lineedit.hpp \
    lib/gui/widgets/xrb_modalwidget.hpp \
    lib/gui/widgets/xrb_progressbar.hpp \
    lib/gui/widgets/xrb_radiobutton.hpp \
    lib/gui/widgets/xrb_spacerwidget.hpp \
    lib/gui/widgets/xrb_textwidget.hpp \
    lib/gui/widgets/xrb_toolbarbutton.hpp \
    lib/gui/widgets/xrb_valueedit.hpp \
    lib/gui/widgets/xrb_valuelabel.hpp \
    lib/gui/widgets/xrb_widgetstack.hpp \
    \
    lib/input/xrb_inputstate.hpp \
    lib/input/xrb_input_events.hpp \
    lib/input/xrb_key.hpp \
    lib/input/xrb_keymap.hpp \
    lib/input/xrb_keyrepeater.hpp \
    \
    lib/lvd/lvd_filoc.hpp \
    lib/lvd/lvd_xml.hpp \
    lib/lvd/lvd_xml_parser.hpp \
    lib/lvd/lvd_xml_scanner.hpp \
    \
    lib/math/xrb_math.hpp \
    lib/math/xrb_matrix2.hpp \
    lib/math/xrb_polynomial.hpp \
    lib/math/xrb_simpletransform2.hpp \
    lib/math/xrb_transform2.hpp \
    lib/math/xrb_vector.hpp \
    \
    lib/parse/xrb_parse_datafile.hpp \
    lib/parse/xrb_parse_datafile_parser.hpp \
    lib/parse/xrb_parse_datafile_scanner.hpp \
    lib/parse/xrb_parse_datafile_value.hpp \
    lib/parse/xrb_parse_filoc.hpp \
    lib/parse/xrb_parse_util.hpp \
    \
    lib/primitive/xrb_circularqueue.hpp \
    lib/primitive/xrb_color.hpp \
    lib/primitive/xrb_exception.hpp \
    lib/primitive/xrb_ntuple.hpp \
    lib/primitive/xrb_rect.hpp \
    lib/primitive/xrb_texture.hpp \
    \
    lib/render/xrb_animation.hpp \
    lib/render/xrb_font.hpp \
    lib/render/xrb_gl.hpp \
    lib/render/xrb_gltexture.hpp \
    lib/render/xrb_gltextureatlas.hpp \
    lib/render/xrb_render.hpp \
    lib/render/xrb_rendercontext.hpp \
    \
    lib/render/fonts/xrb_asciifont.hpp \
    \
    lib/system/xrb_enums.hpp \
    lib/system/xrb_event.hpp \
    lib/system/xrb_eventhandler.hpp \
    lib/system/xrb_eventqueue.hpp \
    lib/system/xrb_framehandler.hpp \
    lib/system/xrb_pal.hpp \
    lib/system/xrb_resourcelibrary.hpp \
    lib/system/xrb_resourceloadparameters.hpp \
    lib/system/xrb_serializer.hpp \
    lib/system/xrb_signalhandler.hpp \
    lib/system/xrb_singleton.hpp \
    lib/system/xrb_statemachine.hpp \
    \
    lib/system/pals/xrb_sdlpal.hpp \
    \
    lib/system/serializers/xrb_binaryfileserializer.hpp \
    \
    lib/util/xrb_characterfilter.hpp \
    lib/util/xrb_commandlineparser.hpp \
    lib/util/xrb_emptystring.hpp \
    lib/util/xrb_endian.hpp \
    lib/util/xrb_frameratecalculator.hpp \
    lib/util/xrb_indentformatter.hpp \
    lib/util/xrb_reference.hpp \
    lib/util/xrb_tokenizer.hpp \
    lib/util/xrb_transformation.hpp \
    lib/util/xrb_utf8.hpp \
    lib/util/xrb_util.hpp \
    lib/util/xrb_validator.hpp

##############################################################################
# custom make rules
##############################################################################

if TRISON_EXISTS
datafile_parser: $(top_srcdir)/lib/parse/xrb_parse_datafile_parser.hpp
$(top_srcdir)/lib/parse/xrb_parse_datafile_parser.hpp: $(top_srcdir)/lib/parse/xrb_parse_datafile_parser.cpp
$(top_srcdir)/lib/parse/xrb_parse_datafile_parser.cpp: $(top_srcdir)/lib/parse/xrb_parse_datafile_parser.trison
	$(TRISON) $(top_srcdir)/lib/parse/xrb_parse_datafile_parser.trison --warnings-as-errors --output-directory $(top_srcdir)/lib/parse --generate-npda-dot-graph xrb_parse_datafile_parser.npda.dot --generate-dpda-dot-graph xrb_parse_datafile_parser.dpda.dot --generate-dpda-states-file xrb_parse_datafile_parser.dpda.states

xml_parser: $(top_srcdir)/lib/lvd/lvd_xml_parser.hpp
$(top_srcdir)/lib/lvd/lvd_xml_parser.hpp: $(top_srcdir)/lib/lvd/lvd_xml_parser.cpp
$(top_srcdir)/lib/lvd/lvd_xml_parser.cpp: $(top_srcdir)/lib/lvd/lvd_xml_parser.trison
	$(TRISON) $(top_srcdir)/lib/lvd/lvd_xml_parser.trison --warnings-as-errors --output-directory $(top_srcdir)/lib/lvd --generate-npda-dot-graph lvd_xml_parser.npda.dot --generate-dpda-dot-graph lvd_xml_parser.dpda.dot --generate-dpda-states-file lvd_xml_parser.dpda.states
endif

if REFLEX_EXISTS
datafile_scanner: $(top_srcdir)/lib/parse/xrb_parse_datafile_scanner.hpp
$(top_srcdir)/lib/parse/xrb_parse_datafile_scanner.hpp: $(top_srcdir)/lib/parse/xrb_parse_datafile_scanner.cpp
$(top_srcdir)/lib/parse/xrb_parse_datafile_scanner.cpp: $(top_srcdir)/lib/parse/xrb_parse_datafile_scanner.reflex
	$(REFLEX) $(top_srcdir)/lib/parse/xrb_parse_datafile_scanner.reflex --warnings-as-errors --output-directory $(top_srcdir)/lib/parse --generate-nfa-dot-graph xrb_parse_datafile_scanner.nfa.dot --generate-dfa-dot-graph xrb_parse_datafile_scanner.dfa.dot

xml_scanner: $(top_srcdir)/lib/lvd/lvd_xml_scanner.hpp
$(top_srcdir)/lib/lvd/lvd_xml_scanner.hpp: $(top_srcdir)/lib/lvd/lvd_xml_scanner.cpp
$(top_srcdir)/lib/lvd/lvd_xml_scanner.cpp: $(top_srcdir)/lib/lvd/lvd_xml_scanner.reflex
	$(REFLEX) $(top_srcdir)/lib/lvd/lvd_xml_scanner.reflex --warnings-as-errors --output-directory $(top_srcdir)/lib/lvd --generate-nfa-dot-graph lvd_xml_scanner.nfa.dot --generate-dfa-dot-graph lvd_xml_scanner.dfa.dot
endif

##############################################################################
# benchmark
##############################################################################

benchmark_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    \
    -I$(top_srcdir)/app/benchmark

benchmark_SOURCES = \
    app/benchmark/bm_commandlineoptions.cpp \
    app/benchmark/bm_config.cpp \
    app/benchmark/bm_main.cpp \
    app/benchmark/bm_master.cpp

##############################################################################
# disasteroids
##############################################################################

disasteroids_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    \
    -I$(top_srcdir)/app/example/disasteroids \
    -I$(top_srcdir)/app/example/disasteroids/game \
    -I$(top_srcdir)/app/example/disasteroids/game/engine2 \
    -I$(top_srcdir)/app/example/disasteroids/game/entities \
    -I$(top_srcdir)/app/example/disasteroids/game/entities/ships \
    -I$(top_srcdir)/app/example/disasteroids/game/items \
    -I$(top_srcdir)/app/example/disasteroids/ui

disasteroids_SOURCES = \
    app/example/disasteroids/dis_commandlineoptions.cpp \
    app/example/disasteroids/dis_config.cpp \
    app/example/disasteroids/dis_enums.cpp \
    app/example/disasteroids/dis_main.cpp \
    app/example/disasteroids/dis_master.cpp \
    app/example/disasteroids/dis_resourcecache.cpp \
    app/example/disasteroids/game/dis_highscores.cpp \
    app/example/disasteroids/game/dis_item.cpp \
    app/example/disasteroids/game/dis_spawn.cpp \
    app/example/disasteroids/game/dis_util.cpp \
    app/example/disasteroids/game/engine2/dis_entity.cpp \
    app/example/disasteroids/game/engine2/dis_physicshandler.cpp \
    app/example/disasteroids/game/engine2/dis_world.cpp \
    app/example/disasteroids/game/entities/dis_asteroid.cpp \
    app/example/disasteroids/game/entities/dis_ballistic.cpp \
    app/example/disasteroids/game/entities/dis_effect.cpp \
    app/example/disasteroids/game/entities/dis_explosive.cpp \
    app/example/disasteroids/game/entities/dis_healthtrigger.cpp \
    app/example/disasteroids/game/entities/dis_mortal.cpp \
    app/example/disasteroids/game/entities/dis_powerup.cpp \
    app/example/disasteroids/game/entities/dis_ship.cpp \
    app/example/disasteroids/game/entities/ships/dis_demi.cpp \
    app/example/disasteroids/game/entities/ships/dis_devourment.cpp \
    app/example/disasteroids/game/entities/ships/dis_enemyship.cpp \
    app/example/disasteroids/game/entities/ships/dis_interloper.cpp \
    app/example/disasteroids/game/entities/ships/dis_playership.cpp \
    app/example/disasteroids/game/entities/ships/dis_revulsion.cpp \
    app/example/disasteroids/game/entities/ships/dis_shade.cpp \
    app/example/disasteroids/game/entities/ships/dis_solitary.cpp \
    app/example/disasteroids/game/items/dis_armor.cpp \
    app/example/disasteroids/game/items/dis_engine.cpp \
    app/example/disasteroids/game/items/dis_powergenerator.cpp \
    app/example/disasteroids/game/items/dis_shield.cpp \
    app/example/disasteroids/game/items/dis_weapon.cpp \
    app/example/disasteroids/ui/dis_controlspanel.cpp \
    app/example/disasteroids/ui/dis_gamewidget.cpp \
    app/example/disasteroids/ui/dis_highscorenameentrydialog.cpp \
    app/example/disasteroids/ui/dis_highscoreswidget.cpp \
    app/example/disasteroids/ui/dis_inventorybutton.cpp \
    app/example/disasteroids/ui/dis_inventorypanel.cpp \
    app/example/disasteroids/ui/dis_titlescreenwidget.cpp \
    app/example/disasteroids/ui/dis_worldview.cpp

##############################################################################
# lessons
##############################################################################

lesson00_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/app/example/lesson/lesson00
lesson00_SOURCES = app/example/lesson/lesson00/lesson00_main.cpp

lesson01_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/app/example/lesson/lesson01
lesson01_SOURCES = app/example/lesson/lesson01/lesson01_main.cpp

lesson02_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/app/example/lesson/lesson02
lesson02_SOURCES = app/example/lesson/lesson02/lesson02_main.cpp

lesson03_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/app/example/lesson/lesson03
lesson03_SOURCES = app/example/lesson/lesson03/lesson03_main.cpp

lesson04_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/app/example/lesson/lesson04
lesson04_SOURCES = app/example/lesson/lesson04/lesson04_main.cpp

lesson05_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/app/example/lesson/lesson05
lesson05_SOURCES = app/example/lesson/lesson05/lesson05_main.cpp

lesson06_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/app/example/lesson/lesson06
lesson06_SOURCES = app/example/lesson/lesson06/lesson06_main.cpp

##############################################################################
# all the no-install header files for the examples
##############################################################################

noinst_HEADERS = \
    app/benchmark/bm_commandlineoptions.hpp \
    app/benchmark/bm_config.hpp \
    app/benchmark/bm_enums.hpp \
    app/benchmark/bm_master.hpp \
    \
    app/example/disasteroids/dis_commandlineoptions.hpp \
    app/example/disasteroids/dis_config.hpp \
    app/example/disasteroids/dis_enums.hpp \
    app/example/disasteroids/dis_events.hpp \
    app/example/disasteroids/dis_master.hpp \
    app/example/disasteroids/dis_resourcecache.hpp \
    app/example/disasteroids/game/dis_highscores.hpp \
    app/example/disasteroids/game/dis_item.hpp \
    app/example/disasteroids/game/dis_spawn.hpp \
    app/example/disasteroids/game/dis_util.hpp \
    app/example/disasteroids/game/engine2/dis_entity.hpp \
    app/example/disasteroids/game/engine2/dis_entityreference.hpp \
    app/example/disasteroids/game/engine2/dis_physicshandler.hpp \
    app/example/disasteroids/game/engine2/dis_world.hpp \
    app/example/disasteroids/game/entities/dis_asteroid.hpp \
    app/example/disasteroids/game/entities/dis_ballistic.hpp \
    app/example/disasteroids/game/entities/dis_effect.hpp \
    app/example/disasteroids/game/entities/dis_explosive.hpp \
    app/example/disasteroids/game/entities/dis_healthtrigger.hpp \
    app/example/disasteroids/game/entities/dis_mortal.hpp \
    app/example/disasteroids/game/entities/dis_powerup.hpp \
    app/example/disasteroids/game/entities/dis_ship.hpp \
    app/example/disasteroids/game/entities/ships/dis_demi.hpp \
    app/example/disasteroids/game/entities/ships/dis_devourment.hpp \
    app/example/disasteroids/game/entities/ships/dis_enemyship.hpp \
    app/example/disasteroids/game/entities/ships/dis_interloper.hpp \
    app/example/disasteroids/game/entities/ships/dis_playership.hpp \
    app/example/disasteroids/game/entities/ships/dis_revulsion.hpp \
    app/example/disasteroids/game/entities/ships/dis_shade.hpp \
    app/example/disasteroids/game/entities/ships/dis_solitary.hpp \
    app/example/disasteroids/game/items/dis_armor.hpp \
    app/example/disasteroids/game/items/dis_engine.hpp \
    app/example/disasteroids/game/items/dis_powereddevice.hpp \
    app/example/disasteroids/game/items/dis_powergenerator.hpp \
    app/example/disasteroids/game/items/dis_shield.hpp \
    app/example/disasteroids/game/items/dis_weapon.hpp \
    app/example/disasteroids/ui/dis_controlspanel.hpp \
    app/example/disasteroids/ui/dis_gamewidget.hpp \
    app/example/disasteroids/ui/dis_highscorenameentrydialog.hpp \
    app/example/disasteroids/ui/dis_highscoreswidget.hpp \
    app/example/disasteroids/ui/dis_inventorybutton.hpp \
    app/example/disasteroids/ui/dis_inventorypanel.hpp \
    app/example/disasteroids/ui/dis_titlescreenwidget.hpp \
    app/example/disasteroids/ui/dis_worldview.hpp
