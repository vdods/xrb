cmake_minimum_required(VERSION 3.0) # I had problems if I only specified "VERSION 3".

project(disasteroids VERSION 0.5)
set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)

if(NOT xrb_DIR)
    set(xrb_DIR "xrb_DIR-NOTFOUND" CACHE STRING "Path to build directory of xrb")
    message(FATAL_ERROR "You must set a valid path for xrb_DIR, which should be the path to build directory of xrb")
endif()

if(NOT LEAP_CMAKE_MODULES)
    set(LEAP_CMAKE_MODULES "LEAP_CMAKE_MODULES-NOTFOUND" CACHE STRING "Path to clone of git repository github.com/leapmotion/cmake-modules")
    message(FATAL_ERROR "You must set a valid path for LEAP_CMAKE_MODULES, which should be the path to clone of git repository github.com/leapmotion/cmake-modules")
endif()

list(APPEND CMAKE_MODULE_PATH ${LEAP_CMAKE_MODULES})
include(TargetImportedLibraries)

# Options to correctly link the standard C++ lib on Mac.
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # This is the correct way to detect Mac OS X operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang") # GCC ("GNU") probably would require a different option
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
endif()

# External packages

# list(APPEND CMAKE_PREFIX_PATH "/usr")
# list(APPEND CMAKE_PREFIX_PATH "/usr/local")

# find_package(SDL 1.2)
# find_package(FreeType 2)
# find_package(PNG 1.5)
# find_package(OpenGL 1.2)

# Helper target(s)

add_library(Strict INTERFACE)
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    target_compile_options(Strict INTERFACE -Werror -Wall)
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
    # TODO
endif()

add_library(C++11 INTERFACE)
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    target_compile_options(C++11 INTERFACE -std=c++11)
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
    # TODO
endif()

add_library(SaveTemps INTERFACE)
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    target_compile_options(SaveTemps INTERFACE -save-temps)
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
    # TODO
endif()

add_library(NoOptimize INTERFACE)
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    target_compile_options(NoOptimize INTERFACE -O0)
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
    # TODO
endif()

find_package(xrb)

###############################################################################
# disasteroids target
###############################################################################

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
target_include_directories(disasteroids PUBLIC
    .
    game
    game/engine2
    game/entities
    game/entities/ships
    game/items
    ui
)
target_link_libraries(disasteroids PUBLIC xrb::xrb)

###################################################################################################
# Post-build resource-copying steps
###################################################################################################

add_custom_command(
    TARGET disasteroids
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${xrb_DIR}/resources" "resources"
)

###################################################################################################
# Global compiler options (and related) go here.  NOTE: ANY SUBLIBRARY-SPECIFIC COMPILER OPTIONS
# SHOULD BE MADE IN THE CORRESPONDING SUBLIBRARY DEFINITION!
###################################################################################################

# TODO: look into using target_compile_definitions and target_compile_options,
# though these seem sufficiently global to warrant the extreme measure of
# setting CMAKE_CXX_FLAGS.

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU") # "GNU" is GCC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-reorder -Wno-unused-variable -Wno-overloaded-virtual -Wno-unused-private-field")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /WX") # /WX is to treat warnings as errors.
endif()
