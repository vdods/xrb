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

#if XRB_PLATFORM == XRB_PLATFORM_SDL

#include "png.h"
#include "ft2build.h"  // the freetype stuff has to be included after png.h,
#include FT_FREETYPE_H // otherwise a very strange compile error occurs.
#include "xrb_asciifont.hpp"
#include "xrb_event.hpp"
#include "xrb_gl.hpp"
#include "xrb_input_events.hpp"
#include "xrb_inputstate.hpp"
#include "xrb_key.hpp"
#include "xrb_screen.hpp"
#include "xrb_texture.hpp"

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

namespace {

Xrb::Key::Code TranslateSDLKey (SDLKey sdl_key)
{
    static Xrb::Key::Code s_key_lookup[SDLK_LAST];
    static bool s_key_lookup_is_initialized = false;
    if (!s_key_lookup_is_initialized)
    {
        for (Xrb::Sint32 i = 0; i < SDLK_LAST; ++i)
            s_key_lookup[i] = Xrb::Key::INVALID;

        s_key_lookup[SDLK_UNKNOWN] = Xrb::Key::UNKNOWN;
        s_key_lookup[SDL_BUTTON_LEFT] = Xrb::Key::LEFTMOUSE;
        s_key_lookup[SDL_BUTTON_MIDDLE] = Xrb::Key::MIDDLEMOUSE;
        s_key_lookup[SDL_BUTTON_RIGHT] = Xrb::Key::RIGHTMOUSE;
        s_key_lookup[SDL_BUTTON_WHEELUP] = Xrb::Key::MOUSEWHEELUP;
        s_key_lookup[SDL_BUTTON_WHEELDOWN] = Xrb::Key::MOUSEWHEELDOWN;
        s_key_lookup[SDLK_BACKSPACE] = Xrb::Key::BACKSPACE;
        s_key_lookup[SDLK_TAB] = Xrb::Key::TAB;
        s_key_lookup[SDLK_CLEAR] = Xrb::Key::CLEAR;
        s_key_lookup[SDLK_RETURN] = Xrb::Key::RETURN;
        s_key_lookup[SDLK_PAUSE] = Xrb::Key::PAUSE;
        s_key_lookup[SDLK_ESCAPE] = Xrb::Key::ESCAPE;
        s_key_lookup[SDLK_SPACE] = Xrb::Key::SPACE;
        s_key_lookup[SDLK_QUOTE] = Xrb::Key::QUOTE;
        s_key_lookup[SDLK_COMMA] = Xrb::Key::COMMA;
        s_key_lookup[SDLK_MINUS] = Xrb::Key::MINUS;
        s_key_lookup[SDLK_PERIOD] = Xrb::Key::PERIOD;
        s_key_lookup[SDLK_SLASH] = Xrb::Key::SLASH;
        s_key_lookup[SDLK_0] = Xrb::Key::ZERO;
        s_key_lookup[SDLK_1] = Xrb::Key::ONE;
        s_key_lookup[SDLK_2] = Xrb::Key::TWO;
        s_key_lookup[SDLK_3] = Xrb::Key::THREE;
        s_key_lookup[SDLK_4] = Xrb::Key::FOUR;
        s_key_lookup[SDLK_5] = Xrb::Key::FIVE;
        s_key_lookup[SDLK_6] = Xrb::Key::SIX;
        s_key_lookup[SDLK_7] = Xrb::Key::SEVEN;
        s_key_lookup[SDLK_8] = Xrb::Key::EIGHT;
        s_key_lookup[SDLK_9] = Xrb::Key::NINE;
        s_key_lookup[SDLK_SEMICOLON] = Xrb::Key::SEMICOLON;
        s_key_lookup[SDLK_EQUALS] = Xrb::Key::EQUALS;
        s_key_lookup[SDLK_LEFTBRACKET] = Xrb::Key::LEFTBRACKET;
        s_key_lookup[SDLK_BACKSLASH] = Xrb::Key::BACKSLASH;
        s_key_lookup[SDLK_RIGHTBRACKET] = Xrb::Key::RIGHTBRACKET;
        s_key_lookup[SDLK_BACKQUOTE] = Xrb::Key::BACKQUOTE;
        s_key_lookup[SDLK_a] = Xrb::Key::A;
        s_key_lookup[SDLK_b] = Xrb::Key::B;
        s_key_lookup[SDLK_c] = Xrb::Key::C;
        s_key_lookup[SDLK_d] = Xrb::Key::D;
        s_key_lookup[SDLK_e] = Xrb::Key::E;
        s_key_lookup[SDLK_f] = Xrb::Key::F;
        s_key_lookup[SDLK_g] = Xrb::Key::G;
        s_key_lookup[SDLK_h] = Xrb::Key::H;
        s_key_lookup[SDLK_i] = Xrb::Key::I;
        s_key_lookup[SDLK_j] = Xrb::Key::J;
        s_key_lookup[SDLK_k] = Xrb::Key::K;
        s_key_lookup[SDLK_l] = Xrb::Key::L;
        s_key_lookup[SDLK_m] = Xrb::Key::M;
        s_key_lookup[SDLK_n] = Xrb::Key::N;
        s_key_lookup[SDLK_o] = Xrb::Key::O;
        s_key_lookup[SDLK_p] = Xrb::Key::P;
        s_key_lookup[SDLK_q] = Xrb::Key::Q;
        s_key_lookup[SDLK_r] = Xrb::Key::R;
        s_key_lookup[SDLK_s] = Xrb::Key::S;
        s_key_lookup[SDLK_t] = Xrb::Key::T;
        s_key_lookup[SDLK_u] = Xrb::Key::U;
        s_key_lookup[SDLK_v] = Xrb::Key::V;
        s_key_lookup[SDLK_w] = Xrb::Key::W;
        s_key_lookup[SDLK_x] = Xrb::Key::X;
        s_key_lookup[SDLK_y] = Xrb::Key::Y;
        s_key_lookup[SDLK_z] = Xrb::Key::Z;
        s_key_lookup[SDLK_DELETE] = Xrb::Key::DELETE;
        s_key_lookup[SDLK_WORLD_0] = Xrb::Key::WORLD_0;
        s_key_lookup[SDLK_WORLD_1] = Xrb::Key::WORLD_1;
        s_key_lookup[SDLK_WORLD_2] = Xrb::Key::WORLD_2;
        s_key_lookup[SDLK_WORLD_3] = Xrb::Key::WORLD_3;
        s_key_lookup[SDLK_WORLD_4] = Xrb::Key::WORLD_4;
        s_key_lookup[SDLK_WORLD_5] = Xrb::Key::WORLD_5;
        s_key_lookup[SDLK_WORLD_6] = Xrb::Key::WORLD_6;
        s_key_lookup[SDLK_WORLD_7] = Xrb::Key::WORLD_7;
        s_key_lookup[SDLK_WORLD_8] = Xrb::Key::WORLD_8;
        s_key_lookup[SDLK_WORLD_9] = Xrb::Key::WORLD_9;
        s_key_lookup[SDLK_WORLD_10] = Xrb::Key::WORLD_10;
        s_key_lookup[SDLK_WORLD_11] = Xrb::Key::WORLD_11;
        s_key_lookup[SDLK_WORLD_12] = Xrb::Key::WORLD_12;
        s_key_lookup[SDLK_WORLD_13] = Xrb::Key::WORLD_13;
        s_key_lookup[SDLK_WORLD_14] = Xrb::Key::WORLD_14;
        s_key_lookup[SDLK_WORLD_15] = Xrb::Key::WORLD_15;
        s_key_lookup[SDLK_WORLD_16] = Xrb::Key::WORLD_16;
        s_key_lookup[SDLK_WORLD_17] = Xrb::Key::WORLD_17;
        s_key_lookup[SDLK_WORLD_18] = Xrb::Key::WORLD_18;
        s_key_lookup[SDLK_WORLD_19] = Xrb::Key::WORLD_19;
        s_key_lookup[SDLK_WORLD_20] = Xrb::Key::WORLD_20;
        s_key_lookup[SDLK_WORLD_21] = Xrb::Key::WORLD_21;
        s_key_lookup[SDLK_WORLD_22] = Xrb::Key::WORLD_22;
        s_key_lookup[SDLK_WORLD_23] = Xrb::Key::WORLD_23;
        s_key_lookup[SDLK_WORLD_24] = Xrb::Key::WORLD_24;
        s_key_lookup[SDLK_WORLD_25] = Xrb::Key::WORLD_25;
        s_key_lookup[SDLK_WORLD_26] = Xrb::Key::WORLD_26;
        s_key_lookup[SDLK_WORLD_27] = Xrb::Key::WORLD_27;
        s_key_lookup[SDLK_WORLD_28] = Xrb::Key::WORLD_28;
        s_key_lookup[SDLK_WORLD_29] = Xrb::Key::WORLD_29;
        s_key_lookup[SDLK_WORLD_30] = Xrb::Key::WORLD_30;
        s_key_lookup[SDLK_WORLD_31] = Xrb::Key::WORLD_31;
        s_key_lookup[SDLK_WORLD_32] = Xrb::Key::WORLD_32;
        s_key_lookup[SDLK_WORLD_33] = Xrb::Key::WORLD_33;
        s_key_lookup[SDLK_WORLD_34] = Xrb::Key::WORLD_34;
        s_key_lookup[SDLK_WORLD_35] = Xrb::Key::WORLD_35;
        s_key_lookup[SDLK_WORLD_36] = Xrb::Key::WORLD_36;
        s_key_lookup[SDLK_WORLD_37] = Xrb::Key::WORLD_37;
        s_key_lookup[SDLK_WORLD_38] = Xrb::Key::WORLD_38;
        s_key_lookup[SDLK_WORLD_39] = Xrb::Key::WORLD_39;
        s_key_lookup[SDLK_WORLD_40] = Xrb::Key::WORLD_40;
        s_key_lookup[SDLK_WORLD_41] = Xrb::Key::WORLD_41;
        s_key_lookup[SDLK_WORLD_42] = Xrb::Key::WORLD_42;
        s_key_lookup[SDLK_WORLD_43] = Xrb::Key::WORLD_43;
        s_key_lookup[SDLK_WORLD_44] = Xrb::Key::WORLD_44;
        s_key_lookup[SDLK_WORLD_45] = Xrb::Key::WORLD_45;
        s_key_lookup[SDLK_WORLD_46] = Xrb::Key::WORLD_46;
        s_key_lookup[SDLK_WORLD_47] = Xrb::Key::WORLD_47;
        s_key_lookup[SDLK_WORLD_48] = Xrb::Key::WORLD_48;
        s_key_lookup[SDLK_WORLD_49] = Xrb::Key::WORLD_49;
        s_key_lookup[SDLK_WORLD_50] = Xrb::Key::WORLD_50;
        s_key_lookup[SDLK_WORLD_51] = Xrb::Key::WORLD_51;
        s_key_lookup[SDLK_WORLD_52] = Xrb::Key::WORLD_52;
        s_key_lookup[SDLK_WORLD_53] = Xrb::Key::WORLD_53;
        s_key_lookup[SDLK_WORLD_54] = Xrb::Key::WORLD_54;
        s_key_lookup[SDLK_WORLD_55] = Xrb::Key::WORLD_55;
        s_key_lookup[SDLK_WORLD_56] = Xrb::Key::WORLD_56;
        s_key_lookup[SDLK_WORLD_57] = Xrb::Key::WORLD_57;
        s_key_lookup[SDLK_WORLD_58] = Xrb::Key::WORLD_58;
        s_key_lookup[SDLK_WORLD_59] = Xrb::Key::WORLD_59;
        s_key_lookup[SDLK_WORLD_60] = Xrb::Key::WORLD_60;
        s_key_lookup[SDLK_WORLD_61] = Xrb::Key::WORLD_61;
        s_key_lookup[SDLK_WORLD_62] = Xrb::Key::WORLD_62;
        s_key_lookup[SDLK_WORLD_63] = Xrb::Key::WORLD_63;
        s_key_lookup[SDLK_WORLD_64] = Xrb::Key::WORLD_64;
        s_key_lookup[SDLK_WORLD_65] = Xrb::Key::WORLD_65;
        s_key_lookup[SDLK_WORLD_66] = Xrb::Key::WORLD_66;
        s_key_lookup[SDLK_WORLD_67] = Xrb::Key::WORLD_67;
        s_key_lookup[SDLK_WORLD_68] = Xrb::Key::WORLD_68;
        s_key_lookup[SDLK_WORLD_69] = Xrb::Key::WORLD_69;
        s_key_lookup[SDLK_WORLD_70] = Xrb::Key::WORLD_70;
        s_key_lookup[SDLK_WORLD_71] = Xrb::Key::WORLD_71;
        s_key_lookup[SDLK_WORLD_72] = Xrb::Key::WORLD_72;
        s_key_lookup[SDLK_WORLD_73] = Xrb::Key::WORLD_73;
        s_key_lookup[SDLK_WORLD_74] = Xrb::Key::WORLD_74;
        s_key_lookup[SDLK_WORLD_75] = Xrb::Key::WORLD_75;
        s_key_lookup[SDLK_WORLD_76] = Xrb::Key::WORLD_76;
        s_key_lookup[SDLK_WORLD_77] = Xrb::Key::WORLD_77;
        s_key_lookup[SDLK_WORLD_78] = Xrb::Key::WORLD_78;
        s_key_lookup[SDLK_WORLD_79] = Xrb::Key::WORLD_79;
        s_key_lookup[SDLK_WORLD_80] = Xrb::Key::WORLD_80;
        s_key_lookup[SDLK_WORLD_81] = Xrb::Key::WORLD_81;
        s_key_lookup[SDLK_WORLD_82] = Xrb::Key::WORLD_82;
        s_key_lookup[SDLK_WORLD_83] = Xrb::Key::WORLD_83;
        s_key_lookup[SDLK_WORLD_84] = Xrb::Key::WORLD_84;
        s_key_lookup[SDLK_WORLD_85] = Xrb::Key::WORLD_85;
        s_key_lookup[SDLK_WORLD_86] = Xrb::Key::WORLD_86;
        s_key_lookup[SDLK_WORLD_87] = Xrb::Key::WORLD_87;
        s_key_lookup[SDLK_WORLD_88] = Xrb::Key::WORLD_88;
        s_key_lookup[SDLK_WORLD_89] = Xrb::Key::WORLD_89;
        s_key_lookup[SDLK_WORLD_90] = Xrb::Key::WORLD_90;
        s_key_lookup[SDLK_WORLD_91] = Xrb::Key::WORLD_91;
        s_key_lookup[SDLK_WORLD_92] = Xrb::Key::WORLD_92;
        s_key_lookup[SDLK_WORLD_93] = Xrb::Key::WORLD_93;
        s_key_lookup[SDLK_WORLD_94] = Xrb::Key::WORLD_94;
        s_key_lookup[SDLK_WORLD_95] = Xrb::Key::WORLD_95;
        s_key_lookup[SDLK_KP0] = Xrb::Key::KP0;
        s_key_lookup[SDLK_KP1] = Xrb::Key::KP1;
        s_key_lookup[SDLK_KP2] = Xrb::Key::KP2;
        s_key_lookup[SDLK_KP3] = Xrb::Key::KP3;
        s_key_lookup[SDLK_KP4] = Xrb::Key::KP4;
        s_key_lookup[SDLK_KP5] = Xrb::Key::KP5;
        s_key_lookup[SDLK_KP6] = Xrb::Key::KP6;
        s_key_lookup[SDLK_KP7] = Xrb::Key::KP7;
        s_key_lookup[SDLK_KP8] = Xrb::Key::KP8;
        s_key_lookup[SDLK_KP9] = Xrb::Key::KP9;
        s_key_lookup[SDLK_KP_PERIOD] = Xrb::Key::KP_PERIOD;
        s_key_lookup[SDLK_KP_DIVIDE] = Xrb::Key::KP_DIVIDE;
        s_key_lookup[SDLK_KP_MULTIPLY] = Xrb::Key::KP_MULTIPLY;
        s_key_lookup[SDLK_KP_MINUS] = Xrb::Key::KP_MINUS;
        s_key_lookup[SDLK_KP_PLUS] = Xrb::Key::KP_PLUS;
        s_key_lookup[SDLK_KP_ENTER] = Xrb::Key::KP_ENTER;
        s_key_lookup[SDLK_KP_EQUALS] = Xrb::Key::KP_EQUALS;
        s_key_lookup[SDLK_UP] = Xrb::Key::UP;
        s_key_lookup[SDLK_DOWN] = Xrb::Key::DOWN;
        s_key_lookup[SDLK_RIGHT] = Xrb::Key::RIGHT;
        s_key_lookup[SDLK_LEFT] = Xrb::Key::LEFT;
        s_key_lookup[SDLK_INSERT] = Xrb::Key::INSERT;
        s_key_lookup[SDLK_HOME] = Xrb::Key::HOME;
        s_key_lookup[SDLK_END] = Xrb::Key::END;
        s_key_lookup[SDLK_PAGEUP] = Xrb::Key::PAGEUP;
        s_key_lookup[SDLK_PAGEDOWN] = Xrb::Key::PAGEDOWN;
        s_key_lookup[SDLK_F1] = Xrb::Key::F1;
        s_key_lookup[SDLK_F2] = Xrb::Key::F2;
        s_key_lookup[SDLK_F3] = Xrb::Key::F3;
        s_key_lookup[SDLK_F4] = Xrb::Key::F4;
        s_key_lookup[SDLK_F5] = Xrb::Key::F5;
        s_key_lookup[SDLK_F6] = Xrb::Key::F6;
        s_key_lookup[SDLK_F7] = Xrb::Key::F7;
        s_key_lookup[SDLK_F8] = Xrb::Key::F8;
        s_key_lookup[SDLK_F9] = Xrb::Key::F9;
        s_key_lookup[SDLK_F10] = Xrb::Key::F10;
        s_key_lookup[SDLK_F11] = Xrb::Key::F11;
        s_key_lookup[SDLK_F12] = Xrb::Key::F12;
        s_key_lookup[SDLK_F13] = Xrb::Key::F13;
        s_key_lookup[SDLK_F14] = Xrb::Key::F14;
        s_key_lookup[SDLK_F15] = Xrb::Key::F15;
        s_key_lookup[SDLK_NUMLOCK] = Xrb::Key::NUMLOCK;
        s_key_lookup[SDLK_CAPSLOCK] = Xrb::Key::CAPSLOCK;
        s_key_lookup[SDLK_SCROLLOCK] = Xrb::Key::SCROLLLOCK;
        s_key_lookup[SDLK_RSHIFT] = Xrb::Key::RSHIFT;
        s_key_lookup[SDLK_LSHIFT] = Xrb::Key::LSHIFT;
        s_key_lookup[SDLK_RCTRL] = Xrb::Key::RCTRL;
        s_key_lookup[SDLK_LCTRL] = Xrb::Key::LCTRL;
        s_key_lookup[SDLK_RALT] = Xrb::Key::RALT;
        s_key_lookup[SDLK_LALT] = Xrb::Key::LALT;
        s_key_lookup[SDLK_RMETA] = Xrb::Key::RMETA;
        s_key_lookup[SDLK_LMETA] = Xrb::Key::LMETA;
        s_key_lookup[SDLK_LSUPER] = Xrb::Key::LWINBLOWS;
        s_key_lookup[SDLK_RSUPER] = Xrb::Key::RWINBLOWS;
        s_key_lookup[SDLK_MODE] = Xrb::Key::MODE;
        s_key_lookup[SDLK_COMPOSE] = Xrb::Key::COMPOSE;
        s_key_lookup[SDLK_HELP] = Xrb::Key::HELP;
        s_key_lookup[SDLK_PRINT] = Xrb::Key::PRINT;
        s_key_lookup[SDLK_SYSREQ] = Xrb::Key::SYSREQ;
        s_key_lookup[SDLK_BREAK] = Xrb::Key::BREAK;
        s_key_lookup[SDLK_MENU] = Xrb::Key::MENU;
        s_key_lookup[SDLK_POWER] = Xrb::Key::POWER;
        s_key_lookup[SDLK_EURO] = Xrb::Key::EURO;
        s_key_lookup[SDLK_UNDO] = Xrb::Key::UNDO;
    }

    ASSERT1(sdl_key >= 0 && sdl_key < SDLK_LAST && "SDLKey out of range");
    ASSERT1(s_key_lookup[sdl_key] != Xrb::Key::INVALID && "invalid SDLKey");
    return s_key_lookup[sdl_key];
}

Xrb::Key::Modifier TranslateSDLMod (SDLMod sdl_mod)
{
    DEBUG1_CODE(static SDLMod const s_all = SDLMod(KMOD_LSHIFT|KMOD_RSHIFT|KMOD_LCTRL|KMOD_RCTRL|KMOD_LALT|KMOD_RALT|KMOD_LMETA|KMOD_RMETA|KMOD_NUM|KMOD_CAPS|KMOD_MODE);)
    ASSERT1((sdl_mod & ~s_all) == 0 && "invalid SDLMod");

    Xrb::Uint32 retval = 0;
    if ((sdl_mod & KMOD_LSHIFT) != 0) retval |= Xrb::Key::MOD_LSHIFT;
    if ((sdl_mod & KMOD_RSHIFT) != 0) retval |= Xrb::Key::MOD_RSHIFT;
    if ((sdl_mod & KMOD_LCTRL)  != 0) retval |= Xrb::Key::MOD_LCTRL;
    if ((sdl_mod & KMOD_RCTRL)  != 0) retval |= Xrb::Key::MOD_RCTRL;
    if ((sdl_mod & KMOD_LALT)   != 0) retval |= Xrb::Key::MOD_LALT;
    if ((sdl_mod & KMOD_RALT)   != 0) retval |= Xrb::Key::MOD_RALT;
    if ((sdl_mod & KMOD_LMETA)  != 0) retval |= Xrb::Key::MOD_LMETA;
    if ((sdl_mod & KMOD_RMETA)  != 0) retval |= Xrb::Key::MOD_RMETA;
    if ((sdl_mod & KMOD_NUM)    != 0) retval |= Xrb::Key::MOD_NUM;
    if ((sdl_mod & KMOD_CAPS)   != 0) retval |= Xrb::Key::MOD_CAPS;
    if ((sdl_mod & KMOD_MODE)   != 0) retval |= Xrb::Key::MOD_MODE;
    return Xrb::Key::Modifier(retval);
}

} // end of unnamed namespace

