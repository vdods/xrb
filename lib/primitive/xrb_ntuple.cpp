// ///////////////////////////////////////////////////////////////////////////
// xrb_ntuple.cpp by Victor Dods, created 2005/06/12
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_ntuple.h"

namespace Xrb
{

// the Bool2 printing function is special because it has to do conversion
// from bool to string.
void Fprint (
    FILE *const fptr,
    Bool2 const &ntuple,
    bool const add_newline)
{
    fprintf(
        fptr,
        "2Tuple = (%s, %s)%c",
        BOOL_TO_STRING(ntuple[0]),
        BOOL_TO_STRING(ntuple[1]),
        add_newline ? '\n' : '\0');
}

// the Alignment2 printing function is also special because it has to
// print stringified enum names.
void Fprint (
    FILE *const fptr,
    Alignment2 const &ntuple,
    bool const add_newline)
{
    char const *stringified0, *stringified1;

    switch (ntuple[0])
    {
        case TOP:    stringified0 = STRINGIFY(TOP);    break;
        case LEFT:   stringified0 = STRINGIFY(LEFT);   break;
        case CENTER: stringified0 = STRINGIFY(CENTER); break;
        case BOTTOM: stringified0 = STRINGIFY(BOTTOM); break;
        case RIGHT:  stringified0 = STRINGIFY(RIGHT);  break;
        case SPACED: stringified0 = STRINGIFY(SPACED); break;
        default: ASSERT1(false && "Invalid Alignment"); return;
    }
    switch (ntuple[1])
    {
        case TOP:    stringified1 = STRINGIFY(TOP);    break;
        case LEFT:   stringified1 = STRINGIFY(LEFT);   break;
        case CENTER: stringified1 = STRINGIFY(CENTER); break;
        case BOTTOM: stringified1 = STRINGIFY(BOTTOM); break;
        case RIGHT:  stringified1 = STRINGIFY(RIGHT);  break;
        case SPACED: stringified1 = STRINGIFY(SPACED); break;
        default: ASSERT1(false && "Invalid Alignment"); return;
    }

    fprintf(
        fptr,
        "2Tuple = (%s, %s)%c",
        stringified0,
        stringified1,
        add_newline ? '\n' : '\0');
}

} // end of namespace Xrb

