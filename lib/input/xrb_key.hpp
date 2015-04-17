// ///////////////////////////////////////////////////////////////////////////
// xrb_key.hpp by Victor Dods, created 2004/06/09
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_KEY_HPP_)
#define _XRB_KEY_HPP_

#include "xrb.hpp"

#include <string>

#include "xrb_eventhandler.hpp"

namespace Xrb
{

/** Also provides static functions for converting between keycode types
  * and checking if a particular keycode may generate EventKeyRepeat
  * events.
  * @brief Contains the function mapping for a single key/button.
  */
class Key : public EventHandler
{
public:

    /** These key codes were predicated on SDL's key codes, but are
      * now separate.  Ascii codes are used here when appropriate.
      * @brief All possible key codes that can be used by Key.
      */
    enum Code
    {
        INVALID = -1,
        NONE = -2,
        UNKNOWN = 0,
        LEFTMOUSE,
        MIDDLEMOUSE,
        RIGHTMOUSE,
        MOUSEWHEELUP,
        MOUSEWHEELDOWN,
        BACKSPACE,
        TAB = '\t',         // ascii 0x09
        CLEAR,
        RETURN = '\r',      // ascii 0x0D
        PAUSE,
        ESCAPE = '\x1B',    // ascii 0x1B
        SPACE = ' ',        // ascii 0x20
        QUOTE = '\'',       // ascii 0x27
        COMMA = ',',        // ascii 0x2C
        MINUS = '-',        // ascii 0x2D
        PERIOD = '.',       // ascii 0x2E
        SLASH = '/',        // ascii 0x2F
        ZERO = '0',         // ascii 0x30
        ONE = '1',          // ascii 0x31
        TWO = '2',          // ascii 0x32
        THREE = '3',        // ascii 0x33
        FOUR = '4',         // ascii 0x34
        FIVE = '5',         // ascii 0x35
        SIX = '6',          // ascii 0x36
        SEVEN = '7',        // ascii 0x37
        EIGHT = '8',        // ascii 0x38
        NINE = '9',         // ascii 0x39
        SEMICOLON = ';',    // ascii 0x3B
        EQUALS = '=',       // ascii 0x3D
        LEFTBRACKET = '[',  // ascii 0x5B
        BACKSLASH = '\\',   // ascii 0x5C
        RIGHTBRACKET = ']', // ascii 0x5D
        BACKQUOTE = '`',    // ascii 0x60
        A = 'a',            // ascii 0x61
        B = 'b',            // ascii 0x62
        C = 'c',            // ascii 0x63
        D = 'd',            // ascii 0x64
        E = 'e',            // ascii 0x65
        F = 'f',            // ascii 0x66
        G = 'g',            // ascii 0x67
        H = 'h',            // ascii 0x68
        I = 'i',            // ascii 0x69
        J = 'j',            // ascii 0x6A
        K = 'k',            // ascii 0x6B
        L = 'l',            // ascii 0x6C
        M = 'm',            // ascii 0x6D
        N = 'n',            // ascii 0x6E
        O = 'o',            // ascii 0x6F
        P = 'p',            // ascii 0x70
        Q = 'q',            // ascii 0x71
        R = 'r',            // ascii 0x72
        S = 's',            // ascii 0x73
        T = 't',            // ascii 0x74
        U = 'u',            // ascii 0x75
        V = 'v',            // ascii 0x76
        W = 'w',            // ascii 0x77
        X = 'x',            // ascii 0x78
        Y = 'y',            // ascii 0x79
        Z = 'z',            // ascii 0x7A
        DELETE = '\x7F',    // ascii 0x7F
        WORLD_0,
        WORLD_1,
        WORLD_2,
        WORLD_3,
        WORLD_4,
        WORLD_5,
        WORLD_6,
        WORLD_7,
        WORLD_8,
        WORLD_9,
        WORLD_10,
        WORLD_11,
        WORLD_12,
        WORLD_13,
        WORLD_14,
        WORLD_15,
        WORLD_16,
        WORLD_17,
        WORLD_18,
        WORLD_19,
        WORLD_20,
        WORLD_21,
        WORLD_22,
        WORLD_23,
        WORLD_24,
        WORLD_25,
        WORLD_26,
        WORLD_27,
        WORLD_28,
        WORLD_29,
        WORLD_30,
        WORLD_31,
        WORLD_32,
        WORLD_33,
        WORLD_34,
        WORLD_35,
        WORLD_36,
        WORLD_37,
        WORLD_38,
        WORLD_39,
        WORLD_40,
        WORLD_41,
        WORLD_42,
        WORLD_43,
        WORLD_44,
        WORLD_45,
        WORLD_46,
        WORLD_47,
        WORLD_48,
        WORLD_49,
        WORLD_50,
        WORLD_51,
        WORLD_52,
        WORLD_53,
        WORLD_54,
        WORLD_55,
        WORLD_56,
        WORLD_57,
        WORLD_58,
        WORLD_59,
        WORLD_60,
        WORLD_61,
        WORLD_62,
        WORLD_63,
        WORLD_64,
        WORLD_65,
        WORLD_66,
        WORLD_67,
        WORLD_68,
        WORLD_69,
        WORLD_70,
        WORLD_71,
        WORLD_72,
        WORLD_73,
        WORLD_74,
        WORLD_75,
        WORLD_76,
        WORLD_77,
        WORLD_78,
        WORLD_79,
        WORLD_80,
        WORLD_81,
        WORLD_82,
        WORLD_83,
        WORLD_84,
        WORLD_85,
        WORLD_86,
        WORLD_87,
        WORLD_88,
        WORLD_89,
        WORLD_90,
        WORLD_91,
        WORLD_92,
        WORLD_93,
        WORLD_94,
        WORLD_95,
        KP0,
        KP1,
        KP2,
        KP3,
        KP4,
        KP5,
        KP6,
        KP7,
        KP8,
        KP9,
        KP_PERIOD,
        KP_DIVIDE,
        KP_MULTIPLY,
        KP_MINUS,
        KP_PLUS,
        KP_ENTER,
        KP_EQUALS,
        UP,
        DOWN,
        RIGHT,
        LEFT,
        INSERT,
        HOME,
        END,
        PAGEUP,
        PAGEDOWN,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        NUMLOCK,
        CAPSLOCK,
        SCROLLLOCK,
        RSHIFT,
        LSHIFT,
        RCTRL,
        LCTRL,
        RALT,
        LALT,
        RMETA,
        LMETA,
        LWINBLOWS,
        RWINBLOWS,
        MODE,
        COMPOSE,
        HELP,
        PRINT,
        SYSREQ,
        BREAK,
        MENU,
        POWER,
        EURO,
        UNDO
    }; // end of enum Code

