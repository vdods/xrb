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

// #include "png.h"
#include "SDL.h"
#include "SDL_image.h"
#include "xrb_event.hpp"
#include "xrb_input_events.hpp"
#include "xrb_inputstate.hpp"
#include "xrb_key.hpp"
#include "xrb_screen.hpp"
#include "xrb_texture.hpp"

/*
#if defined(WORDS_BIGENDIAN)
    #define SDL_RMASK 0xFF000000
    #define SDL_GMASK 0x00FF0000
    #define SDL_BMASK 0x0000FF00
    #define SDL_AMASK 0x000000FF
#else // !defined(WORDS_BIGENDIAN)
    #define SDL_RMASK 0x000000FF
    #define SDL_GMASK 0x0000FF00
    #define SDL_BMASK 0x00FF0000
    #define SDL_AMASK 0xFF000000
#endif // !defined(WORDS_BIGENDIAN)
*/

namespace Xrb
{

namespace {

Key::Code TranslateSDLKey (SDLKey sdl_key)
{
    static Key::Code s_key_lookup[SDLK_LAST];
    static bool s_key_lookup_is_initialized = false;
    if (!s_key_lookup_is_initialized)
    {
        for (Sint32 i = 0; i < SDLK_LAST; ++i)
            s_key_lookup[i] = Key::INVALID;

        s_key_lookup[SDLK_UNKNOWN] = Key::UNKNOWN;
        s_key_lookup[SDL_BUTTON_LEFT] = Key::LEFTMOUSE;
        s_key_lookup[SDL_BUTTON_MIDDLE] = Key::MIDDLEMOUSE;
        s_key_lookup[SDL_BUTTON_RIGHT] = Key::RIGHTMOUSE;
        s_key_lookup[SDL_BUTTON_WHEELUP] = Key::MOUSEWHEELUP;
        s_key_lookup[SDL_BUTTON_WHEELDOWN] = Key::MOUSEWHEELDOWN;
        s_key_lookup[SDLK_BACKSPACE] = Key::BACKSPACE;
        s_key_lookup[SDLK_TAB] = Key::TAB;
        s_key_lookup[SDLK_CLEAR] = Key::CLEAR;
        s_key_lookup[SDLK_RETURN] = Key::RETURN;
        s_key_lookup[SDLK_PAUSE] = Key::PAUSE;
        s_key_lookup[SDLK_ESCAPE] = Key::ESCAPE;
        s_key_lookup[SDLK_SPACE] = Key::SPACE;
        s_key_lookup[SDLK_QUOTE] = Key::QUOTE;
        s_key_lookup[SDLK_COMMA] = Key::COMMA;
        s_key_lookup[SDLK_MINUS] = Key::MINUS;
        s_key_lookup[SDLK_PERIOD] = Key::PERIOD;
        s_key_lookup[SDLK_SLASH] = Key::SLASH;
        s_key_lookup[SDLK_0] = Key::ZERO;
        s_key_lookup[SDLK_1] = Key::ONE;
        s_key_lookup[SDLK_2] = Key::TWO;
        s_key_lookup[SDLK_3] = Key::THREE;
        s_key_lookup[SDLK_4] = Key::FOUR;
        s_key_lookup[SDLK_5] = Key::FIVE;
        s_key_lookup[SDLK_6] = Key::SIX;
        s_key_lookup[SDLK_7] = Key::SEVEN;
        s_key_lookup[SDLK_8] = Key::EIGHT;
        s_key_lookup[SDLK_9] = Key::NINE;
        s_key_lookup[SDLK_SEMICOLON] = Key::SEMICOLON;
        s_key_lookup[SDLK_EQUALS] = Key::EQUALS;
        s_key_lookup[SDLK_LEFTBRACKET] = Key::LEFTBRACKET;
        s_key_lookup[SDLK_BACKSLASH] = Key::BACKSLASH;
        s_key_lookup[SDLK_RIGHTBRACKET] = Key::RIGHTBRACKET;
        s_key_lookup[SDLK_BACKQUOTE] = Key::BACKQUOTE;
        s_key_lookup[SDLK_a] = Key::A;
        s_key_lookup[SDLK_b] = Key::B;
        s_key_lookup[SDLK_c] = Key::C;
        s_key_lookup[SDLK_d] = Key::D;
        s_key_lookup[SDLK_e] = Key::E;
        s_key_lookup[SDLK_f] = Key::F;
        s_key_lookup[SDLK_g] = Key::G;
        s_key_lookup[SDLK_h] = Key::H;
        s_key_lookup[SDLK_i] = Key::I;
        s_key_lookup[SDLK_j] = Key::J;
        s_key_lookup[SDLK_k] = Key::K;
        s_key_lookup[SDLK_l] = Key::L;
        s_key_lookup[SDLK_m] = Key::M;
        s_key_lookup[SDLK_n] = Key::N;
        s_key_lookup[SDLK_o] = Key::O;
        s_key_lookup[SDLK_p] = Key::P;
        s_key_lookup[SDLK_q] = Key::Q;
        s_key_lookup[SDLK_r] = Key::R;
        s_key_lookup[SDLK_s] = Key::S;
        s_key_lookup[SDLK_t] = Key::T;
        s_key_lookup[SDLK_u] = Key::U;
        s_key_lookup[SDLK_v] = Key::V;
        s_key_lookup[SDLK_w] = Key::W;
        s_key_lookup[SDLK_x] = Key::X;
        s_key_lookup[SDLK_y] = Key::Y;
        s_key_lookup[SDLK_z] = Key::Z;
        s_key_lookup[SDLK_DELETE] = Key::DELETE;
        s_key_lookup[SDLK_WORLD_0] = Key::WORLD_0;
        s_key_lookup[SDLK_WORLD_1] = Key::WORLD_1;
        s_key_lookup[SDLK_WORLD_2] = Key::WORLD_2;
        s_key_lookup[SDLK_WORLD_3] = Key::WORLD_3;
        s_key_lookup[SDLK_WORLD_4] = Key::WORLD_4;
        s_key_lookup[SDLK_WORLD_5] = Key::WORLD_5;
        s_key_lookup[SDLK_WORLD_6] = Key::WORLD_6;
        s_key_lookup[SDLK_WORLD_7] = Key::WORLD_7;
        s_key_lookup[SDLK_WORLD_8] = Key::WORLD_8;
        s_key_lookup[SDLK_WORLD_9] = Key::WORLD_9;
        s_key_lookup[SDLK_WORLD_10] = Key::WORLD_10;
        s_key_lookup[SDLK_WORLD_11] = Key::WORLD_11;
        s_key_lookup[SDLK_WORLD_12] = Key::WORLD_12;
        s_key_lookup[SDLK_WORLD_13] = Key::WORLD_13;
        s_key_lookup[SDLK_WORLD_14] = Key::WORLD_14;
        s_key_lookup[SDLK_WORLD_15] = Key::WORLD_15;
        s_key_lookup[SDLK_WORLD_16] = Key::WORLD_16;
        s_key_lookup[SDLK_WORLD_17] = Key::WORLD_17;
        s_key_lookup[SDLK_WORLD_18] = Key::WORLD_18;
        s_key_lookup[SDLK_WORLD_19] = Key::WORLD_19;
        s_key_lookup[SDLK_WORLD_20] = Key::WORLD_20;
        s_key_lookup[SDLK_WORLD_21] = Key::WORLD_21;
        s_key_lookup[SDLK_WORLD_22] = Key::WORLD_22;
        s_key_lookup[SDLK_WORLD_23] = Key::WORLD_23;
        s_key_lookup[SDLK_WORLD_24] = Key::WORLD_24;
        s_key_lookup[SDLK_WORLD_25] = Key::WORLD_25;
        s_key_lookup[SDLK_WORLD_26] = Key::WORLD_26;
        s_key_lookup[SDLK_WORLD_27] = Key::WORLD_27;
        s_key_lookup[SDLK_WORLD_28] = Key::WORLD_28;
        s_key_lookup[SDLK_WORLD_29] = Key::WORLD_29;
        s_key_lookup[SDLK_WORLD_30] = Key::WORLD_30;
        s_key_lookup[SDLK_WORLD_31] = Key::WORLD_31;
        s_key_lookup[SDLK_WORLD_32] = Key::WORLD_32;
        s_key_lookup[SDLK_WORLD_33] = Key::WORLD_33;
        s_key_lookup[SDLK_WORLD_34] = Key::WORLD_34;
        s_key_lookup[SDLK_WORLD_35] = Key::WORLD_35;
        s_key_lookup[SDLK_WORLD_36] = Key::WORLD_36;
        s_key_lookup[SDLK_WORLD_37] = Key::WORLD_37;
        s_key_lookup[SDLK_WORLD_38] = Key::WORLD_38;
        s_key_lookup[SDLK_WORLD_39] = Key::WORLD_39;
        s_key_lookup[SDLK_WORLD_40] = Key::WORLD_40;
        s_key_lookup[SDLK_WORLD_41] = Key::WORLD_41;
        s_key_lookup[SDLK_WORLD_42] = Key::WORLD_42;
        s_key_lookup[SDLK_WORLD_43] = Key::WORLD_43;
        s_key_lookup[SDLK_WORLD_44] = Key::WORLD_44;
        s_key_lookup[SDLK_WORLD_45] = Key::WORLD_45;
        s_key_lookup[SDLK_WORLD_46] = Key::WORLD_46;
        s_key_lookup[SDLK_WORLD_47] = Key::WORLD_47;
        s_key_lookup[SDLK_WORLD_48] = Key::WORLD_48;
        s_key_lookup[SDLK_WORLD_49] = Key::WORLD_49;
        s_key_lookup[SDLK_WORLD_50] = Key::WORLD_50;
        s_key_lookup[SDLK_WORLD_51] = Key::WORLD_51;
        s_key_lookup[SDLK_WORLD_52] = Key::WORLD_52;
        s_key_lookup[SDLK_WORLD_53] = Key::WORLD_53;
        s_key_lookup[SDLK_WORLD_54] = Key::WORLD_54;
        s_key_lookup[SDLK_WORLD_55] = Key::WORLD_55;
        s_key_lookup[SDLK_WORLD_56] = Key::WORLD_56;
        s_key_lookup[SDLK_WORLD_57] = Key::WORLD_57;
        s_key_lookup[SDLK_WORLD_58] = Key::WORLD_58;
        s_key_lookup[SDLK_WORLD_59] = Key::WORLD_59;
        s_key_lookup[SDLK_WORLD_60] = Key::WORLD_60;
        s_key_lookup[SDLK_WORLD_61] = Key::WORLD_61;
        s_key_lookup[SDLK_WORLD_62] = Key::WORLD_62;
        s_key_lookup[SDLK_WORLD_63] = Key::WORLD_63;
        s_key_lookup[SDLK_WORLD_64] = Key::WORLD_64;
        s_key_lookup[SDLK_WORLD_65] = Key::WORLD_65;
        s_key_lookup[SDLK_WORLD_66] = Key::WORLD_66;
        s_key_lookup[SDLK_WORLD_67] = Key::WORLD_67;
        s_key_lookup[SDLK_WORLD_68] = Key::WORLD_68;
        s_key_lookup[SDLK_WORLD_69] = Key::WORLD_69;
        s_key_lookup[SDLK_WORLD_70] = Key::WORLD_70;
        s_key_lookup[SDLK_WORLD_71] = Key::WORLD_71;
        s_key_lookup[SDLK_WORLD_72] = Key::WORLD_72;
        s_key_lookup[SDLK_WORLD_73] = Key::WORLD_73;
        s_key_lookup[SDLK_WORLD_74] = Key::WORLD_74;
        s_key_lookup[SDLK_WORLD_75] = Key::WORLD_75;
        s_key_lookup[SDLK_WORLD_76] = Key::WORLD_76;
        s_key_lookup[SDLK_WORLD_77] = Key::WORLD_77;
        s_key_lookup[SDLK_WORLD_78] = Key::WORLD_78;
        s_key_lookup[SDLK_WORLD_79] = Key::WORLD_79;
        s_key_lookup[SDLK_WORLD_80] = Key::WORLD_80;
        s_key_lookup[SDLK_WORLD_81] = Key::WORLD_81;
        s_key_lookup[SDLK_WORLD_82] = Key::WORLD_82;
        s_key_lookup[SDLK_WORLD_83] = Key::WORLD_83;
        s_key_lookup[SDLK_WORLD_84] = Key::WORLD_84;
        s_key_lookup[SDLK_WORLD_85] = Key::WORLD_85;
        s_key_lookup[SDLK_WORLD_86] = Key::WORLD_86;
        s_key_lookup[SDLK_WORLD_87] = Key::WORLD_87;
        s_key_lookup[SDLK_WORLD_88] = Key::WORLD_88;
        s_key_lookup[SDLK_WORLD_89] = Key::WORLD_89;
        s_key_lookup[SDLK_WORLD_90] = Key::WORLD_90;
        s_key_lookup[SDLK_WORLD_91] = Key::WORLD_91;
        s_key_lookup[SDLK_WORLD_92] = Key::WORLD_92;
        s_key_lookup[SDLK_WORLD_93] = Key::WORLD_93;
        s_key_lookup[SDLK_WORLD_94] = Key::WORLD_94;
        s_key_lookup[SDLK_WORLD_95] = Key::WORLD_95;
        s_key_lookup[SDLK_KP0] = Key::KP0;
        s_key_lookup[SDLK_KP1] = Key::KP1;
        s_key_lookup[SDLK_KP2] = Key::KP2;
        s_key_lookup[SDLK_KP3] = Key::KP3;
        s_key_lookup[SDLK_KP4] = Key::KP4;
        s_key_lookup[SDLK_KP5] = Key::KP5;
        s_key_lookup[SDLK_KP6] = Key::KP6;
        s_key_lookup[SDLK_KP7] = Key::KP7;
        s_key_lookup[SDLK_KP8] = Key::KP8;
        s_key_lookup[SDLK_KP9] = Key::KP9;
        s_key_lookup[SDLK_KP_PERIOD] = Key::KP_PERIOD;
        s_key_lookup[SDLK_KP_DIVIDE] = Key::KP_DIVIDE;
        s_key_lookup[SDLK_KP_MULTIPLY] = Key::KP_MULTIPLY;
        s_key_lookup[SDLK_KP_MINUS] = Key::KP_MINUS;
        s_key_lookup[SDLK_KP_PLUS] = Key::KP_PLUS;
        s_key_lookup[SDLK_KP_ENTER] = Key::KP_ENTER;
        s_key_lookup[SDLK_KP_EQUALS] = Key::KP_EQUALS;
        s_key_lookup[SDLK_UP] = Key::UP;
        s_key_lookup[SDLK_DOWN] = Key::DOWN;
        s_key_lookup[SDLK_RIGHT] = Key::RIGHT;
        s_key_lookup[SDLK_LEFT] = Key::LEFT;
        s_key_lookup[SDLK_INSERT] = Key::INSERT;
        s_key_lookup[SDLK_HOME] = Key::HOME;
        s_key_lookup[SDLK_END] = Key::END;
        s_key_lookup[SDLK_PAGEUP] = Key::PAGEUP;
        s_key_lookup[SDLK_PAGEDOWN] = Key::PAGEDOWN;
        s_key_lookup[SDLK_F1] = Key::F1;
        s_key_lookup[SDLK_F2] = Key::F2;
        s_key_lookup[SDLK_F3] = Key::F3;
        s_key_lookup[SDLK_F4] = Key::F4;
        s_key_lookup[SDLK_F5] = Key::F5;
        s_key_lookup[SDLK_F6] = Key::F6;
        s_key_lookup[SDLK_F7] = Key::F7;
        s_key_lookup[SDLK_F8] = Key::F8;
        s_key_lookup[SDLK_F9] = Key::F9;
        s_key_lookup[SDLK_F10] = Key::F10;
        s_key_lookup[SDLK_F11] = Key::F11;
        s_key_lookup[SDLK_F12] = Key::F12;
        s_key_lookup[SDLK_F13] = Key::F13;
        s_key_lookup[SDLK_F14] = Key::F14;
        s_key_lookup[SDLK_F15] = Key::F15;
        s_key_lookup[SDLK_NUMLOCK] = Key::NUMLOCK;
        s_key_lookup[SDLK_CAPSLOCK] = Key::CAPSLOCK;
        s_key_lookup[SDLK_SCROLLOCK] = Key::SCROLLLOCK;
        s_key_lookup[SDLK_RSHIFT] = Key::RSHIFT;
        s_key_lookup[SDLK_LSHIFT] = Key::LSHIFT;
        s_key_lookup[SDLK_RCTRL] = Key::RCTRL;
        s_key_lookup[SDLK_LCTRL] = Key::LCTRL;
        s_key_lookup[SDLK_RALT] = Key::RALT;
        s_key_lookup[SDLK_LALT] = Key::LALT;
        s_key_lookup[SDLK_RMETA] = Key::RMETA;
        s_key_lookup[SDLK_LMETA] = Key::LMETA;
        s_key_lookup[SDLK_LSUPER] = Key::LWINBLOWS;
        s_key_lookup[SDLK_RSUPER] = Key::RWINBLOWS;
        s_key_lookup[SDLK_MODE] = Key::MODE;
        s_key_lookup[SDLK_COMPOSE] = Key::COMPOSE;
        s_key_lookup[SDLK_HELP] = Key::HELP;
        s_key_lookup[SDLK_PRINT] = Key::PRINT;
        s_key_lookup[SDLK_SYSREQ] = Key::SYSREQ;
        s_key_lookup[SDLK_BREAK] = Key::BREAK;
        s_key_lookup[SDLK_MENU] = Key::MENU;
        s_key_lookup[SDLK_POWER] = Key::POWER;
        s_key_lookup[SDLK_EURO] = Key::EURO;
        s_key_lookup[SDLK_UNDO] = Key::UNDO;
    }

    ASSERT1(sdl_key >= 0 && sdl_key < SDLK_LAST && "SDLKey out of range");
    ASSERT1(s_key_lookup[sdl_key] != Key::INVALID && "invalid SDLKey");
    return s_key_lookup[sdl_key];
}

Key::Modifier TranslateSDLMod (SDLMod sdl_mod)
{
    static SDLMod const s_all = SDLMod(KMOD_LSHIFT|KMOD_RSHIFT|KMOD_LCTRL|KMOD_RCTRL|KMOD_LALT|KMOD_RALT|KMOD_LMETA|KMOD_RMETA|KMOD_NUM|KMOD_CAPS|KMOD_MODE);
    ASSERT1((sdl_mod & ~s_all) == 0 && "invalid SDLMod");

    Uint32 retval = 0;
    if ((sdl_mod & KMOD_LSHIFT) != 0) retval |= Key::MOD_LSHIFT;
    if ((sdl_mod & KMOD_RSHIFT) != 0) retval |= Key::MOD_RSHIFT;
    if ((sdl_mod & KMOD_LCTRL)  != 0) retval |= Key::MOD_LCTRL;
    if ((sdl_mod & KMOD_RCTRL)  != 0) retval |= Key::MOD_RCTRL;
    if ((sdl_mod & KMOD_LALT)   != 0) retval |= Key::MOD_LALT;
    if ((sdl_mod & KMOD_RALT)   != 0) retval |= Key::MOD_RALT;
    if ((sdl_mod & KMOD_LMETA)  != 0) retval |= Key::MOD_LMETA;
    if ((sdl_mod & KMOD_RMETA)  != 0) retval |= Key::MOD_RMETA;
    if ((sdl_mod & KMOD_NUM)    != 0) retval |= Key::MOD_NUM;
    if ((sdl_mod & KMOD_CAPS)   != 0) retval |= Key::MOD_CAPS;
    if ((sdl_mod & KMOD_MODE)   != 0) retval |= Key::MOD_MODE;
    return Key::Modifier(retval);
}

} // end of unnamed namespace

Pal *SDLPal::Create ()
{
    return new SDLPal();
}

Pal::Status SDLPal::Initialize ()
{
    // initialize video (no parachute so we get core dumps)
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE) < 0)
    {
        fprintf(stderr, "SDLPal::Initialize(); unable to initialize SDL.  error: %s\n", SDL_GetError());
        return FAILURE;
    }

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

    SDL_Surface *surface = SDL_SetVideoMode(width, height, bit_depth, video_mode_flags);
    if (surface == NULL)
    {
        fprintf(stderr, "SDLPal::InitializeVideo(); could not set the requested video mode\n");
        return FAILURE;
    }

    return SUCCESS;
}

