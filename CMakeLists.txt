cmake_minimum_required(VERSION 3.0) # I had problems if I only specified "VERSION 3".

project(xrb VERSION 0.5)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})

if(NOT LEAP_CMAKE_MODULES)
    set(LEAP_CMAKE_MODULES "LEAP_CMAKE_MODULES-NOTFOUND" CACHE STRING "Path to clone of git repository github.com/leapmotion/cmake-modules")
    message(FATAL_ERROR "You must set a valid path for LEAP_CMAKE_MODULES, which should be the path to clone of git repository github.com/leapmotion/cmake-modules")
endif()

if(NOT XRB_RESOURCES_DIR)
    set(XRB_RESOURCES_DIR "XRB_RESOURCES_DIR-NOTFOUND" CACHE STRING "Path to XRB `resources` directory, the zipped contents of which can be found at http://thedods.com/victor/resources.zip")
    message(FATAL_ERROR "You must set a valid path for XRB_RESOURCES_DIR, which should be the path to XRB `resources` directory, the zipped contents of which can be found at http://thedods.com/victor/resources.zip")
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

find_package(SDL 1.2)
find_package(FreeType 2)
find_package(PNG 1.5)
find_package(OpenGL 1.2)

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

###############################################################################
# xrb target
###############################################################################

set(xrb_HEADERS
    lib/core/xrb.hpp
    lib/core/xrb_assert.hpp
    lib/core/xrb_debug.hpp
    lib/core/xrb_delete.hpp
    lib/core/xrb_types.hpp
    lib/engine2/circle/xrb_engine2_circle_collisionquadtree.hpp
    lib/engine2/circle/xrb_engine2_circle_entity.hpp
    lib/engine2/circle/xrb_engine2_circle_physicshandler.hpp
    lib/engine2/circle/xrb_engine2_circle_types.hpp
    lib/engine2/xrb_engine2_animatedsprite.hpp
    lib/engine2/xrb_engine2_compound.hpp
    lib/engine2/xrb_engine2_entity.hpp
    lib/engine2/xrb_engine2_enums.hpp
    lib/engine2/xrb_engine2_events.hpp
    lib/engine2/xrb_engine2_object.hpp
    lib/engine2/xrb_engine2_objectlayer.hpp
    lib/engine2/xrb_engine2_physicshandler.hpp
    lib/engine2/xrb_engine2_polygon.hpp
    lib/engine2/xrb_engine2_quadtree.hpp
    lib/engine2/xrb_engine2_sprite.hpp
    lib/engine2/xrb_engine2_svgworldloader.hpp
    lib/engine2/xrb_engine2_types.hpp
    lib/engine2/xrb_engine2_visibilityquadtree.hpp
    lib/engine2/xrb_engine2_world.hpp
    lib/engine2/xrb_engine2_worldview.hpp
    lib/engine2/xrb_engine2_worldviewwidget.hpp
    lib/gui/widgets/xrb_button.hpp
    lib/gui/widgets/xrb_cellpaddingwidget.hpp
    lib/gui/widgets/xrb_checkbox.hpp
    lib/gui/widgets/xrb_dialog.hpp
    lib/gui/widgets/xrb_filedialog.hpp
    lib/gui/widgets/xrb_filepanel.hpp
    lib/gui/widgets/xrb_keyselectorbutton.hpp
    lib/gui/widgets/xrb_label.hpp
    lib/gui/widgets/xrb_layout.hpp
    lib/gui/widgets/xrb_lineedit.hpp
    lib/gui/widgets/xrb_modalwidget.hpp
    lib/gui/widgets/xrb_progressbar.hpp
    lib/gui/widgets/xrb_radiobutton.hpp
    lib/gui/widgets/xrb_spacerwidget.hpp
    lib/gui/widgets/xrb_textwidget.hpp
    lib/gui/widgets/xrb_valueedit.hpp
    lib/gui/widgets/xrb_valuelabel.hpp
    lib/gui/widgets/xrb_widgetstack.hpp
    lib/gui/xrb_containerwidget.hpp
    lib/gui/xrb_gui_events.hpp
    lib/gui/xrb_margins.hpp
    lib/gui/xrb_screen.hpp
    lib/gui/xrb_screencoord.hpp
    lib/gui/xrb_sizeproperties.hpp
    lib/gui/xrb_stylesheet.hpp
    lib/gui/xrb_widget.hpp
    lib/gui/xrb_widgetbackground.hpp
    lib/gui/xrb_widgetcontext.hpp
    lib/input/xrb_input_events.hpp
    lib/input/xrb_inputstate.hpp
    lib/input/xrb_key.hpp
    lib/input/xrb_keymap.hpp
    lib/input/xrb_keyrepeater.hpp
    lib/lvd/lvd_filoc.hpp
    lib/lvd/lvd_xml.hpp
    lib/lvd/lvd_xml_parser.hpp
    lib/lvd/lvd_xml_scanner.hpp
    lib/math/xrb_math.hpp
    lib/math/xrb_matrix2.hpp
    lib/math/xrb_polynomial.hpp
    lib/math/xrb_simpletransform2.hpp
    lib/math/xrb_transform2.hpp
    lib/math/xrb_vector.hpp
    lib/parse/xrb_parse_datafile.hpp
    lib/parse/xrb_parse_datafile_parser.hpp
    lib/parse/xrb_parse_datafile_scanner.hpp
    lib/parse/xrb_parse_datafile_value.hpp
    lib/parse/xrb_parse_filoc.hpp
    lib/parse/xrb_parse_util.hpp
    lib/primitive/xrb_circularqueue.hpp
    lib/primitive/xrb_color.hpp
    lib/primitive/xrb_exception.hpp
    lib/primitive/xrb_ntuple.hpp
    lib/primitive/xrb_rect.hpp
    lib/primitive/xrb_texture.hpp
    lib/primitive/xrb_time.hpp
    lib/render/fonts/xrb_asciifont.hpp
    lib/render/xrb_animation.hpp
    lib/render/xrb_font.hpp
    lib/render/xrb_gl.hpp
    lib/render/xrb_gltexture.hpp
    lib/render/xrb_gltextureatlas.hpp
    lib/render/xrb_render.hpp
    lib/render/xrb_rendercontext.hpp
    lib/system/pals/xrb_sdlpal.hpp
    lib/system/serializers/xrb_binaryfileserializer.hpp
    lib/system/xrb_enums.hpp
    lib/system/xrb_event.hpp
    lib/system/xrb_eventhandler.hpp
    lib/system/xrb_eventqueue.hpp
    lib/system/xrb_filesystem.hpp
    lib/system/xrb_framehandler.hpp
    lib/system/xrb_pal.hpp
    lib/system/xrb_resourcelibrary.hpp
    lib/system/xrb_resourceloadparameters.hpp
    lib/system/xrb_serializer.hpp
    lib/system/xrb_signalhandler.hpp
    lib/system/xrb_singleton.hpp
    lib/system/xrb_statemachine.hpp
    lib/util/xrb_characterfilter.hpp
    lib/util/xrb_commandlineparser.hpp
    lib/util/xrb_emptystring.hpp
    lib/util/xrb_endian.hpp
    lib/util/xrb_frameratecalculator.hpp
    lib/util/xrb_indentformatter.hpp
    lib/util/xrb_reference.hpp
    lib/util/xrb_tokenizer.hpp
    lib/util/xrb_transformation.hpp
    lib/util/xrb_utf8.hpp
    lib/util/xrb_util.hpp
    lib/util/xrb_validator.hpp
)
set(xrb_SOURCES
    lib/core/xrb_compiletimeasserts.cpp
    lib/engine2/circle/xrb_engine2_circle_collisionquadtree.cpp
    lib/engine2/circle/xrb_engine2_circle_entity.cpp
    lib/engine2/circle/xrb_engine2_circle_physicshandler.cpp
    lib/engine2/xrb_engine2_animatedsprite.cpp
    lib/engine2/xrb_engine2_compound.cpp
    lib/engine2/xrb_engine2_entity.cpp
    lib/engine2/xrb_engine2_events.cpp
    lib/engine2/xrb_engine2_object.cpp
    lib/engine2/xrb_engine2_objectlayer.cpp
    lib/engine2/xrb_engine2_polygon.cpp
    lib/engine2/xrb_engine2_quadtree.cpp
    lib/engine2/xrb_engine2_sprite.cpp
    lib/engine2/xrb_engine2_svgworldloader.cpp
    lib/engine2/xrb_engine2_types.cpp
    lib/engine2/xrb_engine2_visibilityquadtree.cpp
    lib/engine2/xrb_engine2_world.cpp
    lib/engine2/xrb_engine2_worldview.cpp
    lib/engine2/xrb_engine2_worldviewwidget.cpp
    lib/gui/widgets/xrb_button.cpp
    lib/gui/widgets/xrb_cellpaddingwidget.cpp
    lib/gui/widgets/xrb_checkbox.cpp
    lib/gui/widgets/xrb_dialog.cpp
    lib/gui/widgets/xrb_filedialog.cpp
    lib/gui/widgets/xrb_filepanel.cpp
    lib/gui/widgets/xrb_keyselectorbutton.cpp
    lib/gui/widgets/xrb_label.cpp
    lib/gui/widgets/xrb_layout.cpp
    lib/gui/widgets/xrb_lineedit.cpp
    lib/gui/widgets/xrb_modalwidget.cpp
    lib/gui/widgets/xrb_progressbar.cpp
    lib/gui/widgets/xrb_spacerwidget.cpp
    lib/gui/widgets/xrb_textwidget.cpp
    lib/gui/widgets/xrb_widgetstack.cpp
    lib/gui/xrb_containerwidget.cpp
    lib/gui/xrb_gui_events.cpp
    lib/gui/xrb_screen.cpp
    lib/gui/xrb_screencoord.cpp
    lib/gui/xrb_sizeproperties.cpp
    lib/gui/xrb_stylesheet.cpp
    lib/gui/xrb_widget.cpp
    lib/gui/xrb_widgetbackground.cpp
    lib/gui/xrb_widgetcontext.cpp
    lib/input/xrb_input_events.cpp
    lib/input/xrb_inputstate.cpp
    lib/input/xrb_key.cpp
    lib/input/xrb_keymap.cpp
    lib/input/xrb_keyrepeater.cpp
    lib/lvd/lvd_filoc.cpp
    lib/lvd/lvd_xml.cpp
    lib/lvd/lvd_xml_parser.cpp
    lib/lvd/lvd_xml_scanner.cpp
    lib/math/xrb_math.cpp
    lib/math/xrb_polynomial.cpp
    lib/parse/xrb_parse_datafile.cpp
    lib/parse/xrb_parse_datafile_parser.cpp
    lib/parse/xrb_parse_datafile_scanner.cpp
    lib/parse/xrb_parse_datafile_value.cpp
    lib/parse/xrb_parse_filoc.cpp
    lib/parse/xrb_parse_util.cpp
    lib/primitive/xrb_color.cpp
    lib/primitive/xrb_texture.cpp
    lib/primitive/xrb_time.cpp
    lib/render/fonts/xrb_asciifont.cpp
    lib/render/xrb_animation.cpp
    lib/render/xrb_font.cpp
    lib/render/xrb_gl.cpp
    lib/render/xrb_gltexture.cpp
    lib/render/xrb_gltextureatlas.cpp
    lib/render/xrb_render.cpp
    lib/render/xrb_rendercontext.cpp
    lib/system/pals/xrb_sdlpal.cpp
    lib/system/serializers/xrb_binaryfileserializer.cpp
    lib/system/xrb_event.cpp
    lib/system/xrb_eventhandler.cpp
    lib/system/xrb_eventqueue.cpp
    lib/system/xrb_filesystem.cpp
    lib/system/xrb_framehandler.cpp
    lib/system/xrb_resourcelibrary.cpp
    lib/system/xrb_signalhandler.cpp
    lib/system/xrb_singleton.cpp
    lib/util/xrb_characterfilter.cpp
    lib/util/xrb_commandlineparser.cpp
    lib/util/xrb_emptystring.cpp
    lib/util/xrb_frameratecalculator.cpp
    lib/util/xrb_indentformatter.cpp
    lib/util/xrb_tokenizer.cpp
    lib/util/xrb_transformation.cpp
    lib/util/xrb_utf8.cpp
    lib/util/xrb_util.cpp
)
add_library(xrb STATIC
    ${xrb_HEADERS}
    ${xrb_SOURCES}
)
set(xrb_INCLUDE_DIRECTORIES
    lib/core
    lib/engine2
    lib/engine2/circle
    lib/gui
    lib/gui/widgets
    lib/input
    lib/lvd
    lib/math
    lib/parse
    lib/primitive
    lib/render
    lib/render/fonts
    lib/sound
    lib/system
    lib/system/pals
    lib/system/serializers
    lib/util
)
foreach(_include_dir ${xrb_INCLUDE_DIRECTORIES})
    target_include_directories(
        xrb PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${_include_dir}>
        $<INSTALL_INTERFACE:include/${_include_dir}>
    )
