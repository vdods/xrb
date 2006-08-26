// ///////////////////////////////////////////////////////////////////////////
// xrb_arithmeticscanner.h by Victor Dods, created 2006/08/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ARITHMETICSCANNER_H_)
#define _XRB_ARITHMETICSCANNER_H_

#include "xrb.h"

#include <string>

#include "xrb_arithmeticparser.h"

namespace Xrb
{

class ArithmeticScanner
{
public:

    ArithmeticScanner (std::string const &input_string);

    ArithmeticParser::Token::Type Scan (Float *scanned_token);

private:

    std::string const m_input_string;
    std::string::const_iterator m_it;
    std::string::const_iterator m_it_end;
}; // end of class ArithmeticScanner

} // end of namespace Xrb

#endif // !defined(_XRB_ARITHMETICSCANNER_H_)
