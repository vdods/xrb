// ///////////////////////////////////////////////////////////////////////////
// lvd_filoc.hpp by Victor Dods, created 2006/07/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(LVD_FILOC_HPP_)
#define LVD_FILOC_HPP_

#if defined(HAVE_CONFIG_H)
    #include "config.h"
#endif // defined(HAVE_CONFIG_H)

#include <cassert>
#include <ostream>
#include <string>

namespace Lvd {

class FiLoc
{
public:

    static FiLoc const ms_invalid;

    FiLoc (std::string const &filename)
        :
        m_filename(filename),
        m_line_number(0)
    {
        assert(!m_filename.empty());
    }
    FiLoc (std::string const &filename, unsigned int line)
        :
        m_filename(filename),
        m_line_number(line)
    {
        assert(!m_filename.empty());
        assert(m_line_number > 0);
    }

    bool IsValid () const { return !m_filename.empty(); }
    bool HasLineNumber () const { return !m_filename.empty() && m_line_number > 0; }
    std::string const &Filename () const
    {
        assert(this != &ms_invalid && "can't use FiLoc::ms_invalid in this manner");
        return m_filename;
    }
    unsigned int LineNumber () const
    {
        assert(this != &ms_invalid && "can't use FiLoc::ms_invalid in this manner");
        return m_line_number;
    }
    std::string AsString () const;

    void SetFilename (std::string const &filename) { m_filename = filename; }
    void SetLineNumber (unsigned int line_number) { m_line_number = line_number; }

    void IncrementLineNumber (unsigned int by_value = 1);

private:

    // for use only by the constructor of ms_invalid
    FiLoc () : m_filename(), m_line_number(0) { }

    std::string m_filename;
    unsigned int m_line_number;
}; // end of class FiLoc

std::ostream &operator << (std::ostream &stream, FiLoc const &filoc);

} // end of namespace Lvd

#endif // !defined(LVD_FILOC_HPP_)
