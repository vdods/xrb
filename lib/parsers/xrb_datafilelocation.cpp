// ///////////////////////////////////////////////////////////////////////////
// xrb_datafilelocation.cpp by Victor Dods, created 2006/07/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_datafilelocation.h"

#include <sstream>

namespace Xrb
{

DataFileLocation const DataFileLocation::ms_invalid;

std::string DataFileLocation::GetText () const
{
    std::ostringstream buffer;
    buffer << m_filename << ":" << m_line;
    return buffer.str();
}

std::ostream &operator << (std::ostream &stream, DataFileLocation const &file_location)
{
    ASSERT1(file_location.GetIsValid())
    return stream << file_location.GetText();
}

} // end of namespace Xrb
