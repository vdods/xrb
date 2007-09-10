// ///////////////////////////////////////////////////////////////////////////
// xrb_tokenizer.cpp by Victor Dods, created 2005/05/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_tokenizer.h"

#include "xrb_util.h"

namespace Xrb
{

Tokenizer::Tokenizer (
    char *const string_to_tokenize,
    char const *const delimiters)
{
    ASSERT1(string_to_tokenize != NULL);
    ASSERT1(delimiters != NULL);

    m_string_to_tokenize = string_to_tokenize;
    m_string_to_tokenize_requires_deletion = false;
    m_delimiters = delimiters;
    m_end_of_token = NULL;
}

Tokenizer::Tokenizer (
    const char *const string_to_tokenize,
    char const *const delimiters)
{
    ASSERT1(string_to_tokenize != NULL);
    ASSERT1(delimiters != NULL);

    m_string_to_tokenize = Util::StringDuplicate(string_to_tokenize);
    m_string_to_tokenize_requires_deletion = false;
    m_delimiters = delimiters;
    m_end_of_token = NULL;
}

Tokenizer::~Tokenizer ()
{
    if (m_string_to_tokenize_requires_deletion)
        Delete(m_string_to_tokenize);
}

char const *Tokenizer::GetToken () const
{
    // if the end of the previous token is not null, replace the char
    // it points to with the value of m_replaced_delimiter
    if (m_end_of_token)
    {
        // if the replaced delimiter is the sentinel value, return null
        // because this indicates the end of the string to tokenize.
        if (m_replaced_delimiter == '\0')
            return 0;

        *m_end_of_token = m_replaced_delimiter;
    }
    // otherwise it indicates that we're starting at the beginning.
    else
    {
        ASSERT1(m_replaced_delimiter == '\0');
        m_end_of_token = m_string_to_tokenize;
    }

    // starting from the end of the previous token, skip all delimiters
    // until a non-delimiter is found (but don't go past the end of
    // the string of course).
    while (*m_end_of_token != '\0' &&
           GetIsCharADelimiter(*m_end_of_token))
    {
        ++m_end_of_token;
    }

    // if the first non-delimiter char is the null char, it indicates
    // the end of the string, so set m_replaced_delimiter to its
    // sentinel value and then return null.
    if (*m_end_of_token == '\0')
    {
        m_replaced_delimiter = '\0';
        return 0;
    }

    // at this point, m_end_of_token points to the first char
    // in the next token.  save this value to be returned once the
    // end of the token has been found.
    char const *token_to_return = m_end_of_token;

    // skip all the non-delimiter chars and find the first delimiter,
    // which will be the end of the token, and the char to replace
    // with a null (but don't go past the end of the string of course).
    while (*m_end_of_token != '\0' &&
           !GetIsCharADelimiter(*m_end_of_token))
    {
        ++m_end_of_token;
    }

    // if the first delimiter char is the null char, it indicates
    // the end of the string, so set m_replaced_delimiter to its
    // sentinel value and then return null.
    if (*m_end_of_token == '\0')
    {
        m_replaced_delimiter = '\0';
        return token_to_return;
    }

    // at this point, m_end_of_token is pointing to the non-null char
    // delimiter at the end of the requested token.  replace it with
    // a null char, saving off the delimiter that was replaced.
    ASSERT1(GetIsCharADelimiter(*m_end_of_token));
    m_replaced_delimiter = *m_end_of_token;
    *m_end_of_token = 0;

    // return the token
    return token_to_return;
}

bool Tokenizer::GetIsCharADelimiter (char const c) const
{
    ASSERT1(m_delimiters != NULL);

    char const *delimiter = m_delimiters;
    while (*delimiter)
    {
        if (c == *delimiter)
            return true;

        ++delimiter;
    }

    return false;
}

} // end of namespace Xrb