endforeach()
target_compile_options(xrb PUBLIC -DXRB_DEBUG_LEVEL=3) # TODO make cmake option for this

target_include_directories(xrb PUBLIC
    ${FREETYPE_INCLUDE_DIR_freetype2}   # This shouldn't be necessary
    ${FREETYPE_INCLUDE_DIR_ftbuild}     # This shouldn't be necessary
    ${PNG_PNG_INCLUDE_DIR}              # This shouldn't be necessary
    ${SDL_INCLUDE_DIR}                  # This shouldn't be necessary
)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # This is the correct way to detect Mac OS X operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
    target_link_libraries(xrb PUBLIC FreeType "-framework OpenGL" PNG SDL SDLmain "-framework Cocoa")
else() # Linux, Windows
    target_link_libraries(xrb PUBLIC FreeType OpenGL PNG SDL SDLmain)
endif()

# ###############################################################################
# # disasteroids target
# ###############################################################################

# add_executable(disasteroids
#     app/example/disasteroids/dis_commandlineoptions.cpp
#     app/example/disasteroids/dis_commandlineoptions.hpp
#     app/example/disasteroids/dis_config.cpp
#     app/example/disasteroids/dis_config.hpp
#     app/example/disasteroids/dis_enums.cpp
#     app/example/disasteroids/dis_enums.hpp
#     app/example/disasteroids/dis_events.hpp
#     app/example/disasteroids/dis_main.cpp
#     app/example/disasteroids/dis_master.cpp
#     app/example/disasteroids/dis_master.hpp
#     app/example/disasteroids/dis_resourcecache.cpp
#     app/example/disasteroids/dis_resourcecache.hpp
#     app/example/disasteroids/game/dis_highscores.cpp
#     app/example/disasteroids/game/dis_highscores.hpp
#     app/example/disasteroids/game/dis_item.cpp
#     app/example/disasteroids/game/dis_item.hpp
#     app/example/disasteroids/game/dis_spawn.cpp
#     app/example/disasteroids/game/dis_spawn.hpp
#     app/example/disasteroids/game/dis_util.cpp
#     app/example/disasteroids/game/dis_util.hpp
#     app/example/disasteroids/game/engine2/dis_entity.cpp
#     app/example/disasteroids/game/engine2/dis_entity.hpp
#     app/example/disasteroids/game/engine2/dis_entityreference.hpp
#     app/example/disasteroids/game/engine2/dis_physicshandler.cpp
#     app/example/disasteroids/game/engine2/dis_physicshandler.hpp
#     app/example/disasteroids/game/engine2/dis_world.cpp
#     app/example/disasteroids/game/engine2/dis_world.hpp
#     app/example/disasteroids/game/entities/dis_asteroid.cpp
#     app/example/disasteroids/game/entities/dis_asteroid.hpp
#     app/example/disasteroids/game/entities/dis_ballistic.cpp
#     app/example/disasteroids/game/entities/dis_ballistic.hpp
#     app/example/disasteroids/game/entities/dis_effect.cpp
#     app/example/disasteroids/game/entities/dis_effect.hpp
#     app/example/disasteroids/game/entities/dis_explosive.cpp
#     app/example/disasteroids/game/entities/dis_explosive.hpp
#     app/example/disasteroids/game/entities/dis_healthtrigger.cpp
#     app/example/disasteroids/game/entities/dis_healthtrigger.hpp
#     app/example/disasteroids/game/entities/dis_mortal.cpp
#     app/example/disasteroids/game/entities/dis_mortal.hpp
#     app/example/disasteroids/game/entities/dis_powerup.cpp
#     app/example/disasteroids/game/entities/dis_powerup.hpp
#     app/example/disasteroids/game/entities/dis_ship.cpp
#     app/example/disasteroids/game/entities/dis_ship.hpp
#     app/example/disasteroids/game/entities/ships/dis_demi.cpp
#     app/example/disasteroids/game/entities/ships/dis_demi.hpp
#     app/example/disasteroids/game/entities/ships/dis_devourment.cpp
#     app/example/disasteroids/game/entities/ships/dis_devourment.hpp
#     app/example/disasteroids/game/entities/ships/dis_enemyship.cpp
#     app/example/disasteroids/game/entities/ships/dis_enemyship.hpp
#     app/example/disasteroids/game/entities/ships/dis_interloper.cpp
#     app/example/disasteroids/game/entities/ships/dis_interloper.hpp
#     app/example/disasteroids/game/entities/ships/dis_playership.cpp
#     app/example/disasteroids/game/entities/ships/dis_playership.hpp
#     app/example/disasteroids/game/entities/ships/dis_revulsion.cpp
#     app/example/disasteroids/game/entities/ships/dis_revulsion.hpp
#     app/example/disasteroids/game/entities/ships/dis_shade.cpp
#     app/example/disasteroids/game/entities/ships/dis_shade.hpp
#     app/example/disasteroids/game/entities/ships/dis_solitary.cpp
#     app/example/disasteroids/game/entities/ships/dis_solitary.hpp
#     app/example/disasteroids/game/items/dis_armor.cpp
#     app/example/disasteroids/game/items/dis_armor.hpp
#     app/example/disasteroids/game/items/dis_engine.cpp
#     app/example/disasteroids/game/items/dis_engine.hpp
#     app/example/disasteroids/game/items/dis_powereddevice.hpp
#     app/example/disasteroids/game/items/dis_powergenerator.cpp
#     app/example/disasteroids/game/items/dis_powergenerator.hpp
#     app/example/disasteroids/game/items/dis_shield.cpp
#     app/example/disasteroids/game/items/dis_shield.hpp
#     app/example/disasteroids/game/items/dis_weapon.cpp
#     app/example/disasteroids/game/items/dis_weapon.hpp
#     app/example/disasteroids/ui/dis_controlspanel.cpp
#     app/example/disasteroids/ui/dis_controlspanel.hpp
#     app/example/disasteroids/ui/dis_gamewidget.cpp
#     app/example/disasteroids/ui/dis_gamewidget.hpp
#     app/example/disasteroids/ui/dis_highscorenameentrydialog.cpp
#     app/example/disasteroids/ui/dis_highscorenameentrydialog.hpp
#     app/example/disasteroids/ui/dis_highscoreswidget.cpp
#     app/example/disasteroids/ui/dis_highscoreswidget.hpp
#     app/example/disasteroids/ui/dis_inventorybutton.cpp
#     app/example/disasteroids/ui/dis_inventorybutton.hpp
#     app/example/disasteroids/ui/dis_inventorypanel.cpp
#     app/example/disasteroids/ui/dis_inventorypanel.hpp
#     app/example/disasteroids/ui/dis_titlescreenwidget.cpp
#     app/example/disasteroids/ui/dis_titlescreenwidget.hpp
#     app/example/disasteroids/ui/dis_worldview.cpp
#     app/example/disasteroids/ui/dis_worldview.hpp
# )
# target_include_directories(disasteroids PUBLIC
#     app/example/disasteroids
#     app/example/disasteroids/game
#     app/example/disasteroids/game/engine2
#     app/example/disasteroids/game/entities
#     app/example/disasteroids/game/entities/ships
#     app/example/disasteroids/game/items
#     app/example/disasteroids/ui
# )
# target_link_libraries(disasteroids PUBLIC xrb)