void SDLPal::ShutdownVideo ()
{
    // nothing needs to be done here -- the SDL_Surface created in
    // InitializeVideo will be freed by SDL_Quit or any successive
    // call to SDL_SetVideoMode.

    // though, SDL_QuitSubSystem(SDL_VIDEO) might be appropriate
}

void SDLPal::SetWindowCaption (char const *window_caption)
{
    ASSERT1(window_caption != NULL);
    // set a window title (the second parameter doesn't seem to do anything)
    SDL_WM_SetCaption(window_caption, "");
}

Uint32 SDLPal::CurrentTime ()
{
    return SDL_GetTicks();
}

void SDLPal::Sleep (Uint32 milliseconds_to_sleep)
{
    SDL_Delay(milliseconds_to_sleep);
}

void SDLPal::FinishFrame ()
{
    SDL_GL_SwapBuffers();
}

Event *SDLPal::PollEvent (Screen const *screen, Float time)
{
    ASSERT1(screen != NULL);
    ASSERT1(time >= 0.0);

    // loop until we've constructed an Xrb::Event, or until there are
    // no SDL events in the event queue.  this weird loop is necessary
    // because some SDL events will be ignored, and returning NULL
    // from this method indicates that the event queue is empty, so we'll
    // have to keep polling until we get a non-ignored event.
    Event *retval = NULL;
    SDL_Event e;
    while (retval == NULL && SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
            case SDL_KEYDOWN:
                retval = new EventKeyDown(
                    TranslateSDLKey(e.key.keysym.sym),
                    TranslateSDLMod(e.key.keysym.mod),
                    time);
                break;

            case SDL_KEYUP:
                retval = new EventKeyUp(
                    TranslateSDLKey(e.key.keysym.sym),
                    TranslateSDLMod(e.key.keysym.mod),
                    time);
                break;

            case SDL_MOUSEBUTTONDOWN:
            {
                SDL_MouseButtonEvent const &mouse_button_event = e.button;
                ScreenCoordVector2 event_position(
                    static_cast<ScreenCoord>(mouse_button_event.x),
                    screen->ScreenRect().Height() - static_cast<ScreenCoord>(mouse_button_event.y));
                if (mouse_button_event.button == SDL_BUTTON_WHEELUP ||
                    mouse_button_event.button == SDL_BUTTON_WHEELDOWN)
                {
                    retval = new EventMouseWheel(
                        TranslateSDLKey(SDLKey(mouse_button_event.button)),
                        event_position,
                        Singleton::InputState().Modifier(),
                        time);
                }
                else
                {
                    retval = new EventMouseButtonDown(
                        TranslateSDLKey(SDLKey(mouse_button_event.button)),
                        event_position,
                        Singleton::InputState().Modifier(),
                        time);
                }
                break;
            }

            case SDL_MOUSEBUTTONUP:
            {
                SDL_MouseButtonEvent const &mouse_button_event = e.button;
                ScreenCoordVector2 event_position(
                    static_cast<ScreenCoord>(mouse_button_event.x),
                    screen->ScreenRect().Height() - static_cast<ScreenCoord>(mouse_button_event.y));
                if (mouse_button_event.button == SDL_BUTTON_WHEELUP ||
                    mouse_button_event.button == SDL_BUTTON_WHEELDOWN)
                {
                    // mousewheel button-up events are superfluous
                    retval = NULL;
                }
                else
                {
                    retval = new EventMouseButtonUp(
                        TranslateSDLKey(SDLKey(mouse_button_event.button)),
                        event_position,
                        Singleton::InputState().Modifier(),
                        time);
                }
                break;
            }

            case SDL_MOUSEMOTION:
            {
                SDL_MouseMotionEvent const &mouse_motion_event = e.motion;
                ScreenCoordVector2 event_position(
                    static_cast<ScreenCoord>(mouse_motion_event.x),
                    screen->ScreenRect().Height() - static_cast<ScreenCoord>(mouse_motion_event.y));
                // SDL provides right-handed coords, so yrel coord is negated
                // to get left-handed coords (used by Screen)
                ScreenCoordVector2 event_delta(
                    mouse_motion_event.xrel,
                    -mouse_motion_event.yrel); 
                retval = new EventMouseMotion(
                    Singleton::InputState().IsKeyPressed(Key::LEFTMOUSE),
                    Singleton::InputState().IsKeyPressed(Key::MIDDLEMOUSE),
                    Singleton::InputState().IsKeyPressed(Key::RIGHTMOUSE),
                    event_position,
                    event_delta,
                    Singleton::InputState().Modifier(),
                    time);
                break;
            }

            case SDL_JOYAXISMOTION:
            case SDL_JOYBALLMOTION:
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
            case SDL_JOYHATMOTION:
                // forget it for now
                break;

            case SDL_QUIT:
                retval = new EventQuit(time);
                break;

            case SDL_ACTIVEEVENT:
            case SDL_SYSWMEVENT:
            case SDL_VIDEORESIZE:
            case SDL_VIDEOEXPOSE:
                // ignore these events
                break;

            case SDL_USEREVENT:
                ASSERT1(false &&
                        "Bad! BAD human! you shouldn't be making "
                        "SDL_USEREVENTs.  Create a subclass "
                        "of EventCustom instead.");
                break;

            default:
                ASSERT1(false && "Unknown event type");
                break;
        }
    }

    // this should only return NULL if the event queue is empty and we didn't
    // encounter any relevant events.
    return retval;
}

