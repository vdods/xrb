// ///////////////////////////////////////////////////////////////////////////
// lesson00_main.cpp by Victor Dods, created 2006/07/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////


// ///////////////////////////////////////////////////////////////////////////
// Lesson 00 - Initializing And Shutting Down The Engine
// ///////////////////////////////////////////////////////////////////////////


/** @page lesson00 Lesson 00 - Initializing And Shutting Down The Engine
@code *//* @endcode
This lesson will show you how to initialize the basic systems needed by
XuqRijBuh -- initializing video mode and Singleton and shutting down.  If
you're reading the source file directly, instead of the doxygen-generated
document, then pay no attention to the "code" and "endcode" tags in each
comment.

    <ul>
    <li>@ref lesson00_main.cpp "This lesson's source code"</li>
    <li>@ref lessons "Main lesson index"</li>
    </ul>

<strong>Procedural Overview</strong>

    <ul>
    <li>Main function</li>
        <ul>
        <li>Initialization (video, sound, and whatever else is needed).</li>
        <li>Initialize game engine singletons (necessary for correct operation of the game engine).</li>
        <li>Create the Screen object.  This is what sets the video mode. </li>
        <li>Execute game-specific code.</li>
        <li>Delete the Screen object.  This does NOT reset the video mode.</li>
        <li>Shutdown game engine singletons (necessary for correct operation of the game engine).</li>
        <li>Shutdown (this is what resets the video mode).</li>
        </ul>
    </ul>

<strong>Code Diving!</strong>

To start off, the <tt>"xrb.hpp"</tt> header MUST be included in every source and
header file, as it contains definitions necessary for the correct usage and
operation of the game engine.
@code */
#include "xrb.hpp"

#include "xrb_screen.hpp" // For use of the necessary Screen widget class.
#include "xrb_sdlpal.hpp" // For use of the SDLPal platform abstraction layer.

/* @endcode
Every declaration in the game engine library is within the <tt>Xrb</tt>
namespace.  Although this is pedantic, it is used to avoid any possible naming
collision.  It turns out not to be inconvenient, because of C++'s
<tt>using</tt> keyword.  This <tt>using</tt> statement is used so we don't
need to qualify every library type/class/etc with <tt>Xrb::</tt>
@code */
using namespace Xrb;

// This is just a helper function to group all the shutdown code together.
void CleanUp ()
{
    std::cerr << "CleanUp();" << std::endl;

    // Shutdown the platform abstraction layer.
    Singleton::Pal().Shutdown();
    // Shutdown the game engine singletons.  This is necessary for the
    // game engine to shutdown cleanly.
    Singleton::Shutdown();
}

int main (int argc, char **argv)
{
    std::cerr << "main();" << std::endl;

    /* @endcode
    This call initializes the game engine singleton facilities.  This must
    be done, or the engine will just not work.  The singletons include:
        <ul>
        <li>ResourceLibrary ensures that we only load one copy of certain
            resources (textures, fonts, sounds, etc) into memory.</li>
        <li>InputState provides accessors for the immediate state of the keyboard
            and mouse (and eventually joysticks, etc).  This is not the
            primary or only means for user input, but we'll get to that
            later.</li>
        <li>KeyMap performs keyboard layout mapping (e.g. Dvorak), which is
            necessary only on Windows builds because the Windows version of
            SDL lacks proper key mapping.</li>
        <li>FTLibrary is used transparently by the font system to use the
            FreeType font rendering facilities (assuming SDLPal is in use).
            There's nothing to worry about.  Trust me.</li>
        </ul>
    @code */
    Singleton::Initialize(SDLPal::Create);

    /* @endcode
    Initialize the platform abstraction layer (Pal).  Then set the window caption.
    @code */
    if (Singleton::Pal().Initialize() != Pal::SUCCESS)
        return 1;

    Singleton::Pal().SetWindowCaption("XuqRijBuh Lesson 00");

    /* @endcode
    This call creates the Screen object and initializes the given video mode
    (800x600, 32 bit color).  There is no constraint on the size or aspect
    ratio of the screen, apart from the ability of your video hardware to
    handle it.  The Screen object is the root widget of the GUI widget
    hierarchy, and does a bunch of special handling to draw its child widgets
    properly.
    @code */
    Screen *screen = Screen::Create(
        800,    // video mode/screen width
        600,    // video mode/screen height
        32,     // video mode pixel bitdepth
        false); // not fullscreen -- for now
    /* @endcode
    If the Screen failed to initialize for whatever reason (probably because
    the system was unable to set the video mode), screen will be NULL.  If
    this happens, print an error message and quit with an error code.
    @code */
    if (screen == NULL)
    {
        std::cerr << "unable to initialize video mode" << std::endl;
        // this shuts down the Pal and singletons.
        CleanUp();
        // return with an error value.
        return 2;
    }

    /* @endcode
    Here is where the game code goes.  To avoid overloading your brain, for
    now we'll just pause for 5 seconds.
    @code */
    {
        std::cerr << "pausing for 5 seconds..." << std::endl;
        Singleton::Pal().Sleep(5.0f);
    }

    /* @endcode
    Delete the Screen object, and with it the entire GUI widget hierarchy.
    @code */
    Delete(screen);
    // this shuts down the Pal and the singletons.
    CleanUp();
    // return with success value.
    return 0;
}
/* @endcode

<strong>Exercises</strong>

    <ul>
    <li>Change the last parameter passed to Screen::Create -- currently
        <tt>false</tt> -- to <tt>true</tt>.  This will cause the
        application to be run in fullscreen mode.  Make sure not to use
        fullscreen mode while developing/debugging, because hitting a
        breakpoint while in fullscreen video mode will make your computer
        a touch ornery.</li>
    <li>With the last parameter to Screen::Create set to <tt>true</tt>
        (indicating fullscreen is enabled), change the video mode width,
        height and bitdepth to various values, seeing what video modes are
        supported by your hardware.</li>
    <li>Write code that parses the commandline arguments so you can specify
        what video mode width, height and bitdepth to use from the
        commandline.</li>
    </ul>

Thus concludes lesson00.  Did it hurt?  The fuck it didn't.
*/
