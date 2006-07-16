// ///////////////////////////////////////////////////////////////////////////
// xrb_datafilelocation.h by Victor Dods, created 2006/07/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_DATAFILELOCATION_H_)
#define _XRB_DATAFILELOCATION_H_

#include "xrb.h"

#include <iostream>
#include <string>

namespace Xrb
{

class DataFileLocation
{
public:

    static DataFileLocation const ms_invalid;

    DataFileLocation (std::string const &filename, uint line)
        :
        m_filename(filename),
        m_line(line)
    {
        assert(GetIsValid());
    }

    inline bool GetIsValid () const
    {
        return !m_filename.empty() && m_line > 0;
    }
    inline std::string const &GetFilename () const
    {
        assert(GetIsValid() && "can't use DataFileLocation::ms_invalid in this manner");
        return m_filename;
    }
    inline uint GetLine () const
    {
        assert(GetIsValid() && "can't use DataFileLocation::ms_invalid in this manner");
        return m_line;
    }
    std::string GetText () const;

private:

    // for use only by the constructor of ms_invalid
    DataFileLocation ()
        :
        m_filename(),
        m_line(0)
    { }

    std::string m_filename;
    uint m_line;
}; // end of class DataFileLocation

std::ostream &operator << (std::ostream &stream, DataFileLocation const &file_location);

} // end of namespace Xrb

#endif // !defined(_XRB_DATAFILELOCATION_H_)
