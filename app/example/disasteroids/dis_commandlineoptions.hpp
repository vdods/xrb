// ///////////////////////////////////////////////////////////////////////////
// dis_commandlineoptions.hpp by Victor Dods, created 2006/07/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_COMMANDLINEOPTIONS_HPP_)
#define _DIS_COMMANDLINEOPTIONS_HPP_

#include "xrb_commandlineparser.hpp"

#include "xrb_screencoord.hpp"

using namespace Xrb;

namespace Dis
{

class CommandLineOptions : public CommandLineParser
{
public:

    CommandLineOptions (std::string const &executable_filename);

    inline void InitializeFullscreen (bool fullscreen) { m_fullscreen = fullscreen; }
    inline void InitializeResolution (ScreenCoordVector2 const &resolution) { m_resolution = resolution; }
    inline void InitializeKeyMapName (std::string const &key_map_name) { m_key_map_name = key_map_name; }

    inline bool Fullscreen () const { return m_fullscreen; }
    inline ScreenCoordVector2 const &Resolution () const { return m_resolution; }
    inline std::string const &KeyMapName () const { return m_key_map_name; }
    inline bool IsHelpRequested () const { return m_is_help_requested; }

private:

    void SetFullscreen (std::string const &arg);
    void SetResolution (std::string const &arg);
    void SetKeyMapName (std::string const &arg);

    void NonOptionArgumentHandler (std::string const &arg);
    void RequestHelp ();

    static CommandLineOption const ms_option[];
    static Uint32 const ms_option_count;

    bool m_fullscreen;
    ScreenCoordVector2 m_resolution;
    std::string m_key_map_name;

    bool m_is_help_requested;
}; // end of class CommandLineOptions

} // end of namespace Dis

#endif // !defined(_DIS_COMMANDLINEOPTIONS_HPP_)

