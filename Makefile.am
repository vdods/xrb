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

lib_LTLIBRARIES = \
	lib/libxrb.la

bin_PROGRAMS = \
    disasteroids \
    guidemo \
    lesson00 \
    lesson01 \
    lesson02 \
    lesson03 \
    lesson04 \
    lesson05 \
    lesson06
#    mapeditor2 \ # is temporarily out of commission

EXTRA_DIST = \
	configure.ac \
	Doxyfile \
	INSTALL \
    LICENSE \
    README \
    TODO \
    bootstrap.sh

##############################################################################
# phony target alias for lib/libxrb.la for your convenience
##############################################################################

.PHONY: libxrb

libxrb: lib/libxrb.la

##############################################################################
# general preprocessor and linker flags
##############################################################################

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
    -I$(top_srcdir)/lib/math \
    -I$(top_srcdir)/lib/parsers \
    -I$(top_srcdir)/lib/primitive \
    -I$(top_srcdir)/lib/render \
    -I$(top_srcdir)/lib/sound \
    -I$(top_srcdir)/lib/system \
    -I$(top_srcdir)/lib/util

#    `$(LIBPNG_CONFIG) --cflags` \
#

# the map editor is temporarily out of service
#    -I$(top_srcdir)/lib/mapeditor2 \
#

AM_LDFLAGS = \
    `$(FREETYPE_CONFIG) --libtool` \
    `$(SDL_CONFIG) --libs`

#    `$(LIBPNG_CONFIG) --libs`

LDADD = $(LIBOBJS) $(top_builddir)/lib/libxrb.la

##############################################################################
# libxrb source and header files
##############################################################################

lib_libxrb_la_SOURCES = \
    lib/core/xrb_compiletimeasserts.cpp \
    \
    lib/engine2/xrb_engine2_compound.cpp \
    lib/engine2/xrb_engine2_entity.cpp \
    lib/engine2/xrb_engine2_events.cpp \
    lib/engine2/xrb_engine2_object.cpp \
    lib/engine2/xrb_engine2_objectlayer.cpp \
    lib/engine2/xrb_engine2_polygon.cpp \
    lib/engine2/xrb_engine2_quadtree.cpp \
    lib/engine2/xrb_engine2_sprite.cpp \
    lib/engine2/xrb_engine2_visibilityquadtree.cpp \
    lib/engine2/xrb_engine2_world.cpp \
    \
    lib/gui/xrb_containerwidget.cpp \
    lib/gui/xrb_gui_events.cpp \
    lib/gui/xrb_screen.cpp \
    lib/gui/xrb_screencoord.cpp \
    lib/gui/xrb_sizeproperties.cpp \
    lib/gui/xrb_widget.cpp \
    lib/gui/xrb_widgetbackground.cpp \
    lib/gui/xrb_widgetskin.cpp \
    lib/gui/xrb_widgetskinhandler.cpp \
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
    lib/math/xrb_math.cpp \
    lib/math/xrb_matrix2.cpp \
    lib/math/xrb_polynomial.cpp \
    lib/math/xrb_simpletransform2.cpp \
    lib/math/xrb_transform2.cpp \
    lib/math/xrb_vector.cpp \
    \
    lib/parsers/xrb_datafilelocation.cpp \
    lib/parsers/xrb_datafileparser.cpp \
    lib/parsers/xrb_datafileparser.trison \
    lib/parsers/xrb_datafilescanner.cpp \
    lib/parsers/xrb_datafilevalue.cpp \
    lib/parsers/xrb_arithmeticparser.cpp \
    lib/parsers/xrb_arithmeticparser.trison \
    lib/parsers/xrb_arithmeticscanner.cpp \
    \
    lib/primitive/xrb_color.cpp \
    lib/primitive/xrb_ntuple.cpp \
    lib/primitive/xrb_rect.cpp \
    lib/primitive/xrb_texture.cpp \
    \
    lib/render/xrb_engine2_worldview.cpp \
    lib/render/xrb_engine2_worldviewwidget.cpp \
    lib/render/xrb_font.cpp \
    lib/render/xrb_fontface.cpp \
    lib/render/xrb_gl.cpp \
    lib/render/xrb_gltexture.cpp \
    lib/render/xrb_render.cpp \
    lib/render/xrb_rendercontext.cpp \
    \
    lib/system/xrb_event.cpp \
    lib/system/xrb_eventhandler.cpp \
    lib/system/xrb_eventqueue.cpp \
    lib/system/xrb_framehandler.cpp \
    lib/system/xrb_resourcelibrary.cpp \
    lib/system/xrb_serializer.cpp \
    lib/system/xrb_signalhandler.cpp \
    lib/system/xrb_singleton.cpp \
    \
    lib/system/serializers/xrb_binaryfileserializer.cpp \
    \
    lib/util/xrb_bitcache.cpp \
    lib/util/xrb_bitcachedfile.cpp \
    lib/util/xrb_characterfilter.cpp \
    lib/util/xrb_commandlineparser.cpp \
    lib/util/xrb_emptystring.cpp \
    lib/util/xrb_frameratecalculator.cpp \
    lib/util/xrb_indentformatter.cpp \
    lib/util/xrb_tokenizer.cpp \
    lib/util/xrb_transformation.cpp \
    lib/util/xrb_utf8.cpp \
    lib/util/xrb_util.cpp

