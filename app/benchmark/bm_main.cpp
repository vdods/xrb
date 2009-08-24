// ///////////////////////////////////////////////////////////////////////////
// bm_main.cpp by Victor Dods, created 2009/08/23
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb.hpp"

#include "bm_commandlineoptions.hpp"
#include "bm_config.hpp"
#include "bm_master.hpp"
#include "xrb_screen.hpp"
#include "xrb_sdlpal.hpp"

using namespace std;
using namespace Xrb;

#define CONFIG_FILE_PATH "benchmark.config"

Bm::Config g_config;

// shuts down our Pal and the singletons.
void CleanUp ()
{
    Singleton::Pal().Shutdown();
    Singleton::Shutdown();
}

int main (int argc, char **argv)
{
    fprintf(stderr, "\nmain();\n");

    {
        // read in the user's config file (video resolution, key binds, etc).
        g_config.Read(CONFIG_FILE_PATH);

        // initialize the commandline options with the config values and then
        // parse the commandline into the options object.
        Bm::CommandLineOptions options(argv[0]);
        options.InitializeFullscreen(g_config.Boolean(Bm::VIDEO__FULLSCREEN));
        options.InitializeResolution(g_config.Resolution());
        options.InitializeKeyMapName(g_config.GetString(Bm::SYSTEM__KEY_MAP_NAME));
        options.Parse(argc, argv);
        if (!options.ParseSucceeded() || options.IsHelpRequested())
        {
            options.PrintHelpMessage(cerr);
            return options.ParseSucceeded() ? 0 : 1;
        }

        Singleton::Initialize(SDLPal::Create, options.KeyMapName().c_str());

        // register on-exit function, which will be called after main() has returned.
        atexit(CleanUp);

        if (Singleton::Pal().Initialize() != Pal::SUCCESS)
            return 1;

        Singleton::Pal().SetWindowCaption("XRB Benchmark");

        // init the screen
        Screen *screen = Screen::Create(
            options.Resolution()[Dim::X],
            options.Resolution()[Dim::Y],
            32,
            options.Fullscreen());
        if (screen == NULL)
            return 2;

        // create and run the game
        {
            Bm::Master master(screen);
            master.Run();
        }

        Delete(screen);

        // write the config file back out (because it may have changed during
        // the execution of the game.
        g_config.Write(CONFIG_FILE_PATH);
    }

    // return with no error condition
    return 0;
}
