// ///////////////////////////////////////////////////////////////////////////
// xrb_singleton.cpp by Victor Dods, created 2005/02/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_singleton.hpp"

#include "xrb.hpp"

#include "xrb_gl.hpp"
#include "xrb_inputstate.hpp"
#include "xrb_key.hpp"
#include "xrb_keymap.hpp"
#include "xrb_pal.hpp"
#include "xrb_resourcelibrary.hpp"

namespace Xrb
{

namespace
{
    // KeyBind singleton -- this handles keyboard input (and mouse buttons too)
    InputState *g_inputstate = NULL;
    // KeyMap singleton -- for alternate keyboard layouts in WIN32 (necessary
    // due to WIN32 SDL's lack of support for alternate keyboard layouts).
    KeyMap const *g_key_map = NULL;
    // Platform-specific instantiation of the Pal interface.
    Pal *g_pal = NULL;
    // ResourceLibrary singleton -- loads and manages reference counted assets
    ResourceLibrary *g_resource_library = NULL;
    // Gl singleton -- mainly exists to implement texture atlases
    Gl *g_gl = NULL;

    // indicates if Singleton::Initialize has been called
    bool g_is_initialized = false;
} // end of namespace

Gl &Singleton::Gl ()
{
    ASSERT1(g_is_initialized && "can't use Singleton::Gl() before Singleton::Initialize()");
    ASSERT1(g_gl != NULL);
    return *g_gl;
}

InputState &Singleton::InputState ()
{
    ASSERT1(g_is_initialized && "can't use Singleton::InputState() before Singleton::Initialize()");
    ASSERT1(g_inputstate != NULL);
    return *g_inputstate;
}

KeyMap const &Singleton::KeyMap ()
{
    ASSERT1(g_is_initialized && "can't use Singleton::KeyMap() before Singleton::Initialize()");
    ASSERT1(g_key_map != NULL);
    return *g_key_map;
}

Pal &Singleton::Pal ()
{
    ASSERT1(g_is_initialized && "can't use Singleton::Pal() before Singleton::Initialize()");
    ASSERT1(g_pal != NULL);
    return *g_pal;
}

ResourceLibrary &Singleton::ResourceLibrary ()
{
    ASSERT1(g_is_initialized && "can't use Singleton::ResourceLibrary() before Singleton::Initialize()");
    ASSERT1(g_resource_library != NULL);
    return *g_resource_library;
}

void Singleton::Initialize (PalFactory CreatePal, char const *const key_map_name)
{
    ASSERT1(key_map_name != NULL);

    ASSERT1(!g_is_initialized);

    fprintf(stderr, "Singleton::Initialize();\n");
    fprintf(stderr, "    sizeof(bool) = %d\n", Sint32(sizeof(bool)));

    g_pal = CreatePal();
    g_inputstate = new Xrb::InputState();
    g_key_map = Xrb::KeyMap::Create(key_map_name);
    g_resource_library = new Xrb::ResourceLibrary();

    ASSERT0(g_pal != NULL && "CreatePal() returned NULL");
//     fprintf(stderr, "\tattempting to use KeyMap \"%s\", got \"%s\"\n", key_map_name, g_key_map->Name().c_str());
    ASSERT0(g_key_map != NULL && "failed to create KeyMap");

    g_is_initialized = true;
}

void Singleton::Shutdown ()
{
    ASSERT1(g_is_initialized);
    ASSERT1(g_inputstate != NULL);
    ASSERT1(g_key_map != NULL);
    ASSERT1(g_resource_library != NULL);

    fprintf(stderr, "Singleton::Shutdown();\n");

    ASSERT1(g_gl == NULL && "you must Singleton::ShutdownGl() first (Screen takes care of this -- maybe you didn't delete your Screen object");

    // shutdown in reverse order as init

    DeleteAndNullify(g_inputstate);
    DeleteAndNullify(g_resource_library);
    DeleteAndNullify(g_key_map);
    DeleteAndNullify(g_pal);

    g_is_initialized = false;
}

void Singleton::InitializeGl ()
{
    ASSERT1(g_gl == NULL && "can't InitializeGl() twice in a row");
    g_gl = new Xrb::Gl();
    // the Gl singleton is now initialized, so calls to Singleton::Gl() can
    // be made.  this is critical for certain operations to work (e.g.
    // creation of the opaque white texture in the Gl singleton).
    g_gl->FinishInitialization();
}

void Singleton::ShutdownGl ()
{
    ASSERT1(g_gl != NULL && "can't ShutdownGl() twice in a row");
    DeleteAndNullify(g_gl);
}

} // end of namespace Xrb
