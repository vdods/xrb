cmake_minimum_required(VERSION 3.0) # I had problems if I only specified "VERSION 3".

project(xrb VERSION 0.5)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})

if(NOT XRB_RESOURCES_DIR)
    set(XRB_RESOURCES_DIR "XRB_RESOURCES_DIR-NOTFOUND" CACHE STRING "Path to XRB `resources` directory, the zipped contents of which can be found at http://thedods.com/victor/resources.zip")
    message(FATAL_ERROR "You must set a valid path for XRB_RESOURCES_DIR, which should be the path to XRB `resources` directory, the zipped contents of which can be found at http://thedods.com/victor/resources.zip")
endif()

if(NOT XRB_DEBUG_LEVEL)
    set(XRB_DEBUG_LEVEL "0" CACHE STRING "Debug level for xrb code -- can be 0 (no asserts), 1, 2, or 3 (all asserts)." FORCE)
endif()

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

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    list(APPEND CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu")
endif()

find_package(SDL 1.2 REQUIRED)
find_package(Freetype 2 REQUIRED)
find_package(PNG 1.2 REQUIRED)
find_package(OpenGL 1.2 REQUIRED)

mark_as_advanced(SDL_LIBRARY SDL_INCLUDE_DIR SDLMAIN_LIBRARY)

###################################################################################################
# xrb target
###################################################################################################

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

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU") # "GNU" is GCC
    target_compile_options(
        xrb PRIVATE
        -Wall -Werror -Wno-reorder -Wno-unused-variable -Wno-overloaded-virtual -Wno-unused-private-field
    )
    target_compile_options(
        xrb PUBLIC
        -DXRB_DEBUG_LEVEL=${XRB_DEBUG_LEVEL}
    )
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
    target_compile_options(
        xrb PRIVATE
        /Wall /WX # /WX is to treat warnings as errors.
    )
    target_compile_options(
        xrb PUBLIC
        /DXRB_DEBUG_LEVEL=${XRB_DEBUG_LEVEL}
    )
endif()

target_include_directories(
    xrb PUBLIC
    ${FREETYPE_INCLUDE_DIR_freetype2}
    ${FREETYPE_INCLUDE_DIR_ftbuild}
    ${PNG_PNG_INCLUDE_DIR}
    ${SDL_INCLUDE_DIR}
)

# if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # This is the correct way to detect Mac OS X operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
#     target_link_libraries(xrb PUBLIC Freetype "-framework OpenGL" PNG SDL SDLmain "-framework Cocoa")
target_link_libraries(
    xrb PUBLIC
    ${FREETYPE_LIBRARY}
    ${OPENGL_gl_LIBRARY}
    ${OPENGL_glu_LIBRARY}
    ${PNG_LIBRARY_RELEASE}
    ${SDL_LIBRARY}
)
# elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#     target_link_libraries(xrb PUBLIC freetype GL png16 SDL SDLmain)
# else() # Other systems, e.g. Windows
#     message(FATAL_ERROR "TODO: Figure out target_link_libraries for this platform.")
# endif()

###################################################################################################
# Global compiler options (and related) go here.
###################################################################################################

# TODO: look into using target_compile_definitions and target_compile_options,
# though these seem sufficiently global to warrant the extreme measure of
# setting CMAKE_CXX_FLAGS.

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
    DIRECTORY ${PROJECT_BINARY_DIR}/${XRB_RESOURCES_DIR}
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
configure_file(${CMAKE_SOURCE_DIR}/cmake/GenericConfigVersion.cmake.in ${PROJECT_BINARY_DIR}/xrb-config-version.cmake @ONLY)
# Install rule for the version configuration file.  It should go in the root dir of the install.
install(
    FILES ${PROJECT_BINARY_DIR}/xrb-config-version.cmake
    DESTINATION .
)
