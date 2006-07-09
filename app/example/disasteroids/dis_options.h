// ///////////////////////////////////////////////////////////////////////////
// dis_options.h by Victor Dods, created 2006/07/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_OPTIONS_H_)
#define _DIS_OPTIONS_H_

#include "xrb_commandlineparser.h"

#include "xrb_screencoord.h"

using namespace Xrb;

namespace Dis
{

class Options : public CommandLineParser
{
public:

    Options (Sint32 argc, char const *const *argv);

    inline bool GetFullscreen () const { return m_fullscreen; }
    inline ScreenCoordVector2 const &GetResolution () const { return m_resolution; }
    inline bool GetIsHelpRequested () const { return m_is_help_requested; }

    void SetFullscreen (std::string const &arg);
    void SetResolution (std::string const &arg);

    void RequestHelp (std::string const &arg);

private:

    bool m_fullscreen;
    ScreenCoordVector2 m_resolution;

    bool m_is_help_requested;
}; // end of class Options

} // end of namespace Dis

#endif // !defined(_DIS_OPTIONS_H_)

