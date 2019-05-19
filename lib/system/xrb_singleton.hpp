// ///////////////////////////////////////////////////////////////////////////
// xrb_singleton.hpp by Victor Dods, created 2005/02/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_SINGLETON_HPP_)
#define _XRB_SINGLETON_HPP_

// this header is included by xrb.h

// this header declares all the singletons which behave essentially as
// "devices".  e.g. the keyboard/mouse button input, the resource manager.
// (possibly move the screen into this category).

namespace Xrb {

class FileSystem;
class Gl;
class InputState;
class KeyMap;
class Pal;
class ResourceLibrary;

typedef Pal *(*PalFactory)();

/// @brief Controls the singleton objects which can be thought of as devices.
/// @details Contains functions Initialize and Shutdown which should be the first and
/// last things done, respectively, by the @c main function.  The singleton
/// objects are set up and shut down by these functions.  The singletons can
/// then be accessed via calls to the respective accessor functions.
namespace Singleton {

    /// FileSystem singleton -- controls the search path for platform-abstracted
    /// file/directory organization and provides means to "mod" resources.
    extern Xrb::FileSystem *g_file_system;
    /// KeyBind singleton -- this handles keyboard input (and mouse buttons too).
    /// @note Not for use anywhere outside of the Singleton namespace.
    extern Xrb::InputState *g_input_state;
    /// KeyMap singleton -- for alternate keyboard layouts in WIN32 (necessary
    /// due to WIN32 SDL's lack of support for alternate keyboard layouts).
    /// @note Not for use anywhere outside of the Singleton namespace.
    extern Xrb::KeyMap const *g_key_map;
    /// Platform-specific instantiation of the Pal interface.
    /// @note Not for use anywhere outside of the Singleton namespace.
    extern Xrb::Pal *g_pal;
    /// ResourceLibrary singleton -- loads and manages reference counted assets
    /// @note Not for use anywhere outside of the Singleton namespace.
    extern Xrb::ResourceLibrary *g_resource_library;
    /// Gl singleton -- mainly exists to implement texture atlases
    /// @note Not for use anywhere outside of the Singleton namespace.
    extern Xrb::Gl *g_gl;

    /// Returns the FileSystem singleton.
    inline Xrb::FileSystem &FileSystem ()
    {
        ASSERT1(g_file_system != NULL && "FileSystem singleton not initialized");
        return *g_file_system;
    }
    /// Returns the Gl singleton.
    inline Xrb::Gl &Gl ()
    {
        ASSERT1(g_gl != NULL && "Gl singleton not initialized");
        return *g_gl;
    }
    /// Returns the InputState singleton.
    inline Xrb::InputState &InputState ()
    {
        ASSERT1(g_input_state != NULL && "InputState singleton not initialized");
        return *g_input_state;
    }
    /// Returns the KeyMap singleton.
    inline Xrb::KeyMap const &KeyMap ()
    {
        ASSERT1(g_key_map != NULL && "KeyMap singleton not initialized");
        return *g_key_map;
    }
    /// Returns the Pal singleton.
    inline Xrb::Pal &Pal ()
    {
        ASSERT1(g_pal != NULL && "Pal singleton not initialized");
        return *g_pal;
    }
    /// Returns the ResourceLibrary singleton.
    inline Xrb::ResourceLibrary &ResourceLibrary ()
    {
        ASSERT1(g_resource_library != NULL && "ResourceLibrary singleton not initialized");
        return *g_resource_library;
    }

    /// @brief Initializes the singleton objects.
    /// @param CreatePal Specifies the platform-specific factory function for creating an
    /// instance of Pal.  The returned object will be deleted during Shutdown.
    /// @details This should be the first thing done, because other systems require
    /// the usage of the singleton objects.  If this function is not called
    /// before other systems activate, the singleton object accessors will
    /// assert.
    void Initialize (PalFactory CreatePal);
    /// @brief Shuts down the singleton objects.
    /// @details This should be the last thing done, so that proper cleanup is performed for the singleton objects.
    void Shutdown ();

    /// @brief Reinitialize the KeyMap singleton using the given keymap name.
    /// @param key_map_name Specifies the name of the KeyMap to use.  Must not be @c NULL.  See @ref KeyMap::Create for valid arguments.
    /// @details Useful when the keymap name comes from a config file and therefore has to be changed.  @c key_map_name can be specified
    /// to perform key mapping to supplement the windows-version-of-SDL's lack of alternate keyboard layout support (e.g. Dvorak).
    /// This is not necessary in Unix (it is currently unknown if it is necessary for Mac OS).
    void ReinitializeKeyMap (char const *key_map_name);
    
    /// @brief Initializes the openGL-controlling singleton.
    /// @details This is done automatically by Screen.
    void InitializeGl ();
    /// @brief Shuts down the openGL-controlling singleton.
    /// @details This is done automatically by Screen.
    void ShutdownGl ();

} // end of namespace Singleton

} // end of namespace Xrb

#endif // !defined(_XRB_SINGLETON_HPP_)

