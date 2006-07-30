// ///////////////////////////////////////////////////////////////////////////
// xrb_singletons.h by Victor Dods, created 2005/02/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_SINGLETONS_H_)
#define _XRB_SINGLETONS_H_

// this header is included by xrb.h

// this header declares all the singletons which behave essentially as
// "devices".  e.g. the keyboard/mouse button input, the resource manager.
// (possibly move the screen into this category).

struct FT_LibraryRec_;

namespace Xrb
{

class Input;
class KeyMap;
class ResourceLibrary;

/** Contains functions Initialize and Shutdown which should be the first and
  * last things done, respectively, by the @c main function.  The singleton
  * objects are set up and shut down by these functions.  The singletons can
  * then be accessed via calls to the respective accessor functions.
  * @brief Controls the singleton objects which can be thought of as devices.
  */
namespace Singletons
{
    /** @brief Returns a reference to the Input singleton object.
      */
    Input &Input ();
    /** @brief Returns a const reference to the KeyMap singleton object.
      */
    KeyMap const &KeyMap ();
    /** @brief Returns a reference to the ResourceLibrary singleton object.
      */
    ResourceLibrary &ResourceLibrary ();
    /** @brief Returns a pointer to the FreeType library singleton object.
      */
    FT_LibraryRec_ *const FTLibrary ();

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
      * @param key_map_name Specifies the name of the KeyMap to use.  Must
      *                     not be @c NULL.  See @ref KeyMap::Create for valid
      *                     arguments.
      */
    void Initialize (char const *key_map_name);
    /** This should be the last thing done, so that proper cleanup is
      * performed for the singleton objects.
      * @brief Shuts down the singleton objects.
      */
    void Shutdown ();
} // end of namespace Singletons

} // end of namespace Xrb

#endif // !defined(_XRB_SINGLETONS_H_)

