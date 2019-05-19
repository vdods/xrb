cmake_minimum_required(VERSION 3.0) # I had problems if I only specified "VERSION 3".

project(disasteroids VERSION 0.5)
set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)

# if(NOT xrb_DIR)
#     set(xrb_DIR "xrb_DIR-NOTFOUND" CACHE STRING "Path to xrb library.")
#     message(FATAL_ERROR "You must set a valid path for xrb_DIR, which should be the path xrb library.")
# endif()

# Options to correctly link the standard C++ lib on Mac.
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # This is the correct way to detect Mac OS X operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang") # GCC ("GNU") probably would require a different option
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
endif()

###################################################################################################
# External packages
###################################################################################################

find_package(xrb 0.5)

###################################################################################################
# disasteroids target
###################################################################################################

add_executable(disasteroids
    dis_commandlineoptions.cpp
    dis_commandlineoptions.hpp
    dis_config.cpp
    dis_config.hpp
    dis_enums.cpp
    dis_enums.hpp
    dis_events.hpp
    dis_main.cpp
    dis_master.cpp
    dis_master.hpp
    dis_resourcecache.cpp
    dis_resourcecache.hpp
    game/dis_highscores.cpp
    game/dis_highscores.hpp
    game/dis_item.cpp
    game/dis_item.hpp
    game/dis_spawn.cpp
    game/dis_spawn.hpp
    game/dis_util.cpp
    game/dis_util.hpp
    game/engine2/dis_entity.cpp
    game/engine2/dis_entity.hpp
    game/engine2/dis_entityreference.hpp
    game/engine2/dis_physicshandler.cpp
    game/engine2/dis_physicshandler.hpp
    game/engine2/dis_world.cpp
    game/engine2/dis_world.hpp
    game/entities/dis_asteroid.cpp
    game/entities/dis_asteroid.hpp
    game/entities/dis_ballistic.cpp
    game/entities/dis_ballistic.hpp
    game/entities/dis_effect.cpp
    game/entities/dis_effect.hpp
    game/entities/dis_explosive.cpp
    game/entities/dis_explosive.hpp
    game/entities/dis_healthtrigger.cpp
    game/entities/dis_healthtrigger.hpp
    game/entities/dis_mortal.cpp
    game/entities/dis_mortal.hpp
    game/entities/dis_powerup.cpp
    game/entities/dis_powerup.hpp
    game/entities/dis_ship.cpp
    game/entities/dis_ship.hpp
    game/entities/ships/dis_demi.cpp
    game/entities/ships/dis_demi.hpp
    game/entities/ships/dis_devourment.cpp
    game/entities/ships/dis_devourment.hpp
    game/entities/ships/dis_enemyship.cpp
    game/entities/ships/dis_enemyship.hpp
    game/entities/ships/dis_interloper.cpp
    game/entities/ships/dis_interloper.hpp
    game/entities/ships/dis_playership.cpp
    game/entities/ships/dis_playership.hpp
    game/entities/ships/dis_revulsion.cpp
    game/entities/ships/dis_revulsion.hpp
    game/entities/ships/dis_shade.cpp
    game/entities/ships/dis_shade.hpp
    game/entities/ships/dis_solitary.cpp
    game/entities/ships/dis_solitary.hpp
    game/items/dis_armor.cpp
    game/items/dis_armor.hpp
    game/items/dis_engine.cpp
    game/items/dis_engine.hpp
    game/items/dis_powereddevice.hpp
    game/items/dis_powergenerator.cpp
    game/items/dis_powergenerator.hpp
    game/items/dis_shield.cpp
    game/items/dis_shield.hpp
    game/items/dis_weapon.cpp
    game/items/dis_weapon.hpp
    ui/dis_controlspanel.cpp
    ui/dis_controlspanel.hpp
    ui/dis_gamewidget.cpp
    ui/dis_gamewidget.hpp
    ui/dis_highscorenameentrydialog.cpp
    ui/dis_highscorenameentrydialog.hpp
    ui/dis_highscoreswidget.cpp
    ui/dis_highscoreswidget.hpp
    ui/dis_inventorybutton.cpp
    ui/dis_inventorybutton.hpp
    ui/dis_inventorypanel.cpp
    ui/dis_inventorypanel.hpp
    ui/dis_titlescreenwidget.cpp
    ui/dis_titlescreenwidget.hpp
    ui/dis_worldview.cpp
    ui/dis_worldview.hpp
)

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU") # "GNU" is GCC
    target_compile_options(
        disasteroids PUBLIC
        -Wall -Werror -Wno-reorder -Wno-unused-variable -Wno-overloaded-virtual -Wno-unused-private-field -Wno-unused-but-set-variable
    )
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
    target_compile_options(
        disasteroids PUBLIC
        /Wall /WX # /WX is to treat warnings as errors.
    )
endif()

target_include_directories(
    disasteroids PUBLIC
    .
    game
    game/engine2
    game/entities
    game/entities/ships
    game/items
    ui
)
target_link_libraries(
    disasteroids PUBLIC
    xrb::xrb
)

###################################################################################################
# Post-build resource-copying steps
###################################################################################################

add_custom_command(
    TARGET disasteroids
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${xrb_DIR}/resources" "resources"
)
