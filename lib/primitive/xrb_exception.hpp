// ///////////////////////////////////////////////////////////////////////////
// xrb_exception.hpp by Victor Dods, created 2009/08/22
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_EXCEPTION_HPP_)
#define _XRB_EXCEPTION_HPP_

#include "xrb.hpp"

#include <exception>
#include <string>

namespace Xrb
{

class Exception : public std::exception
{
public:

    Exception (std::string const &description) throw() : m_description(description) { }
    virtual ~Exception () throw() { }

    virtual const char *what () const throw() { return m_description.c_str(); }

protected:

    std::string m_description;
}; // end of class Exception

} // end of namespace Xrb

#endif // !defined(_XRB_EXCEPTION_HPP_)