    /** This enum isn't actually used in an instance of Key.  Key is just
      * being used for namespace scoping/organization.
      * @brief Flags for key modifiers such as left-alt, caps lock, etc.
      */
    enum Modifier
    {
        MOD_NONE   = 0x0000,
        MOD_LSHIFT = 0x0001,
        MOD_RSHIFT = 0x0002,
        MOD_LCTRL  = 0x0040,
        MOD_RCTRL  = 0x0080,
        MOD_LALT   = 0x0100,
        MOD_RALT   = 0x0200,
        MOD_LMETA  = 0x0400,
        MOD_RMETA  = 0x0800,
        MOD_NUM    = 0x1000,
        MOD_CAPS   = 0x2000,
        MOD_MODE   = 0x4000,

        // convenience flags
        MOD_SHIFT  = MOD_LSHIFT|MOD_RSHIFT,
        MOD_ALT    = MOD_LALT|MOD_RALT,
        MOD_CTRL   = MOD_LCTRL|MOD_RCTRL
    }; // end of enum Modifier

    ~Key () { }

    static Key *Create (Key::Code key, std::string const &name);

    static bool IsKeyRepeatable (Key::Code code);
    static bool IsKeyAscii (Key::Code code);
    inline Key::Code GetCode () const { return m_code; }
    inline std::string const &Name () const { return m_name; }
    inline bool IsPressed () const { return m_is_pressed; }