# the mapeditor is temporarily out of service
#    lib/mapeditor2/xrb_mapeditor2_compound.cpp \
#    lib/mapeditor2/xrb_mapeditor2_compoundentity.cpp \
#    lib/mapeditor2/xrb_mapeditor2_compoundvertex.cpp \
#    lib/mapeditor2/xrb_mapeditor2_entity.cpp \
#    lib/mapeditor2/xrb_mapeditor2_entitypropertiespanel.cpp \
#    lib/mapeditor2/xrb_mapeditor2_mainwidget.cpp \
#    lib/mapeditor2/xrb_mapeditor2_object.cpp \
#    lib/mapeditor2/xrb_mapeditor2_objectlayer.cpp \
#    lib/mapeditor2/xrb_mapeditor2_objectpropertiespanel.cpp \
#    lib/mapeditor2/xrb_mapeditor2_polygon.cpp \
#    lib/mapeditor2/xrb_mapeditor2_sprite.cpp \
#    lib/mapeditor2/xrb_mapeditor2_spriteentity.cpp \
#    lib/mapeditor2/xrb_mapeditor2_visibilityquadtree.cpp \
#    lib/mapeditor2/xrb_mapeditor2_world.cpp \
#    lib/mapeditor2/xrb_mapeditor2_worldview.cpp \
#    \
#

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
    lib/engine2/xrb_engine2_types.hpp \
    lib/engine2/xrb_engine2_visibilityquadtree.hpp \
    lib/engine2/xrb_engine2_world.hpp \
    \
    lib/gui/xrb_containerwidget.hpp \
    lib/gui/xrb_gui_events.hpp \
    lib/gui/xrb_screen.hpp \
    lib/gui/xrb_screencoord.hpp \
    lib/gui/xrb_sizeproperties.hpp \
    lib/gui/xrb_widget.hpp \
    lib/gui/xrb_widgetbackground.hpp \
    lib/gui/xrb_widgetskin.hpp \
    lib/gui/xrb_widgetskinhandler.hpp \
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
    lib/math/xrb_math.hpp \
    lib/math/xrb_matrix2.hpp \
    lib/math/xrb_polynomial.hpp \
    lib/math/xrb_simpletransform2.hpp \
    lib/math/xrb_transform2.hpp \
    lib/math/xrb_vector.hpp \
    \
    lib/parsers/xrb_datafilelocation.hpp \
    lib/parsers/xrb_datafileparser.h \
    lib/parsers/xrb_datafilescanner.hpp \
    lib/parsers/xrb_datafilevalue.hpp \
    lib/parsers/xrb_arithmeticparser.h \
    lib/parsers/xrb_arithmeticscanner.hpp \
    \
    lib/primitive/xrb_bitarray.hpp \
    lib/primitive/xrb_circularqueue.hpp \
    lib/primitive/xrb_color.hpp \
    lib/primitive/xrb_ntuple.hpp \
    lib/primitive/xrb_rect.hpp \
    lib/primitive/xrb_texture.hpp \
    \
    lib/render/xrb_engine2_worldview.hpp \
    lib/render/xrb_engine2_worldviewwidget.hpp \
    lib/render/xrb_font.hpp \
    lib/render/xrb_fontface.hpp \
    lib/render/xrb_gl.hpp \
    lib/render/xrb_gltexture.hpp \
    lib/render/xrb_render.hpp \
    lib/render/xrb_rendercontext.hpp \
    \
    lib/system/xrb_enums.hpp \
    lib/system/xrb_event.hpp \
    lib/system/xrb_eventhandler.hpp \
    lib/system/xrb_eventqueue.hpp \
    lib/system/xrb_framehandler.hpp \
    lib/system/xrb_resourcelibrary.hpp \
    lib/system/xrb_serializer.hpp \
    lib/system/xrb_signalhandler.hpp \
    lib/system/xrb_singleton.hpp \
    lib/system/xrb_statemachine.hpp \
    \
    lib/system/serializers/xrb_binaryfileserializer.hpp \
    \
    lib/util/xrb_bitcache.hpp \
    lib/util/xrb_bitcachedfile.hpp \
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

