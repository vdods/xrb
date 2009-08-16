// ///////////////////////////////////////////////////////////////////////////
// xrb_sdlpal.cpp by Victor Dods, created 2009/08/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_sdlpal.hpp"

#include "SDL.h"

namespace Xrb
{

Pal::Status SDLPal::Initialize ()
{
    // initialize video (no parachute so we get core dumps)
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE) < 0)
    {
        fprintf(stderr, "SDLPal::Initialize(); unable to initialize SDL.  error: %s\n", SDL_GetError());
        return FAILURE;
    }

    // set a window title (i dunno what the icon string is)
    SDL_WM_SetCaption("TODO: change me", "icon thingy");

    return SUCCESS;
}

void SDLPal::Shutdown ()
{
    // make sure input isn't grabbed
    SDL_WM_GrabInput(SDL_GRAB_OFF);
    // call SDL's cleanup func
    SDL_Quit();
}

Pal::Status SDLPal::InitializeVideo (Uint16 width, Uint16 height, Uint8 bit_depth, bool fullscreen)
{
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    Uint32 video_mode_flags = SDL_OPENGL;
    if (fullscreen)
        video_mode_flags |= SDL_FULLSCREEN;

    if (NULL == SDL_SetVideoMode(width, height, bit_depth, video_mode_flags))
    {
        fprintf(stderr, "SDLPal::InitializeVideo(); could not set the requested video mode\n");
        return FAILURE;
    }

    return SUCCESS;
}

void SDLPal::ShutdownVideo ()
{
    // nothing needs to be done here for SDL
}

Uint32 SDLPal::CurrentTimeInMilliseconds ()
{
    return SDL_GetTicks();
}

void SDLPal::SleepForMilliseconds (Uint32 milliseconds_to_sleep)
{
    SDL_Delay(milliseconds_to_sleep);
}

void SDLPal::FinishFrame ()
{
    SDL_GL_SwapBuffers();
}

Event *SDLPal::PollEvent ()
{
    // TODO
    return NULL;
}

Texture *SDLPal::LoadImage (char const *image_path)
{
    return NULL; // TODO
}

Pal::Status SDLPal::SaveImage (char const *image_path, Texture const &texture)
{
    return FAILURE; // TODO
}

} // end of namespace Xrb