    // resets the m_is_pressed state to false
    void ResetPressed ();

protected:

    // protected constructor so you *have* to use Create instead
    Key ();

    // processes an event (really just sets the 'pressed' bit)
    virtual bool HandleEvent (Event const &e);

private:

    // the value of the key as specified by Event
    Key::Code m_code;
    // the name of the button (i.e. "A" or "LALT" or "F11")
    std::string m_name;
    // indicates if the button is currently pressed
    bool m_is_pressed;
}; // end of class Key

GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_TAB_IS_ASCII, Key::TAB == '\t')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_RETURN_IS_ASCII, Key::RETURN == '\r')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_ESCAPE_IS_ASCII, Key::ESCAPE == 0x1B)
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_SPACE_IS_ASCII, Key::SPACE == ' ')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_QUOTE_IS_ASCII, Key::QUOTE == '\'')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_COMMA_IS_ASCII, Key::COMMA == ',')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_MINUS_IS_ASCII, Key::MINUS == '-')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_PERIOD_IS_ASCII, Key::PERIOD == '.')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_SLASH_IS_ASCII, Key::SLASH == '/')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_ZERO_IS_ASCII, Key::ZERO == '0')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_ONE_IS_ASCII, Key::ONE == '1')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_TWO_IS_ASCII, Key::TWO == '2')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_THREE_IS_ASCII, Key::THREE == '3')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_FOUR_IS_ASCII, Key::FOUR == '4')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_FIVE_IS_ASCII, Key::FIVE == '5')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_SIX_IS_ASCII, Key::SIX == '6')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_SEVEN_IS_ASCII, Key::SEVEN == '7')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_EIGHT_IS_ASCII, Key::EIGHT == '8')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_NINE_IS_ASCII, Key::NINE == '9')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_SEMICOLON_IS_ASCII, Key::SEMICOLON == ';')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_EQUALS_IS_ASCII, Key::EQUALS == '=')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_LEFTBRACKET_IS_ASCII, Key::LEFTBRACKET == '[')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_BACKSLASH_IS_ASCII, Key::BACKSLASH == '\\')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_RIGHTBRACKET_IS_ASCII, Key::RIGHTBRACKET == ']')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_BACKQUOTE_IS_ASCII, Key::BACKQUOTE == '`')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_A_IS_ASCII, Key::A == 'a')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_B_IS_ASCII, Key::B == 'b')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_C_IS_ASCII, Key::C == 'c')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_D_IS_ASCII, Key::D == 'd')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_E_IS_ASCII, Key::E == 'e')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_F_IS_ASCII, Key::F == 'f')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_G_IS_ASCII, Key::G == 'g')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_H_IS_ASCII, Key::H == 'h')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_I_IS_ASCII, Key::I == 'i')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_J_IS_ASCII, Key::J == 'j')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_K_IS_ASCII, Key::K == 'k')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_L_IS_ASCII, Key::L == 'l')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_M_IS_ASCII, Key::M == 'm')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_N_IS_ASCII, Key::N == 'n')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_O_IS_ASCII, Key::O == 'o')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_P_IS_ASCII, Key::P == 'p')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_Q_IS_ASCII, Key::Q == 'q')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_R_IS_ASCII, Key::R == 'r')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_S_IS_ASCII, Key::S == 's')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_T_IS_ASCII, Key::T == 't')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_U_IS_ASCII, Key::U == 'u')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_V_IS_ASCII, Key::V == 'v')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_W_IS_ASCII, Key::W == 'w')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_X_IS_ASCII, Key::X == 'x')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_Y_IS_ASCII, Key::Y == 'y')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_Z_IS_ASCII, Key::Z == 'z')
GLOBAL_SCOPE_COMPILE_TIME_ASSERT(KEY_DELETE_IS_ASCII, Key::DELETE == 0x7F)

} // end of namespace Xrb

#endif // !defined(_XRB_KEY_HPP_)
