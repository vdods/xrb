// ///////////////////////////////////////////////////////////////////////////
// xrb_singletons.cpp by Victor Dods, created 2005/02/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_singletons.h"

#include "xrb.h"

#include "ft2build.h"
#include FT_FREETYPE_H
#include "xrb_input.h"
#include "xrb_keymap.h"
#include "xrb_resourcelibrary.h"

namespace Xrb
{

namespace
{
    // KeyBind singleton -- this handles keyboard input (and mouse buttons too)
    Input *g_input = NULL;
    // KeyMap singleton -- for alternate keyboard layouts in WIN32 (necessary
    // due to WIN32 SDL's lack of support for alternate keyboard layouts).
    KeyMap const *g_key_map = NULL;
    // ResourceLibrary singleton -- loads and manages reference counted assets
    ResourceLibrary *g_resource_library = NULL;
    // FreeType library singleton -- handles rendering font glyphs.
    // this pointer type should be equivalent to type FT_Library.
    FT_LibraryRec_ *g_ft_library = NULL;

    // indicates if Singletons::Initialize has been called
    bool g_is_initialized = false;
} // end of namespace

Input &Singletons::Input ()
{
    ASSERT1(g_is_initialized)
    ASSERT1(g_input != NULL)
    return *g_input;
}

KeyMap const &Singletons::KeyMap ()
{
    ASSERT1(g_is_initialized)
    ASSERT1(g_key_map != NULL)
    return *g_key_map;
}

ResourceLibrary &Singletons::ResourceLibrary ()
{
    ASSERT1(g_is_initialized)
    ASSERT1(g_resource_library != NULL)
    return *g_resource_library;
}

FT_LibraryRec_ *const Singletons::FTLibrary ()
{
    ASSERT1(g_is_initialized)
    ASSERT1(g_ft_library != NULL)
    return g_ft_library;
}

void Singletons::Initialize (Xrb::KeyMap const *const key_map)
{
    ASSERT1(!g_is_initialized)

    fprintf(stderr, "Singletons::Initialize();\n");

    g_input = new Xrb::Input();

    g_key_map = (key_map != NULL) ? key_map : new KeyMapIdentity();
    fprintf(stderr, "\tusing %s\n", g_key_map->GetName().c_str());

    g_resource_library = new Xrb::ResourceLibrary();

    g_ft_library = NULL;
    FT_Error error = FT_Init_FreeType(&g_ft_library);
    ASSERT0(error == 0 && "The FreeType library failed to initialize")

    g_is_initialized = true;
}

void Singletons::Shutdown ()
{
    ASSERT1(g_is_initialized)
    ASSERT1(g_input != NULL)
    ASSERT1(g_key_map != NULL)
    ASSERT1(g_resource_library != NULL)
    ASSERT1(g_ft_library != NULL)

    fprintf(stderr, "Singletons::Shutdown();\n");

    DeleteAndNullify(g_input);
    DeleteAndNullify(g_resource_library);
    DeleteAndNullify(g_key_map);
    FT_Done_FreeType(g_ft_library);
    g_ft_library = NULL;

    g_is_initialized = false;
}

} // end of namespace Xrb