SDLPal::~SDLPal ()
{
    if (m_ft_library != NULL)
    {
        FT_Done_FreeType(m_ft_library);
        m_ft_library = NULL;
    }
}

Xrb::Pal *SDLPal::Create ()
{
    FT_LibraryRec_ *ft_library = NULL;
    FT_Error error = FT_Init_FreeType(&ft_library);
    if (error != 0)
    {
        fprintf(stderr, "SDLPal::Create(); the FreeType library failed to initialize\n");
        ft_library = NULL;
    }
    return new SDLPal(ft_library);
}

Xrb::Pal::Status SDLPal::Initialize ()
{
    fprintf(stderr, "SDLPal::Initialize();\n");

    // initialize video (no parachute so we get core dumps)
    if (SDL_Init(SDL_INIT_NOPARACHUTE) < 0)
    {
        fprintf(stderr, "SDLPal::Initialize(); unable to initialize SDL.  error: %s\n", SDL_GetError());
        return FAILURE;
    }

    return SUCCESS;
}

void SDLPal::Shutdown ()
{
    fprintf(stderr, "SDLPal::Shutdown();\n");

    // make sure input isn't grabbed
    SDL_WM_GrabInput(SDL_GRAB_OFF);
    // call SDL's cleanup func
    SDL_Quit();
}

