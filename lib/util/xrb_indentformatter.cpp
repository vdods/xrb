// ///////////////////////////////////////////////////////////////////////////
// xrb_indentformatter.cpp by Victor Dods, created 2005/07/18
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_indentformatter.hpp"

#include "xrb_util.hpp"

namespace Xrb {

IndentFormatter::IndentFormatter (std::ostream &out, std::string const &indent_string)
    :
    m_out(out),
    m_indent_string(indent_string)
{
    m_indent_level = 0;
    m_indent_before_next_output = false;
}

IndentFormatter::~IndentFormatter ()
{
    Flush();
    ASSERT1(m_cache.str().empty());
}

void IndentFormatter::EnsureNewline ()
{
    m_indent_before_next_output = true;
    
    if (m_cache.str().empty())
        return; // empty cache is as good as newline
        
    if (*m_cache.str().rbegin() != '\n')
        m_cache << '\n';
}

void IndentFormatter::Indent ()
{
    ASSERT1(m_indent_level < UINT32_UPPER_BOUND);
    // we have to flush before the indent level changes so that all the replacements
    // done in Flush happen with the same indent level.
    Flush();
    ++m_indent_level;
}

void IndentFormatter::Unindent ()
{
    ASSERT1(m_indent_level > 0);
    // we have to flush before the indent level changes so that all the replacements
    // done in Flush happen with the same indent level.
    Flush();
    --m_indent_level;
}

void IndentFormatter::Flush ()
{
    if (m_cache.str().empty())
        return; // nothing to do
    
    // steal the string from m_cache
    std::string s(m_cache.str());
    m_cache.str(std::string()); // empty string
    // replace newlines with newline+indentations
    ReplaceNewlinesInString(s);
    // output the munged string
    m_out << s;

    if (!s.empty() && *s.rbegin() == '\n')
        m_indent_before_next_output = true;
}

void IndentFormatter::ReplaceNewlinesInString (std::string &s)
{
    if (s.empty())
        return; // nothing to do
    
    bool restore_ending_newline = false;
    if (*s.rbegin() == '\n')
    {
        restore_ending_newline = true;
        // erase the final newline, but remember to restore it after the other replacement
        s.erase(s.length()-1);
    }
    
    // create the newline replacement string
    std::string newline_replacement;
    newline_replacement += '\n';
    for (Uint32 i = 0; i < m_indent_level; ++i)
        newline_replacement += m_indent_string;
    // replace the other newlines with newlines followed by indentation
    Util::ReplaceAllInString(&s, "\n", newline_replacement);
    
    // restore the newline if indicated.  this one is waiting for indent level to possibly change before being replaced.
    if (restore_ending_newline)
        s.push_back('\n');
}

} // end of namespace Xrb