###################################################################################################
# Global compiler options (and related) go here.
###################################################################################################

# TODO: look into using target_compile_definitions and target_compile_options,
# though these seem sufficiently global to warrant the extreme measure of
# setting CMAKE_CXX_FLAGS.

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU") # "GNU" is GCC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-reorder -Wno-unused-variable -Wno-overloaded-virtual -Wno-unused-private-field")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /WX") # /WX is to treat warnings as errors.
endif()

###################################################################################################
# Install and export rules
###################################################################################################

install(
    TARGETS xrb
    EXPORT xrbRelease
    CONFIGURATIONS Release # This line has to come before the others, or else CMake will ignore it!
    # (See http://www.cmake.org/pipermail/cmake/2013-October/056190.html)
    LIBRARY DESTINATION lib/Release
    ARCHIVE DESTINATION lib/Release
    INCLUDES DESTINATION "include"
)
install(
    TARGETS xrb
    EXPORT xrbDebug
    CONFIGURATIONS Debug # This line has to come before the others, or else CMake will ignore it!
    # (See http://www.cmake.org/pipermail/cmake/2013-October/056190.html)
    LIBRARY DESTINATION lib/Debug
    ARCHIVE DESTINATION lib/Debug
    INCLUDES DESTINATION "include"
)
foreach(_include_dir ${xrb_INCLUDE_DIRECTORIES})
    install(
        DIRECTORY ${_include_dir}
        DESTINATION include/lib
        FILES_MATCHING PATTERN "*.hpp"
    )
