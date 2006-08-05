bin_PROGRAMS = \
    disasteroids

disasteroids_SOURCES = \
    dis_config.cpp \
    dis_main.cpp \
    dis_master.cpp \
    dis_options.cpp \
    dis_resourcecache.cpp \
    game/dis_highscores.cpp \
    game/dis_item.cpp \
    game/dis_spawn.cpp \
    game/dis_util.cpp \
    game/engine2/dis_collisionquadtree.cpp \
    game/engine2/dis_entity.cpp \
    game/engine2/dis_physicshandler.cpp \
    game/engine2/dis_world.cpp \
    game/entities/dis_asteroid.cpp \
    game/entities/dis_ballistic.cpp \
    game/entities/dis_effect.cpp \
    game/entities/dis_explosive.cpp \
    game/entities/dis_healthtrigger.cpp \
    game/entities/dis_mortal.cpp \
    game/entities/dis_powerup.cpp \
    game/entities/dis_ship.cpp \
    game/entities/ships/dis_demi.cpp \
    game/entities/ships/dis_devourment.cpp \
    game/entities/ships/dis_enemyship.cpp \
    game/entities/ships/dis_interloper.cpp \
    game/entities/ships/dis_playership.cpp \
    game/entities/ships/dis_revulsion.cpp \
    game/entities/ships/dis_shade.cpp \
    game/entities/ships/dis_solitary.cpp \
    game/items/dis_armor.cpp \
    game/items/dis_engine.cpp \
    game/items/dis_powergenerator.cpp \
    game/items/dis_shield.cpp \
    game/items/dis_weapon.cpp \
    ui/dis_gamewidget.cpp \
    ui/dis_highscorenameentrydialog.cpp \
    ui/dis_highscoreswidget.cpp \
    ui/dis_inventorybutton.cpp \
    ui/dis_inventorypanel.cpp \
    ui/dis_optionspanel.cpp \
    ui/dis_titlescreenwidget.cpp \
    ui/dis_worldview.cpp

noinst_HEADERS = \
    dis_config.h \
    dis_enums.h \
    dis_events.h \
    dis_master.h \
    dis_options.h \
    dis_resourcecache.h \
    game/dis_highscores.h \
    game/dis_item.h \
    game/dis_spawn.h \
    game/dis_util.h \
    game/engine2/dis_areatracelist.h \
    game/engine2/dis_collisionpair.h \
    game/engine2/dis_collisionquadtree.h \
    game/engine2/dis_entity.h \
    game/engine2/dis_entityreference.h \
    game/engine2/dis_linetracebinding.h \
    game/engine2/dis_physicshandler.h \
    game/engine2/dis_world.h \
    game/entities/dis_asteroid.h \
    game/entities/dis_ballistic.h \
    game/entities/dis_effect.h \
    game/entities/dis_explosive.h \
    game/entities/dis_healthtrigger.h \
    game/entities/dis_mortal.h \
    game/entities/dis_powerup.h \
    game/entities/dis_ship.h \
    game/entities/ships/dis_demi.h \
    game/entities/ships/dis_devourment.h \
    game/entities/ships/dis_enemyship.h \
    game/entities/ships/dis_interloper.h \
    game/entities/ships/dis_playership.h \
    game/entities/ships/dis_revulsion.h \
    game/entities/ships/dis_shade.h \
    game/entities/ships/dis_solitary.h \
    game/items/dis_armor.h \
    game/items/dis_engine.h \
    game/items/dis_powereddevice.h \
    game/items/dis_powergenerator.h \
    game/items/dis_shield.h \
    game/items/dis_weapon.h \
    ui/dis_gamewidget.h \
    ui/dis_highscorenameentrydialog.h \
    ui/dis_highscoreswidget.h \
    ui/dis_inventorybutton.h \
    ui/dis_inventorypanel.h \
    ui/dis_optionspanel.h \
    ui/dis_titlescreenwidget.h \
    ui/dis_worldview.h

AM_CPPFLAGS = \
    `$(FREETYPE_CONFIG) --cflags` \
    `$(SDL_CONFIG) --cflags` \
    \
    -I$(top_srcdir)/app/example/disasteroids \
    -I$(top_srcdir)/app/example/disasteroids/game \
    -I$(top_srcdir)/app/example/disasteroids/game/engine2 \
    -I$(top_srcdir)/app/example/disasteroids/game/entities \
    -I$(top_srcdir)/app/example/disasteroids/game/entities/ships \
    -I$(top_srcdir)/app/example/disasteroids/game/items \
    -I$(top_srcdir)/app/example/disasteroids/ui \
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

disasteroids_LDADD = \
    $(LIBOBJS) \
    $(top_builddir)/lib/libxrb.la
