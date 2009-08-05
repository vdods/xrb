// ///////////////////////////////////////////////////////////////////////////
// xrb_tokenizer.hpp by Victor Dods, created 2005/05/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_TOKENIZER_HPP_)
#define _XRB_TOKENIZER_HPP_

#include "xrb.hpp"

namespace Xrb
{

class Tokenizer
{
public:

    // this version of the constructor takes a pointer to a non-const char
    // string, which we will use directly.
    Tokenizer (char *string_to_tokenize, char const *delimiters);
    // this version of the constructor takes a pointer to a const char
    // string, which means we will need to make a copy in order to use it.
    Tokenizer (const char *string_to_tokenize, char const *delimiters);
    // destructor -- deletes the string_to_tokenize iff a copy was made
    // of the original.
    ~Tokenizer ();

    // returns the first token and then successive tokens.
    char const *GetToken () const;

private:

    // helper function which returns true iff the given character is
    // in the set of delimiters.
    bool IsCharADelimiter (char c) const;

    // the string which will be tokenized.  must point to non-const
    // chars, because null chars are inserted and removed to delimit
    // the end of tokens.
    char *m_string_to_tokenize;
    // indicates if m_string_to_tokenize is actually an allocated
    // copy of the provided string (which means that it will have to
    // be deleted upon destruction).
    bool m_string_to_tokenize_requires_deletion;
    // string containing the delimiters.  this string should be immutable,
    // or at least exist for the entire life of this Tokenizer object.
    // a delimiter cannot be the null char.
    char const *m_delimiters;
    // a pointer to the end of the last token, where the replaced
    // delimiter should be put back.
    mutable char *m_end_of_token;
    // delimiter which was replaced by a null char in the previous call
    // which needs to be replaced to reconstruct the string properly.
    mutable char m_replaced_delimiter;
}; // end of class Tokenizer

} // end of namespace Xrb

#endif // !defined(_XRB_TOKENIZER_HPP_)
