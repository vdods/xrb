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
// Lesson 00 - initializing and shutting down the engine
// ///////////////////////////////////////////////////////////////////////////


// this header MUST be included in every source/header file, as it contains
// definitions necessary for the correct usage and operation of libxrb.
#include "xrb.h"

// for use of the Screen class.
#include "xrb_screen.h"

// this using statement is useful so that we don't need to qualify every
// library type/class/etc with Xrb::
using namespace Xrb;

// cleans stuff up.  see below
void CleanUp ()
{
    fprintf(stderr, "CleanUp();\n");

    // shutdown the singletons
    Singletons::Shutdown();
    // make sure the application doesn't still have the mouse grabbed,
    // or you'll have a hard time pointy-clickying at stuff.
    SDL_WM_GrabInput(SDL_GRAB_OFF);
    // call SDL's cleanup function.
    SDL_Quit();
}

int main (int argc, char **argv)
{
    fprintf(stderr, "main();\n");

    // first attempt to initialize SDL.  currently this just involves
    // initializing the video, but will later initialize sound, once
    // sound code is actually written.  the SDL_INIT_NOPARACHUTE flag
    // is used because we want crashes that will be useful in debug mode
    // (meaning, in unix, it will core dump).
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE) < 0)
    {
        fprintf(stderr, "unable to initialize video.  error: %s\n", SDL_GetError());
        // return with an error value.
        return 1;
    }

    // this call initializes libxrb's singleton facilities.  this must
    // be done, or the engine will just not work.  the singletons include:
    // - ResourceLibrary: ensures that we only load one copy of certain
    //                    resources (textures, fonts, sounds, etc) into memory.
    // - Input: accessor for the immediate state of the keyboard and mouse
    //          (and eventually joysticks, etc).  this is not the primary/only
    //          means for user input, but we'll get to that later.
    // - FTLibrary: this is used by the font system to use the FreeType font
    //              rendering facilities.  you shouldn't need to worry about it.
    Singletons::Initialize();

    // set the caption for the application's window.  i haven't figured out
    // what the icon string is, maybe it's supposed to be the filename for
    // a BMP file or something.
    SDL_WM_SetCaption("XuqRijBuh Lesson 00", "icon thingy");

    // this call creates the Screen object and initializes the given video
    // mode (1024x768, 32 bit color).  there is no constraint on the size
    // or aspect ratio of the screen, apart from the ability of your video
    // hardware to handle it.  the Screen object is the root widget of the
    // GUI widget hierarchy, and does a bunch of special handling to draw
    // its child widgets properly.  depending on the value of the FULLSCREEN
    // macro, set fullscreen or not.
    Screen *screen = Screen::Create(
        1024,                   // video mode/screen width
        768,                    // video mode/screen height
        32,                     // video mode pixel bitdepth
        /*SDL_FULLSCREEN*/ 0);  // SDL_SetVideomode flags.  use SDL_FULLSCREEN
                                // instead of 0 to set fullscreen video mode.
    // if the Screen failed to initialize for whatever reason (probably because
    // the system was unable to set the video mode), screen will be NULL.  if
    // this happens, print an error message and quit with an error code.
    if (screen == NULL)
    {
        fprintf(stderr, "unable to initialize video mode\n");
        // this shuts down libxrb's singletons, and shuts down SDL.
        CleanUp();
        // return with an error value.
        return 2;
    }

    // here is where the game code goes.  for now we'll just pause for 5 seconds.
    {
        fprintf(stderr, "pausing for 5000 milliseconds...\n");
        SDL_Delay(5000);
    }

    // delete the Screen object, and with it the entire GUI widget hierarchy.
    // this call doesn't reset the video mode however, that is done by
    // calling SDL_Quit(), which we have stashed away in CleanUp().
    Delete(screen);

    // this shuts down libxrb's singletons, and shuts down SDL.
    CleanUp();
    // return with success value.
    return 0;
}