endforeach()
install(
    DIRECTORY ${XRB_RESOURCES_DIR}
    DESTINATION .
)
# This causes automatically-generated export targets to be defined during the install step in
#   ${CMAKE_INSTALL_PREFIX}/cmake/xrb-export.cmake
# which will be called by ${CMAKE_INSTALL_PREFIX}/xrb-config.cmake (see next step with
# configure_file and install).
install(
    EXPORT xrbRelease
    CONFIGURATIONS Release
    NAMESPACE xrb::
    DESTINATION cmake
    FILE xrb-export.cmake
)
install(
    EXPORT xrbDebug
    CONFIGURATIONS Debug
    NAMESPACE xrb::
    DESTINATION cmake
    FILE xrb-export.cmake
)
# Generate the file xrb-config.cmake that will be installed with the `make install` steps.
# NOTE: This is distinct from the xrb-config.cmake file that will be generated in
# ${PROJECT_BINARY_DIR} by the export command below.
configure_file(${CMAKE_SOURCE_DIR}/cmake/xrb-config.cmake.in ${PROJECT_BINARY_DIR}/cmake/for_install_dir/xrb-config.cmake @ONLY)
# Install rule for the xrb-config.cmake file just generated.  It goes in the root directory
# of the installation, and is the file that cmake looks for and uses in find_package.
install(
    FILES ${PROJECT_BINARY_DIR}/cmake/for_install_dir/xrb-config.cmake
    DESTINATION .
)
# Generate the version configuration file.  This is the preferred method for indicating the presence
# of a library and conveying version information to the find_package cmake command.
configure_file(${CMAKE_SOURCE_DIR}/cmake/GenericConfigVersion.cmake.in ${PROJECT_BINARY_DIR}/xrbConfigVersion.cmake @ONLY)
# Install rule for the version configuration file.  It should go in the root dir of the install.
install(
    FILES ${PROJECT_BINARY_DIR}/xrbConfigVersion.cmake
    DESTINATION .
)
