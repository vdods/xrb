// ///////////////////////////////////////////////////////////////////////////
// xrb_singleton.cpp by Victor Dods, created 2005/02/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb.hpp" // xrb_singleton.hpp is included here

#include "xrb_gl.hpp"
#include "xrb_inputstate.hpp"
#include "xrb_key.hpp"
#include "xrb_keymap.hpp"
#include "xrb_pal.hpp"
#include "xrb_resourcelibrary.hpp"

namespace Xrb {
namespace Singleton {

Xrb::InputState *g_inputstate = NULL;
Xrb::KeyMap const *g_key_map = NULL;
Xrb::Pal *g_pal = NULL;
Xrb::ResourceLibrary *g_resource_library = NULL;
Xrb::Gl *g_gl = NULL;

void Initialize (PalFactory CreatePal, char const *const key_map_name)
{
    ASSERT1(key_map_name != NULL);

    fprintf(stderr, "Singleton::Initialize();\n");
    fprintf(stderr, "    sizeof(bool) = %d\n", Sint32(sizeof(bool)));

    ASSERT1(g_pal == NULL);
    ASSERT1(g_inputstate == NULL);
    ASSERT1(g_key_map == NULL);
    ASSERT1(g_resource_library == NULL);

    g_pal = CreatePal();
    g_inputstate = new Xrb::InputState();
    g_key_map = Xrb::KeyMap::Create(key_map_name);
    g_resource_library = new Xrb::ResourceLibrary();

    ASSERT0(g_pal != NULL && "CreatePal() returned NULL");
//     fprintf(stderr, "\tattempting to use KeyMap \"%s\", got \"%s\"\n", key_map_name, g_key_map->Name().c_str());
    ASSERT0(g_inputstate != NULL && "failed to create InputState");
    ASSERT0(g_key_map != NULL && "failed to create KeyMap");
    ASSERT0(g_resource_library != NULL && "failed to created ResourceLibrary");
}

void Shutdown ()
{
    ASSERT1(g_pal != NULL);
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
}

void InitializeGl ()
{
    ASSERT1(g_gl == NULL && "can't InitializeGl() twice in a row");
    g_gl = new Xrb::Gl();
    // the Gl singleton is now initialized, so calls to Singleton::Gl() can
    // be made.  this is critical for certain operations to work (e.g.
    // creation of the opaque white texture in the Gl singleton).
    g_gl->FinishInitialization();
}

void ShutdownGl ()
{
    ASSERT1(g_gl != NULL && "can't ShutdownGl() twice in a row");
    DeleteAndNullify(g_gl);
}

} // end of namespace Singleton
} // end of namespace Xrb
