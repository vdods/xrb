// ///////////////////////////////////////////////////////////////////////////
// xrb_indentformatter.hpp by Victor Dods, created 2005/07/18
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_INDENTFORMATTER_HPP_)
#define _XRB_INDENTFORMATTER_HPP_

#include "xrb.hpp"

#include <sstream>
#include <string>

#include "xrb_util.hpp"

namespace Xrb {

/// This is guaranteed not to change any of the format flags of the stream passed to it.
class IndentFormatter
{
public:

    /// The ostream you provide better stay in scope for at least as long as this does.
    IndentFormatter (std::ostream &out, std::string const &indent_string);
    ~IndentFormatter ();

    void EnsureNewline ();
    void Indent ();
    void Unindent ();
    void Flush ();

    template <typename T>
    IndentFormatter &operator << (T const &value)
    {
        if (m_indent_before_next_output)
        {
            for (Uint32 i = 0; i < m_indent_level; ++i)
                m_cache << m_indent_string;
            m_indent_before_next_output = false;
        }
        m_cache << value;
        // flush it if we have a newline at the end, so the cache doesn't get too big.
        if (!m_cache.str().empty() && *m_cache.str().rbegin() == '\n')
            Flush();
        return *this;
    }
    
private:

    void ReplaceNewlinesInString (std::string &s);

    // the ostream to output everything to eventually
    std::ostream &m_out;
    // the string specifying a single indentation (e.g. "    " or "\t")
    std::string const m_indent_string;
    // the current indent count
    Uint32 m_indent_level;
    // indicates that an indentation should be printed before anything else
    bool m_indent_before_next_output;
    // this is what actually catches all the formatting flags
    std::ostringstream m_cache;
}; // end of class IndentFormatter

} // end of namespace Xrb

#endif // !defined(_XRB_INDENTFORMATTER_HPP_)

