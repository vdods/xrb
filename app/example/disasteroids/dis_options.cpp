// ///////////////////////////////////////////////////////////////////////////
// dis_options.cpp by Victor Dods, created 2006/07/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_options.h"

#include <sstream>

using namespace std;
using namespace Xrb;

namespace Dis
{

CommandLineOption const Options::ms_option[] =
{
    CommandLineOption("Video options"),
    CommandLineOption(
        'f',
        "fullscreen",
        &Options::SetFullscreen,
        "    Indicates if fullscreen video will be used.  The argument must be\n"
        "    either 1 or 0, indicating fullscreen and non-fullscreen respectively.\n"
        "    The default value is 1.  See also option -r."),
    CommandLineOption(
        'r',
        "resolution",
        &Options::SetResolution,
        "    Set the screen resolution.  The argument must be of the form 123x456,\n"
        "    where the first value is width, and the second is height.  The default\n"
        "    value is 1024x768.  See also option -f."),
    CommandLineOption("Keyboard options"),
    CommandLineOption(
        'k',
        "keymap",
        &Options::SetKeyMapName,
        "    Sets the keyboard layout to use.  The windows version of SDL does not\n"
        "    support alternate keyboard layouts (this option is unnecessary in other\n"
        "    operating systems).  Valid values are: \"dvorak\" (quotes for clarity).\n"
        "    Anything else will disable altered key mapping (this is the default)."),
    CommandLineOption(""),
    CommandLineOption(
        'h',
        "help",
        &Options::RequestHelp,
        "    Prints this help message.")
};
Uint32 const Options::ms_option_count = sizeof(Options::ms_option) / sizeof(CommandLineOption);

Options::Options (std::string const &executable_filename)
    :
    CommandLineParser(
        &Options::NonOptionArgumentHandler,
        ms_option,
        ms_option_count,
        executable_filename,
        "Disasteroids - written by Victor Dods, as part of the XuqRijBuh Game Engine.",
        "[options]"),
    m_fullscreen(true),
    m_resolution(ScreenCoordVector2::ms_zero),
    m_key_map_name("none"),
    m_is_help_requested(false)
{ }

void Options::SetFullscreen (string const &arg)
{
    if (arg.length() != 1 || arg[0] != '0' && arg[0] != '1')
        throw string("error: invalid argument to --fullscreen - \"") + arg + "\"";
    else
        m_fullscreen = arg[0] == '1';
}

void Options::SetResolution (string const &arg)
{
    if (arg.empty())
        throw string("error: invalid argument to --resolution - \"") + arg + "\"";

    istringstream in(arg);
    ScreenCoordVector2 resolution;
    char c;

    c = in.peek();
    if (c < '0' || c > '9')
        throw string("error: invalid argument to --resolution - \"") + arg + "\"";
    in >> resolution[Dim::X];

    in >> c;
    if (c != 'x' && c != 'X')
        throw string("error: invalid argument to --resolution - \"") + arg + "\"";

    c = in.peek();
    if (c < '0' || c > '9')
        throw string("error: invalid argument to --resolution - \"") + arg + "\"";
    in >> resolution[Dim::Y];

    m_resolution = resolution;
}

void Options::SetKeyMapName (std::string const &arg)
{
    if (arg.empty())
        throw string("error: invalid argument to --keymap - \"") + arg + "\"";

    m_key_map_name = arg;
}

void Options::NonOptionArgumentHandler (std::string const &arg)
{
    throw string("error: invalid non-option argument - \"") + arg + "\"";
}

void Options::RequestHelp ()
{
    m_is_help_requested = true;
}

} // end of namespace Dis

