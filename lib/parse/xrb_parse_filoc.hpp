// ///////////////////////////////////////////////////////////////////////////
// xrb_parse_filoc.hpp by Victor Dods, created 2006/07/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_PARSE_FILOC_HPP_)
#define _XRB_PARSE_FILOC_HPP_

#include "xrb.hpp"

#include <ostream>
#include <string>

namespace Xrb
{

class FiLoc
{
public:

    static FiLoc const ms_invalid;

    FiLoc (std::string const &filename)
        :
        m_filename(filename),
        m_line_number(0)
    {
        ASSERT1(!m_filename.empty());
    }
    FiLoc (std::string const &filename, Uint32 line)
        :
        m_filename(filename),
        m_line_number(line)
    {
        ASSERT1(!m_filename.empty());
        ASSERT1(m_line_number > 0);
    }

    bool IsValid () const { return !m_filename.empty(); }
    bool HasLineNumber () const { return !m_filename.empty() && m_line_number > 0; }
    std::string const &Filename () const
    {
        ASSERT1(this != &ms_invalid && "can't use FiLoc::ms_invalid in this manner");
        return m_filename;
    }
    Uint32 LineNumber () const
    {
        ASSERT1(this != &ms_invalid && "can't use FiLoc::ms_invalid in this manner");
        return m_line_number;
    }
    std::string AsString () const;

    void SetFilename (std::string const &filename) { m_filename = filename; }
    void SetLineNumber (Uint32 line_number) { m_line_number = line_number; }

    void IncrementLineNumber (Uint32 by_value = 1);

private:

    // for use only by the constructor of ms_invalid
    FiLoc () : m_filename(), m_line_number(0) { }

    std::string m_filename;
    Uint32 m_line_number;
}; // end of class FiLoc

std::ostream &operator << (std::ostream &stream, FiLoc const &filoc);

} // end of namespace Xrb

#endif // !defined(_XRB_PARSE_FILOC_HPP_)
