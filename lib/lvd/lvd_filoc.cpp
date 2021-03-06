// ///////////////////////////////////////////////////////////////////////////
// lvd_filoc.cpp by Victor Dods, created 2006/07/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include <lvd_filoc.hpp>

#include <sstream>

using namespace std;

namespace Lvd {

FiLoc const FiLoc::ms_invalid;

string FiLoc::AsString () const
{
    assert(this != &ms_invalid && "can't use FiLoc::ms_invalid in this manner");
    assert(IsValid());

    ostringstream out;
    out << m_filename;
    if (m_line_number > 0)
        out << ":" << m_line_number;
    return out.str();
}

void FiLoc::IncrementLineNumber (unsigned int by_value)
{
    assert(m_line_number > 0 && "don't use this on non-line-number-using FiLocs");
    m_line_number += by_value;
}

ostream &operator << (ostream &stream, FiLoc const &filoc)
{
    return stream << filoc.AsString();
}

} // end of namespace Lvd