/*
// TODO: change to not use SDL_Surface (use Texture directly)
SDL_Surface *Texture::LoadPNG (char const *image_path)
{
    // the code in this function is based on the code from example.c
    // in the libpng documentation.

    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    FILE *fp;

    if ((fp = fopen(image_path, "rb")) == NULL)
    {
        fprintf(stderr, "Texture::LoadPNG(\"%s\"); error opening file\n", image_path);
        return NULL;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        fclose(fp);
        fprintf(stderr, "Texture::LoadPNG(\"%s\"); error reading PNG file\n", image_path);
        return NULL;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        fprintf(stderr, "Texture::LoadPNG(\"%s\"); error reading PNG file\n", image_path);
        return NULL;
    }

    // Set error handling if you are using the setjmp/longjmp method (this is
    // the normal method of doing things with libpng).  REQUIRED unless you
    // set up your own error handlers in the png_create_read_struct() earlier.
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        fprintf(stderr, "Texture::LoadPNG(\"%s\"); error reading PNG file\n", image_path);
        return NULL;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, sig_read);
    png_read_info(png_ptr, info_ptr);

    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, int_p_NULL, int_p_NULL);

    // create the SDL_Surface
    SDL_Surface *surface = SDL_CreateRGBSurface(0, width, height, 32, SDL_RMASK, SDL_GMASK, SDL_BMASK, SDL_AMASK);
    if (surface == NULL)
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        fprintf(stderr, "Texture::LoadPNG(\"%s\"); error creating SDL_Surface\n", image_path);
        return NULL;
    }

    // Expand paletted colors into true RGB triplets
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    // Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_gray_1_2_4_to_8(png_ptr);

    // Expand paletted or RGB images with transparency to full alpha channels
    // so the data will be available as RGBA quartets.
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    png_read_update_info(png_ptr, info_ptr);
    png_bytep *row_pointers = (png_bytepp)png_malloc(png_ptr, height*sizeof(png_bytep));
    for (Uint32 row = 0; row < height; row++)
        row_pointers[row] = png_bytep(surface->pixels) + row*surface->pitch;

    png_read_image(png_ptr, row_pointers);
    png_read_end(png_ptr, info_ptr);
    // At this point you have read the entire image

    if (color_type != PNG_COLOR_TYPE_RGB && color_type != PNG_COLOR_TYPE_RGB_ALPHA)
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        SDL_FreeSurface(surface);
        fprintf(stderr, "Texture::LoadPNG(\"%s\"); unsupported PNG color type\n", image_path);
        return NULL;
    }

    // close the file and the png stuff
    fclose(fp);

    // now we're done with the png stuff
    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

    return surface;
}
*/

Texture *SDLPal::LoadImage (char const *image_path)
{
    SDL_Surface *surface = IMG_Load(image_path);
    if (surface == NULL)
    {
        fprintf(stderr, "SDLPal::LoadImage(); error while loading image \"%s\"\n", image_path);
        return NULL;
    }
    if (surface->format->BitsPerPixel != 32)
    {
        fprintf(stderr, "SDLPal::LoadImage(); unsupported bit depth: %d (32 bpp is the only supported depth)\n", int(surface->format->BitsPerPixel));
        return NULL;
    }

    Texture *retval = Texture::Create(ScreenCoordVector2(surface->w, surface->h), false);
    ASSERT1(retval != NULL);

    // copy the data from the surface to the Texture
    memcpy(retval->Data(), surface->pixels, retval->DataLength());

    return retval;
}

Pal::Status SDLPal::SaveImage (char const *image_path, Texture const &texture)
{
    return FAILURE; // TODO
}

} // end of namespace Xrb
