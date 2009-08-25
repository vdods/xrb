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

namespace Xrb
{

class Gl;
class InputState;
class KeyMap;
class Pal;
class ResourceLibrary;

typedef Pal *(*PalFactory)();

/** Contains functions Initialize and Shutdown which should be the first and
  * last things done, respectively, by the @c main function.  The singleton
  * objects are set up and shut down by these functions.  The singletons can
  * then be accessed via calls to the respective accessor functions.
  * @brief Controls the singleton objects which can be thought of as devices.
  */
namespace Singleton
{
    /** @brief Returns a reference to the Gl singleton object.
      */
    Gl &Gl ();
    /** @brief Returns a reference to the InputState singleton object.
      */
    InputState &InputState ();
    /** @brief Returns a const reference to the KeyMap singleton object.
      */
    KeyMap const &KeyMap ();
    /** @brief Returns a reference to the Platform-specific singleton instance
      *        of the Pal interface.
      */
    Pal &Pal ();
    /** @brief Returns a reference to the ResourceLibrary singleton object.
      */
    ResourceLibrary &ResourceLibrary ();

    /** This should be the first thing done, because other systems require
      * the usage of the singleton objects.  If this function is not called
      * before other systems activate, the singleton object accessors will
      * assert.
      *
      * @c key_map_name can be specified to perform key mapping to supplement
      * the windows-version-of-SDL's lack of alternate keyboard layout support
      * (e.g. Dvorak).  This is not necessary in Unix (it is currently unknown
      * if it is necessary for Mac OS).
      *
      * @brief Initializes the singleton objects.
      * @param CreatePal Specifies the platform-specific factory function
      *                  for creating an instance of Pal.  The returned
      *                  object will be deleted during Shutdown.
      * @param key_map_name Specifies the name of the KeyMap to use.  Must
      *                     not be @c NULL.  See @ref KeyMap::Create for valid
      *                     arguments.
      */
    void Initialize (PalFactory CreatePal, char const *key_map_name);
    /** This should be the last thing done, so that proper cleanup is
      * performed for the singleton objects.
      * @brief Shuts down the singleton objects.
      */
    void Shutdown ();

    /** This is done automatically by Screen.
      * @brief Initializes the openGL-controlling singleton.
      */
    void InitializeGl ();
    /** This is done automatically by Screen.
      * @brief Shuts down the openGL-controlling singleton.
      */
    void ShutdownGl ();
} // end of namespace Singleton

} // end of namespace Xrb

#endif // !defined(_XRB_SINGLETON_HPP_)

