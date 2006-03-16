// ///////////////////////////////////////////////////////////////////////////
// xrb_indentformatter.cpp by Victor Dods, created 2005/07/18
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_indentformatter.h"

#include "xrb_util.h"

namespace Xrb
{

IndentFormatter::IndentFormatter (
    FILE *const fptr,
    char const *const indent_string)
{
    ASSERT1(fptr != NULL)
    ASSERT1(indent_string != NULL)
    m_fptr = fptr;
    m_state = FRESH_LINE;
    m_indent_string = indent_string;
    m_indent_level = 0;

    UpdateNewlineReplacement();
}

void IndentFormatter::PrintLine (char const *const format, ...)
{
    ASSERT1(m_state == FRESH_LINE)

    va_list list;
    va_start(list, format);
    InternalPrintf(format, list, true, true);
    va_end(list);
}

void IndentFormatter::BeginLine (char const *const format, ...)
{
    bool indent = m_state == FRESH_LINE;

    va_list list;
    va_start(list, format);
    InternalPrintf(format, list, indent, false);
    va_end(list);

    m_state = CONTINUING_LINE;
}

void IndentFormatter::ContinueLine (char const *const format, ...)
{
    ASSERT1(m_state == CONTINUING_LINE)

    va_list list;
    va_start(list, format);
    InternalPrintf(format, list, false, false);
    va_end(list);
}

void IndentFormatter::EndLine (char const *const format, ...)
{
    bool indent = m_state != CONTINUING_LINE;

    va_list list;
    va_start(list, format);
    InternalPrintf(format, list, indent, true);
    va_end(list);

    m_state = FRESH_LINE;
}

void IndentFormatter::Indent ()
{
    ASSERT1(m_indent_level < UINT32_UPPER_BOUND)
    ++m_indent_level;
    UpdateNewlineReplacement();
}

void IndentFormatter::Unindent ()
{
    ASSERT1(m_indent_level > 0)
    --m_indent_level;
    UpdateNewlineReplacement();
}

void IndentFormatter::InternalPrintf (
    char const *const format,
    va_list list,
    bool const prepend_indentation,
    bool const append_newline)
{
    m_reformat.clear();
    
    if (prepend_indentation)
        for (Uint32 i = 0; i < m_indent_level; ++i)
            m_reformat += m_indent_string;
            
    m_reformat += format;
    Util::ReplaceAllInString(&m_reformat, "\n", m_newline_replacement);

    if (append_newline)
        m_reformat += '\n';

    ASSERT1(m_fptr != NULL)
    vfprintf(m_fptr, m_reformat.c_str(), list);
}

void IndentFormatter::UpdateNewlineReplacement ()
{
    m_newline_replacement.clear();
    m_newline_replacement += '\n';
    for (Uint32 i = 0; i < m_indent_level; ++i)
        m_newline_replacement += m_indent_string;
}

} // end of namespace Xrb

