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
    OPTION_HEADER("Video options"),
    {
        'f',
        "fullscreen",
        REQUIRES_A_PARAMETER,
        OPTION_HANDLER(&Options::SetFullscreen),
        "    Indicates if fullscreen video will be used.  The parameter must be\n"
        "    either 1 or 0, indicating fullscreen and non-fullscreen respectively.\n"
        "    The default value is 1.  See also option -r."
    },
    {
        'r',
        "resolution",
        REQUIRES_A_PARAMETER,
        OPTION_HANDLER(&Options::SetResolution),
        "    Set the screen resolution.  The parameter must be of the form 123x456,\n"
        "    where the first value is width, and the second is height.  The default\n"
        "    value is 1024x768.  See also option -f."
    },
    OPTION_HEADER(""),
    {
        'h',
        "help",
        NO_PARAMETER,
        OPTION_HANDLER(&Options::RequestHelp),
        "    Prints this help message."
    }
};
Uint32 const Options::ms_option_count = sizeof(Options::ms_option) / sizeof(CommandLineOption);

Options::Options (std::string const &executable_name)
    :
    CommandLineParser(
        NULL, // no non-option argument handler function
        ms_option,
        ms_option_count,
        executable_name,
        "Disasteroids - written by Victor Dods, as part of the XuqRijBuh Game Engine.",
        "[options]"),
    m_fullscreen(true),
    m_resolution(ScreenCoordVector2::ms_zero),
    m_is_help_requested(false)
{ }

void Options::Parse (Sint32 const argc, char const *const *const argv)
{
    CommandLineParser::Parse(argc, argv);
}

void Options::SetFullscreen (string const &arg)
{
    if (arg.length() != 1 || arg[0] != '0' && arg[0] != '1')
        throw string("error: invalid parameter to --fullscreen");
    else
        m_fullscreen = arg[0] == '1';
}

void Options::SetResolution (string const &arg)
{
    if (arg.empty())
        throw string("error: invalid parameter to --resolution");

    istringstream in(arg);
    ScreenCoordVector2 resolution;
    char c;

    c = in.peek();
    if (c < '0' || c > '9')
        throw string("error: invalid parameter to --resolution");
    in >> resolution[Dim::X];

    in >> c;
    if (c != 'x' && c != 'X')
        throw string("error: invalid parameter to --resolution");

    c = in.peek();
    if (c < '0' || c > '9')
        throw string("error: invalid parameter to --resolution");
    in >> resolution[Dim::Y];

    m_resolution = resolution;
}

void Options::RequestHelp (std::string const &arg)
{
    ASSERT1(arg.empty())

    m_is_help_requested = true;
}

} // end of namespace Dis

