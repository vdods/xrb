// ///////////////////////////////////////////////////////////////////////////
// xrb_characterfilter.cpp by Victor Dods, created 2005/02/23
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_characterfilter.h"

namespace Xrb
{

char CharacterFilter::GetFilteredCharacter (char const c) const
{
    char const *filter = GetFilter().c_str();

    // filter the character according to if the filter
    // specifies allowed/disallowed
    switch (GetFilterType())
    {
        case ALLOW:
            // allowed indicates that only characters in the filter
            // string should be able to be typed
            while (*filter)
            {
                // if the character was found in the filter, return it
                if (c == *filter)
                    return c;

                ++filter;
            }
            // the character wasn't found in the filter, return null char
            return '\0';

        case DENY:
            // disallowed indicates that characters in the filter
            // string should not be able to be typed
            while (*filter)
            {
                // if the character was found in the filter, return null char
                if (c == *filter)
                    return '\0';

                ++filter;
            }
            // if the character wasn't filtered out, return it
            return c;

        default:
            ASSERT1(false && "Invalid character filter type")
            return '\0';
    }
}

} // end of namespace Xrb