Xrb::Pal::Status SDLPal::InitializeVideo (Xrb::Uint16 width, Xrb::Uint16 height, Xrb::Uint8 bit_depth, bool fullscreen)
{
    fprintf(stderr, "SDLPal::InitializeVideo();\n");

    SDL_InitSubSystem(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    Xrb::Uint32 video_mode_flags = SDL_OPENGL;
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
    fprintf(stderr, "SDLPal::ShutdownVideo();\n");

    // this should delete the SDL_Surface which was created by
    // SDL_VideoMode in InitializeVideo.
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void SDLPal::SetWindowCaption (char const *window_caption)
{
    fprintf(stderr, "SDLPal::SetWindowCaption(\"%s\");\n", window_caption);

    ASSERT1(window_caption != NULL);
    // set a window title (the second parameter doesn't seem to do anything)
    SDL_WM_SetCaption(window_caption, "");
}

Xrb::Uint32 SDLPal::CurrentTime ()
{
    return SDL_GetTicks();
}

void SDLPal::Sleep (Xrb::Uint32 milliseconds_to_sleep)
{
    SDL_Delay(milliseconds_to_sleep);
}

void SDLPal::FinishFrame ()
{
    SDL_GL_SwapBuffers();
}

Xrb::Event *SDLPal::PollEvent (Xrb::Screen const *screen, Xrb::Float time)
{
    ASSERT1(screen != NULL);
    ASSERT1(time >= 0.0);

    // loop until we've constructed an Xrb::Event, or until there are
    // no SDL events in the event queue.  this weird loop is necessary
    // because some SDL events will be ignored, and returning NULL
    // from this method indicates that the event queue is empty, so we'll
    // have to keep polling until we get a non-ignored event.
    Xrb::Event *retval = NULL;
    SDL_Event e;
    while (retval == NULL && SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
            case SDL_KEYDOWN:
                retval = new Xrb::EventKeyDown(
                    TranslateSDLKey(e.key.keysym.sym),
                    TranslateSDLMod(e.key.keysym.mod),
                    time);
                break;

            case SDL_KEYUP:
                retval = new Xrb::EventKeyUp(
                    TranslateSDLKey(e.key.keysym.sym),
                    TranslateSDLMod(e.key.keysym.mod),
                    time);
                break;

            case SDL_MOUSEBUTTONDOWN:
            {
                SDL_MouseButtonEvent const &mouse_button_event = e.button;
                Xrb::ScreenCoordVector2 event_position(
                    static_cast<Xrb::ScreenCoord>(mouse_button_event.x),
                    screen->ScreenRect().Height() - static_cast<Xrb::ScreenCoord>(mouse_button_event.y));
                if (mouse_button_event.button == SDL_BUTTON_WHEELUP ||
                    mouse_button_event.button == SDL_BUTTON_WHEELDOWN)
                {
                    retval = new Xrb::EventMouseWheel(
                        TranslateSDLKey(SDLKey(mouse_button_event.button)),
                        event_position,
                        Xrb::Singleton::InputState().Modifier(),
                        time);
                }
                else
                {
                    retval = new Xrb::EventMouseButtonDown(
                        TranslateSDLKey(SDLKey(mouse_button_event.button)),
                        event_position,
                        Xrb::Singleton::InputState().Modifier(),
                        time);
                }
                break;
            }

            case SDL_MOUSEBUTTONUP:
            {
                SDL_MouseButtonEvent const &mouse_button_event = e.button;
                Xrb::ScreenCoordVector2 event_position(
                    static_cast<Xrb::ScreenCoord>(mouse_button_event.x),
                    screen->ScreenRect().Height() - static_cast<Xrb::ScreenCoord>(mouse_button_event.y));
                if (mouse_button_event.button == SDL_BUTTON_WHEELUP ||
                    mouse_button_event.button == SDL_BUTTON_WHEELDOWN)
                {
                    // mousewheel button-up events are superfluous
                    retval = NULL;
                }
                else
                {
                    retval = new Xrb::EventMouseButtonUp(
                        TranslateSDLKey(SDLKey(mouse_button_event.button)),
                        event_position,
                        Xrb::Singleton::InputState().Modifier(),
                        time);
                }
                break;
            }

            case SDL_MOUSEMOTION:
            {
                SDL_MouseMotionEvent const &mouse_motion_event = e.motion;
                Xrb::ScreenCoordVector2 event_position(
                    static_cast<Xrb::ScreenCoord>(mouse_motion_event.x),
                    screen->ScreenRect().Height() - static_cast<Xrb::ScreenCoord>(mouse_motion_event.y));
                // SDL provides right-handed coords, so yrel coord is negated
                // to get left-handed coords (used by Screen)
                Xrb::ScreenCoordVector2 event_delta(
                    mouse_motion_event.xrel,
                    -mouse_motion_event.yrel); 
                retval = new Xrb::EventMouseMotion(
                    Xrb::Singleton::InputState().IsKeyPressed(Xrb::Key::LEFTMOUSE),
                    Xrb::Singleton::InputState().IsKeyPressed(Xrb::Key::MIDDLEMOUSE),
                    Xrb::Singleton::InputState().IsKeyPressed(Xrb::Key::RIGHTMOUSE),
                    event_position,
                    event_delta,
                    Xrb::Singleton::InputState().Modifier(),
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
                retval = new Xrb::EventQuit(time);
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

Xrb::Texture *SDLPal::LoadImage (char const *image_path)
{
//     fprintf(stderr, "SDLPal::LoadImage(\"%s\");\n", image_path);

    ASSERT1(image_path != NULL);

    // the code in this function is based on the code from example.c
    // in the libpng documentation.

    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    FILE *fp;

    if ((fp = fopen(image_path, "rb")) == NULL)
    {
        fprintf(stderr, "SDLPal::LoadImage(\"%s\"); error opening file\n", image_path);
        return NULL;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        fclose(fp);
        fprintf(stderr, "SDLPal::LoadImage(\"%s\"); error reading PNG file\n", image_path);
        return NULL;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        fprintf(stderr, "SDLPal::LoadImage(\"%s\"); error reading PNG file\n", image_path);
        return NULL;
    }

    // something about error handling
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        fprintf(stderr, "SDLPal::LoadImage(\"%s\"); error reading PNG file\n", image_path);
        return NULL;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, sig_read);
    png_read_info(png_ptr, info_ptr);

    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, int_p_NULL, int_p_NULL);

    // create the Texture
    Xrb::Texture *texture = Xrb::Texture::Create(Xrb::ScreenCoordVector2(width, height), false);
    ASSERT1(texture != NULL);

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

    // the above two calls will cause the image to be read as full 32 bit RGBA

    png_read_update_info(png_ptr, info_ptr);
    png_bytep *row_pointers = new png_bytep[height*sizeof(png_bytep)];;
    for (png_uint_32 row = 0; row < height; row++)
        row_pointers[row] = png_bytep(texture->Data()) + row*texture->Width()*4; // 4 bytes per pixel

    png_read_image(png_ptr, row_pointers);
    png_read_end(png_ptr, info_ptr);

    // At this point we have read the entire image

    // close the file
    fclose(fp);
    // now we're done with the png stuff
    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
    delete[] row_pointers;
    // return the Texture we fought so hard to obtain.
    return texture;
}

Xrb::Pal::Status SDLPal::SaveImage (char const *image_path, Xrb::Texture const &texture)
{
//     fprintf(stderr, "SDLPal::SaveImage(\"%s\");\n", image_path);

    ASSERT1(image_path != NULL);

    // the code in this function is based on the code from example.c
    // in the libpng documentation.

    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;

    fp = fopen(image_path, "wb");
    if (fp == NULL)
    {
        fprintf(stderr, "SDLPal::SaveImage(\"%s\"); error opening file\n", image_path);
        return FAILURE;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        fprintf(stderr, "SDLPal::SaveImage(\"%s\"); error in PNG creation\n", image_path);
        fclose(fp);
        return FAILURE;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fprintf(stderr, "SDLPal::SaveImage(\"%s\"); error in PNG creation\n", image_path);
        fclose(fp);
        png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
        return FAILURE;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fprintf(stderr, "SDLPal::SaveImage(\"%s\"); error in PNG creation\n", image_path);
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return FAILURE;
    }

    png_init_io(png_ptr, fp);

    png_bytep *row_pointers = (png_bytepp)png_malloc(png_ptr, texture.Height()*sizeof(png_bytep));
    for (Xrb::ScreenCoord row = 0; row < texture.Height(); ++row)
        row_pointers[row] = png_bytep(texture.Data()) + row*texture.Width()*4; // 4 bytes per pixel

    // only 32bit RGBA is supported right now
    png_set_IHDR(png_ptr, info_ptr, texture.Width(), texture.Height(), 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

//     // Optionally write comments into the image
//     text_ptr[0].key = "Title";
//     text_ptr[0].text = "Mona Lisa";
//     text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
//     text_ptr[1].key = "Author";
//     text_ptr[1].text = "Leonardo DaVinci";
//     text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
//     text_ptr[2].key = "Description";
//     text_ptr[2].text = "<long text>";
//     text_ptr[2].compression = PNG_TEXT_COMPRESSION_zTXt;
//     png_set_text(png_ptr, info_ptr, text_ptr, 3);

    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, row_pointers);
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return SUCCESS;
}

// font-loading helper functions
namespace {

// container to make dealing with FT_FaceRec_ easier
class FontFace
{
public:

    FontFace (std::string const &path, FT_FaceRec_ *face)
        :
        m_path(path),
        m_face(face)
    {
        ASSERT1(m_face != NULL);
    }
    ~FontFace ()
    {
        FT_Done_Face(m_face);
    }

    static FontFace *Create (std::string const &path, FT_LibraryRec_ *ft_library)
    {
        ASSERT1(ft_library != NULL);

        FontFace *retval = NULL;

        FT_Error error;
        FT_FaceRec_ *face;

        error = FT_New_Face(ft_library, path.c_str(), 0, &face);
        if (error != 0)
            return retval;

        ASSERT1(face != NULL);

        // check if there is a metrics file associated with this font file.
        // (this is sort of a hacky way to check for type1 fonts, but i don't
        // know of any better way).
        if (path.find_last_of(".pfa") < path.length() ||
            path.find_last_of(".pfb") < path.length())
        {
            std::string metrics_path(path.substr(0, path.length()-4));
            metrics_path += ".afm";
            // attempt to attach the font metrics file, but ignore errors,
            // since loading this file is not mandatory.
            FT_Attach_File(face, metrics_path.c_str());

            metrics_path = path.substr(0, path.length()-4);
            metrics_path += ".pfm";
            // attempt to attach the font metrics file, but ignore errors,
            // since loading this file is not mandatory.
            FT_Attach_File(face, metrics_path.c_str());
        }

        if (FT_HAS_KERNING(face))
            fprintf(stderr, "FontFace::Create(\"%s\"); loaded font with kerning\n", path.c_str());
        else
            fprintf(stderr, "FontFace::Create(\"%s\"); loaded font without kerning\n", path.c_str());

        return new FontFace(path, face);
    }

    std::string const &Path () const { return m_path; }
    FT_FaceRec_ *FTFace () const { return m_face; }

private:

    std::string const m_path;
    FT_FaceRec_ *const m_face;
}; // end of class FontFace

Xrb::Uint32 UsedTextureArea (
    Xrb::ScreenCoordVector2 const &texture_size,
    Xrb::AsciiFont::GlyphSpecification *const *const sorted_glyph_specification)
{
    ASSERT1(texture_size[Xrb::Dim::X] > 0);
    ASSERT1(texture_size[Xrb::Dim::Y] > 0);
    // the left hand size of this cryptic expression evaluates to 0
    // for zero or any non-negative integer power of 2 (e.g. 0, 1, 2,
    // 4, 8, 16, 32, 64, etc).
    ASSERT1((texture_size[Xrb::Dim::X] & (texture_size[Xrb::Dim::X] - 1)) == 0);
    ASSERT1((texture_size[Xrb::Dim::Y] & (texture_size[Xrb::Dim::Y] - 1)) == 0);
    ASSERT1(sorted_glyph_specification != NULL);

    Xrb::ScreenCoord packed_width = 0;
    Xrb::ScreenCoordVector2 current_packing_area = Xrb::ScreenCoordVector2::ms_zero;
    Xrb::Uint32 index = 0;
    Xrb::Uint32 total_area = 0;

    while (packed_width + current_packing_area[Xrb::Dim::X] <= texture_size[Xrb::Dim::X] &&
           index < Xrb::AsciiFont::ms_rendered_glyph_count)
    {
        Xrb::AsciiFont::GlyphSpecification *glyph = sorted_glyph_specification[index];
        ASSERT1(glyph != NULL);

        // return failure if this single glyph doesn't even fit
        // inside the entire texture.
        if (glyph->m_size[Xrb::Dim::X] > texture_size[Xrb::Dim::X] ||
            glyph->m_size[Xrb::Dim::Y] > texture_size[Xrb::Dim::Y])
            return 0;

        // if the glyph would stick off the bottom of this column,
        // go to the top of the next column.
        if (current_packing_area[Xrb::Dim::Y] + glyph->m_size[Xrb::Dim::Y] > texture_size[Xrb::Dim::Y])
        {
            packed_width += current_packing_area[Xrb::Dim::X];
            current_packing_area[Xrb::Dim::Y] = 0;
        }
        // otherwise stick the glyph at the bottom of the current column.
        else
        {
            total_area += glyph->m_size[Xrb::Dim::X] * glyph->m_size[Xrb::Dim::Y];
            glyph->m_texture_coordinates.SetComponents(
                packed_width,
                current_packing_area[Xrb::Dim::Y]);
            if (current_packing_area[Xrb::Dim::X] < glyph->m_size[Xrb::Dim::X])
                current_packing_area[Xrb::Dim::X] = glyph->m_size[Xrb::Dim::X];
            current_packing_area[Xrb::Dim::Y] += glyph->m_size[Xrb::Dim::Y];
            ++index;
        }
        ASSERT1(current_packing_area[Xrb::Dim::Y] <= texture_size[Xrb::Dim::Y]);
    }

    // return with success iff all the glyphs were stored within
    // the bounds of the texture.
    if (packed_width <= texture_size[Xrb::Dim::X] && index == Xrb::AsciiFont::ms_rendered_glyph_count)
    {
        ASSERT1(total_area > 0);
        return total_area;
    }
    else
        return 0;
}

Xrb::ScreenCoordVector2 FindSmallestFittingTextureSize (
    Xrb::AsciiFont::GlyphSpecification *const *const sorted_glyph_specification)
{
    GLint max_texture_size;
    // TODO: replace with an accessor to the current video options
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);

    Xrb::Dim::Component increase = Xrb::Dim::Y;
    Xrb::ScreenCoordVector2 texture_size(32, 32);
    while(texture_size[Xrb::Dim::X] <= static_cast<Xrb::ScreenCoord>(max_texture_size) &&
          texture_size[Xrb::Dim::Y] <= static_cast<Xrb::ScreenCoord>(max_texture_size))
    {
        Xrb::Uint32 used_area = UsedTextureArea(texture_size, sorted_glyph_specification);
        DEBUG1_CODE(Xrb::Uint32 total_area = texture_size[Xrb::Dim::X] * texture_size[Xrb::Dim::Y]);
        ASSERT1(used_area <= total_area);
        if (used_area > 0)
            return texture_size;

        texture_size[increase] <<= 1;
        increase = (increase == Xrb::Dim::X) ? Xrb::Dim::Y : Xrb::Dim::X;
    }
    // TODO: proper handling -- scale the face down and try again
    ASSERT0(false && "No texture big enough to hold all glyphs");
    return Xrb::ScreenCoordVector2::ms_zero;
}

Xrb::Texture *GenerateTexture (
    FontFace const &font_face,
    Xrb::ScreenCoordVector2 const &texture_size,
    Xrb::AsciiFont::GlyphSpecification glyph_specification[Xrb::AsciiFont::ms_rendered_glyph_count])
{
    Xrb::Texture *texture = Xrb::Texture::Create(texture_size, true);

    FT_Error error;
    FT_FaceRec_ *ft_face = font_face.FTFace();
    for (Xrb::Uint32 i = 0; i < Xrb::AsciiFont::ms_rendered_glyph_count; ++i)
    {
        Xrb::AsciiFont::GlyphSpecification &glyph = glyph_specification[i];

        error = FT_Load_Glyph(ft_face, FT_Get_Char_Index(ft_face, glyph.m_ascii), FT_LOAD_RENDER);
        ASSERT1(error == 0);
        ASSERT1(ft_face->glyph->format == FT_GLYPH_FORMAT_BITMAP);

        // copy the bitmap data over
        // TODO: real method which does proper pitch detection
        ASSERT1(ft_face->glyph->bitmap.pitch == glyph.m_size[Xrb::Dim::X]);
        Xrb::Uint8 const *source_pixel_data = ft_face->glyph->bitmap.buffer;
        for (Xrb::Sint32 y = 0; y < glyph.m_size[Xrb::Dim::Y]; ++y)
        {
            Xrb::Uint8 *dest_pixel_data =
                texture->Data() +
                (glyph.m_texture_coordinates[Xrb::Dim::Y] + y) * texture_size[Xrb::Dim::X] * 4 +
                 glyph.m_texture_coordinates[Xrb::Dim::X] * 4;

            for (Xrb::Sint32 x = 0; x < glyph.m_size[Xrb::Dim::X]; ++x)
            {
                // assumed to be 32 bit RGBA for now
                *(dest_pixel_data++) = 0xFF;
                *(dest_pixel_data++) = 0xFF;
                *(dest_pixel_data++) = 0xFF;
                *(dest_pixel_data++) = *(source_pixel_data++);
            }
        }
    }

    return texture;
}

} // end of unnamed namespace

Xrb::Font *SDLPal::LoadFont (char const *font_path, Xrb::ScreenCoord pixel_height)
{
    Xrb::AsciiFont *retval = NULL;

    // first check if this font and size are cached on disk
    retval = Xrb::AsciiFont::CreateFromCache(font_path, pixel_height);
    if (retval != NULL)
        return retval;

    // if the font wasn't cached, then we have to create it using the freetype lib.

    FontFace *font_face = FontFace::Create(font_path, m_ft_library);
    if (font_face == NULL)
        return retval;

    FT_FaceRec_ *ft_face = font_face->FTFace();
    ASSERT1(ft_face != NULL);
    {
        FT_Error error = FT_Set_Pixel_Sizes(ft_face, 0, pixel_height);
        if (error != 0)
        {
            Xrb::Delete(font_face);
            return retval;
        }
    }

    bool has_kerning;
    Xrb::ScreenCoord baseline_height;
    Xrb::AsciiFont::GlyphSpecification glyph_specification[Xrb::AsciiFont::ms_rendered_glyph_count];
    Xrb::FontCoord kern_pair_26_6[Xrb::AsciiFont::ms_rendered_glyph_count*Xrb::AsciiFont::ms_rendered_glyph_count];

    has_kerning = FT_HAS_KERNING(ft_face) != 0;

    // populate the glyph specification
    {
        FT_Error error;
        FT_FaceRec_ *ft_face = font_face->FTFace();
        Xrb::ScreenCoord tallest_glyph_height = -1;
        Xrb::ScreenCoord tallest_glyph_bearing_y = -1;

        for (char ascii = Xrb::AsciiFont::ms_rendered_glyph_lowest;
             ascii != Xrb::AsciiFont::ms_rendered_glyph_highest+1;
             ++ascii)
        {
            error = FT_Load_Glyph(ft_face, FT_Get_Char_Index(ft_face, ascii), FT_LOAD_DEFAULT);
            ASSERT1(error == 0);

            Xrb::AsciiFont::GlyphSpecification &glyph = glyph_specification[Xrb::AsciiFont::GlyphIndex(ascii)];

            glyph.m_ascii = ascii;
            glyph.m_size.SetComponents(
                ft_face->glyph->metrics.width >> 6,
                ft_face->glyph->metrics.height >> 6);
            glyph.m_bearing_26_6.SetComponents(
                ft_face->glyph->metrics.horiBearingX,
                ft_face->glyph->metrics.horiBearingY);
            glyph.m_advance_26_6 = ft_face->glyph->metrics.horiAdvance;

            if (tallest_glyph_height < glyph.m_size[Xrb::Dim::Y])
            {
                tallest_glyph_height = glyph.m_size[Xrb::Dim::Y];
                tallest_glyph_bearing_y = Xrb::FontToScreenCoord(glyph.m_bearing_26_6[Xrb::Dim::Y]);
            }
        }

        baseline_height =
            (pixel_height - tallest_glyph_height) / 2 +
            (tallest_glyph_height - tallest_glyph_bearing_y);
    }

    // store the kern pairs
    {
        FT_Error error;
        // if there is no kerning, or there is an error reading the kern data
        // just set all kern pairs to zero.
        if (!has_kerning || (error = FT_Set_Pixel_Sizes(font_face->FTFace(), 0, pixel_height)) != 0)
        {
            memset(kern_pair_26_6, 0, sizeof(kern_pair_26_6));
        }
        else // there is kerning, so store the kern pairs
        {
            FT_Vector delta;
            for (Xrb::Uint32 left = 0; left < Xrb::AsciiFont::ms_rendered_glyph_count; ++left)
            {
                for (Xrb::Uint32 right = 0; right < Xrb::AsciiFont::ms_rendered_glyph_count; ++right)
                {
                    FT_Get_Kerning(
                        font_face->FTFace(),
                        FT_Get_Char_Index(font_face->FTFace(), Xrb::AsciiFont::AsciiValue(left)),
                        FT_Get_Char_Index(font_face->FTFace(), Xrb::AsciiFont::AsciiValue(right)),
                        FT_KERNING_UNFITTED,
                        &delta);
                    kern_pair_26_6[left*Xrb::AsciiFont::ms_rendered_glyph_count + right] = delta.x;
                }
            }
        }
    }

    // make a list of pointers to each glyph specification
    Xrb::AsciiFont::GlyphSpecification *sorted_glyph_specification[Xrb::AsciiFont::ms_rendered_glyph_count];
    for (Xrb::Uint32 i = 0; i < Xrb::AsciiFont::ms_rendered_glyph_count; ++i)
        sorted_glyph_specification[i] = &glyph_specification[i];

    // sort them by width then height
    qsort(
        sorted_glyph_specification,
        Xrb::AsciiFont::ms_rendered_glyph_count,
        sizeof(Xrb::AsciiFont::GlyphSpecification const *),
        Xrb::AsciiFont::GlyphSpecification::SortByWidthFirst);

    // find the smallest texture size that will fit all the glyphs
    Xrb::ScreenCoordVector2 smallest_fitting_texture_size =
        FindSmallestFittingTextureSize(sorted_glyph_specification);
    ASSERT1(smallest_fitting_texture_size != Xrb::ScreenCoordVector2::ms_zero);

    // generate the font texture using the calculated size
    Xrb::Texture *font_texture = GenerateTexture(*font_face, smallest_fitting_texture_size, glyph_specification);

    Xrb::Delete(font_face);

    // create the actual Font instance
    retval = Xrb::AsciiFont::Create(
        font_path,
        pixel_height,
        has_kerning,
        baseline_height,
        glyph_specification,
        kern_pair_26_6,
        font_texture);

    // cache the font to disk and delete the intermediate Texture
    retval->CacheToDisk(font_texture);
    Xrb::Delete(font_texture);

    // finally return the Font we fought so hard to create.
    return retval;
}

#endif // XRB_PLATFORM == XRB_PLATFORM_SDL
