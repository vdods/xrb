bin_PROGRAMS = \
    disasteroids

disasteroids_SOURCES = \
    dis_highscores.cpp \
    dis_main.cpp \
    dis_master.cpp \
    dis_resourcecache.cpp \
    engine2/dis_collisionquadtree.cpp \
    engine2/dis_physicshandler.cpp \
    engine2/dis_world.cpp \
    game/dis_gameobject.cpp \
    game/dis_item.cpp \
    game/dis_spawn.cpp \
    game/dis_util.cpp \
    game/gameobjects/dis_asteroid.cpp \
    game/gameobjects/dis_ballistic.cpp \
    game/gameobjects/dis_effect.cpp \
    game/gameobjects/dis_explosive.cpp \
    game/gameobjects/dis_healthtrigger.cpp \
    game/gameobjects/dis_mortal.cpp \
    game/gameobjects/dis_powerup.cpp \
    game/gameobjects/dis_ship.cpp \
    game/gameobjects/ships/dis_devourment.cpp \
    game/gameobjects/ships/dis_enemyship.cpp \
    game/gameobjects/ships/dis_interloper.cpp \
    game/gameobjects/ships/dis_playership.cpp \
    game/gameobjects/ships/dis_revulsion.cpp \
    game/gameobjects/ships/dis_shade.cpp \
    game/gameobjects/ships/dis_solitary.cpp \
    game/items/dis_armor.cpp \
    game/items/dis_engine.cpp \
    game/items/dis_powergenerator.cpp \
    game/items/dis_shield.cpp \
    game/items/dis_weapon.cpp \
    ui/dis_gamewidget.cpp \
    ui/dis_highscorenameentrydialog.cpp \
    ui/dis_inventorybutton.cpp \
    ui/dis_inventorypanel.cpp \
    ui/dis_optionspanel.cpp \
    ui/dis_progressbar.cpp \
    ui/dis_titlescreenwidget.cpp \
    ui/dis_worldview.cpp
    
noinst_HEADERS = \
    dis_enums.h \
    dis_events.h \
    dis_highscores.h \
    dis_master.h \
    dis_resourcecache.h \
    engine2/dis_areatracelist.h \
    engine2/dis_collisionpair.h \
    engine2/dis_collisionquadtree.h \
    engine2/dis_linetracebinding.h \
    engine2/dis_physicshandler.h \
    engine2/dis_world.h \
    game/dis_gameobject.h \
    game/dis_gameobjectreference.h \
    game/dis_item.h \
    game/dis_spawn.h \
    game/dis_util.h \
    game/gameobjects/dis_asteroid.h \
    game/gameobjects/dis_ballistic.h \
    game/gameobjects/dis_effect.h \
    game/gameobjects/dis_explosive.h \
    game/gameobjects/dis_healthtrigger.h \
    game/gameobjects/dis_mortal.h \
    game/gameobjects/dis_powerup.h \
    game/gameobjects/dis_ship.h \
    game/gameobjects/ships/dis_devourment.h \
    game/gameobjects/ships/dis_enemyship.h \
    game/gameobjects/ships/dis_interloper.h \
    game/gameobjects/ships/dis_playership.h \
    game/gameobjects/ships/dis_revulsion.h \
    game/gameobjects/ships/dis_shade.h \
    game/gameobjects/ships/dis_solitary.h \
    game/items/dis_armor.h \
    game/items/dis_engine.h \
    game/items/dis_powereddevice.h \
    game/items/dis_powergenerator.h \
    game/items/dis_shield.h \
    game/items/dis_weapon.h \
    ui/dis_gamewidget.h \
    ui/dis_highscorenameentrydialog.h \
    ui/dis_inventorybutton.h \
    ui/dis_inventorypanel.h \
    ui/dis_optionspanel.h \
    ui/dis_progressbar.h \
    ui/dis_titlescreenwidget.h \
    ui/dis_worldview.h

AM_CPPFLAGS = \
    `$(FREETYPE_CONFIG) --cflags` \
    `$(SDL_CONFIG) --cflags` \
    \
    -I$(top_srcdir)/app/example/disasteroids/engine2 \
    -I$(top_srcdir)/app/example/disasteroids/game \
    -I$(top_srcdir)/app/example/disasteroids/game/gameobjects \
    -I$(top_srcdir)/app/example/disasteroids/game/gameobjects/ships \
    -I$(top_srcdir)/app/example/disasteroids/game/items \
    -I$(top_srcdir)/app/example/disasteroids/ui \
    \
    -I$(top_srcdir)/lib/core \
    -I$(top_srcdir)/lib/core/serializers \
    -I$(top_srcdir)/lib/engine2 \
    -I$(top_srcdir)/lib/engine2/physicshandlers \
    -I$(top_srcdir)/lib/gui \
    -I$(top_srcdir)/lib/gui/widgets \
    -I$(top_srcdir)/lib/input \
    -I$(top_srcdir)/lib/mapeditor2 \
    -I$(top_srcdir)/lib/math \
    -I$(top_srcdir)/lib/parsers \
    -I$(top_srcdir)/lib/primitive \
    -I$(top_srcdir)/lib/render \
    -I$(top_srcdir)/lib/render/engine2_worldviews \
    -I$(top_srcdir)/lib/sound \
    -I$(top_srcdir)/lib/util

AM_LDFLAGS = \
    `$(FREETYPE_CONFIG) --libs` \
    `$(SDL_CONFIG) --libs`

disasteroids_LDADD = \
    $(LIBOBJS) \
    $(top_builddir)/lib/libxrb.la
