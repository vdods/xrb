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

#include <stdarg.h>
#include <stdio.h>
#include <string>

namespace Xrb
{

class IndentFormatter
{
public:

    IndentFormatter (FILE *fptr, char const *indent_string);
    ~IndentFormatter () { } 

    void PrintLine (char const *format, ...);
    void BeginLine (char const *format, ...);
    void ContinueLine (char const *format, ...);
    void EndLine (char const *format, ...);
    void Indent ();
    void Unindent ();
    
private:

    enum State
    {
        FRESH_LINE = 0,
        CONTINUING_LINE
    };        

    void InternalPrintf (
        char const *format,
        va_list list,
        bool prepend_indentation,
        bool append_newline);
    void UpdateNewlineReplacement ();

    FILE *m_fptr;
    State m_state;
    char const *m_indent_string;
    Uint32 m_indent_level;
    std::string m_newline_replacement;
    std::string m_reformat;
}; // end of class IndentFormatter

} // end of namespace Xrb

#endif // !defined(_XRB_INDENTFORMATTER_HPP_)