# the mapeditor is temporarily out of service
#    lib/mapeditor2/xrb_mapeditor2_compound.hpp \
#    lib/mapeditor2/xrb_mapeditor2_compoundentity.hpp \
#    lib/mapeditor2/xrb_mapeditor2_compoundvertex.hpp \
#    lib/mapeditor2/xrb_mapeditor2_compoundvertex.hpp \
#    lib/mapeditor2/xrb_mapeditor2_entity.hpp \
#    lib/mapeditor2/xrb_mapeditor2_entitypropertiespanel.hpp \
#    lib/mapeditor2/xrb_mapeditor2_mainwidget.hpp \
#    lib/mapeditor2/xrb_mapeditor2_object.hpp \
#    lib/mapeditor2/xrb_mapeditor2_objectlayer.hpp \
#    lib/mapeditor2/xrb_mapeditor2_objectpropertiespanel.hpp \
#    lib/mapeditor2/xrb_mapeditor2_polygon.hpp \
#    lib/mapeditor2/xrb_mapeditor2_sprite.hpp \
#    lib/mapeditor2/xrb_mapeditor2_spriteentity.hpp \
#    lib/mapeditor2/xrb_mapeditor2_visibilityquadtree.hpp \
#    lib/mapeditor2/xrb_mapeditor2_world.hpp \
#    lib/mapeditor2/xrb_mapeditor2_worldview.hpp \
#    \
#

##############################################################################
# custom make rules
##############################################################################

if TRISON_EXISTS
$(srcdir)/lib/parsers/xrb_datafileparser.h: $(srcdir)/lib/parsers/xrb_datafileparser.cpp
$(srcdir)/lib/parsers/xrb_datafileparser.cpp: $(srcdir)/lib/parsers/xrb_datafileparser.trison
	$(TRISON) $(srcdir)/lib/parsers/xrb_datafileparser.trison --warnings-as-errors --output-basename $(srcdir)/lib/parsers/xrb_datafileparser --generate-states-file $(srcdir)/lib/parsers/xrb_datafileparser.states

$(srcdir)/lib/parsers/xrb_arithmeticparser.h: $(srcdir)/lib/parsers/xrb_arithmeticparser.cpp
$(srcdir)/lib/parsers/xrb_arithmeticparser.cpp: $(srcdir)/lib/parsers/xrb_arithmeticparser.trison
	$(TRISON) $(srcdir)/lib/parsers/xrb_arithmeticparser.trison --warnings-as-errors --output-basename $(srcdir)/lib/parsers/xrb_arithmeticparser --generate-states-file $(srcdir)/lib/parsers/xrb_arithmeticparser.states
endif

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
    app/example/disasteroids/game/engine2/dis_collisionquadtree.cpp \
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
    app/example/disasteroids/ui/dis_gamewidget.cpp \
    app/example/disasteroids/ui/dis_highscorenameentrydialog.cpp \
    app/example/disasteroids/ui/dis_highscoreswidget.cpp \
    app/example/disasteroids/ui/dis_inventorybutton.cpp \
    app/example/disasteroids/ui/dis_inventorypanel.cpp \
    app/example/disasteroids/ui/dis_optionspanel.cpp \
    app/example/disasteroids/ui/dis_titlescreenwidget.cpp \
    app/example/disasteroids/ui/dis_worldview.cpp

##############################################################################
# guidemo
##############################################################################

guidemo_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	\
    -I$(top_srcdir)/app/example/guidemo

guidemo_SOURCES = \
    app/example/guidemo/gui_main.cpp \
    app/example/guidemo/gui_masterwidget.cpp

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
# mapeditor2
##############################################################################

#mapeditor2_CPPFLAGS = \
#	$(AM_CPPFLAGS) \
#	\
#	-I$(top_srcdir)/app/mapeditor2
#
#mapeditor2_SOURCES = \
#    app/mapeditor2/main.cpp

##############################################################################
# all the no-install header files for the examples
##############################################################################

noinst_HEADERS = \
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
    app/example/disasteroids/game/engine2/dis_areatracelist.hpp \
    app/example/disasteroids/game/engine2/dis_collisionpair.hpp \
    app/example/disasteroids/game/engine2/dis_collisionquadtree.hpp \
    app/example/disasteroids/game/engine2/dis_entity.hpp \
    app/example/disasteroids/game/engine2/dis_entityreference.hpp \
    app/example/disasteroids/game/engine2/dis_linetracebinding.hpp \
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
    app/example/disasteroids/ui/dis_gamewidget.hpp \
    app/example/disasteroids/ui/dis_highscorenameentrydialog.hpp \
    app/example/disasteroids/ui/dis_highscoreswidget.hpp \
    app/example/disasteroids/ui/dis_inventorybutton.hpp \
    app/example/disasteroids/ui/dis_inventorypanel.hpp \
    app/example/disasteroids/ui/dis_optionspanel.hpp \
    app/example/disasteroids/ui/dis_titlescreenwidget.hpp \
    app/example/disasteroids/ui/dis_worldview.hpp \
	\
	app/example/guidemo/gui_masterwidget.hpp
