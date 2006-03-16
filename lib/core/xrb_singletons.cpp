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
#include "xrb_keybinds.h"
#include "xrb_resourcelibrary.h"

namespace Xrb
{

namespace
{
    // KeyBind singleton -- this handles keyboard input (and mouse buttons too)
    KeyBinds g_key_binds;

    // ResourceLibrary singleton -- loads and manages reference counted assets
    ResourceLibrary g_resource_library;

    // FreeType library singleton -- handles rendering font glyphs.
    // this pointer type should be equivalent to type FT_Library.
    FT_LibraryRec_ *g_ft_library;

    // indicates if Singletons::Initialize has been called
    bool g_is_initialized = false;
} // end of namespace

KeyBinds *const Singletons::KeyBinds ()
{
    ASSERT1(g_is_initialized)
    return &g_key_binds;
}

ResourceLibrary *const Singletons::ResourceLibrary ()
{
    ASSERT1(g_is_initialized)
    return &g_resource_library;
}

FT_LibraryRec_ *const Singletons::FTLibrary ()
{
    ASSERT1(g_is_initialized)
    return g_ft_library;
}

void Singletons::Initialize ()
{
    ASSERT1(!g_is_initialized)

    g_ft_library = NULL;
    FT_Error error = FT_Init_FreeType(&g_ft_library);
    ASSERT0(error == 0 && "The FreeType library failed to initialize")

    g_is_initialized = true;
}

void Singletons::Shutdown ()
{
    ASSERT1(g_is_initialized)
    ASSERT1(g_ft_library != NULL)

    FT_Done_FreeType(g_ft_library);

    g_is_initialized = false;
}

} // end of namespace Xrb
