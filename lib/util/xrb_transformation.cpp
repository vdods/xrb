// ///////////////////////////////////////////////////////////////////////////
// xrb_transformation.cpp by Victor Dods, created 2006/01/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_transformation.hpp"

#include "xrb_util.hpp"

namespace Xrb
{

bool Transformation::BooleanNegation (bool value)
{
    return !value;
}

std::string Transformation::Lowercase (std::string value)
{
    Util::MakeLowercase(&value);
    return value;
}

std::string Transformation::Uppercase (std::string value)
{
    Util::MakeUppercase(&value);
    return value;
}

} // end of namespace Xrb
