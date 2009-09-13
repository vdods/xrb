// ///////////////////////////////////////////////////////////////////////////
// xrb_resourceloadparameters.hpp by Victor Dods, created 2009/09/12
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_RESOURCELOADPARAMETERS_HPP_)
#define _XRB_RESOURCELOADPARAMETERS_HPP_

#include "xrb.hpp"

#include <string>

namespace Xrb
{

// this class is used for articulating parameters to the load function
// for a Resource.  subclass it and be happy.
class ResourceLoadParameters
{
public:

    virtual ~ResourceLoadParameters () { }

    // return a unique identifier representing the implementation of
    // this interface class.  the class name of the implementation is
    // preferred.
    virtual std::string Name () const = 0;
    // ordering method.  other.Name() is guaranteed to match Name(),
    // so that it's reasonable to do dynamic casting inside the method.
    virtual bool IsLessThan (ResourceLoadParameters const &other_parameters) const = 0;
    // for console spew.  should print all on one line.
    virtual void Print (FILE *fptr) const = 0;
}; // end of class ResourceLoadParameters

} // end of namespace Xrb

#endif // !defined(_XRB_RESOURCELOADPARAMETERS_HPP_)

