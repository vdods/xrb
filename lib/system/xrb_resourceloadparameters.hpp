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

namespace Xrb {

// this class is used for articulating parameters to the load function
// for a Resource (e.g. filesystem path).  subclass it and be happy.
class ResourceLoadParameters
{
public:

    virtual ~ResourceLoadParameters () { }

    // return a unique identifier representing the implementation of
    // this interface class.  the fully qualified class name of the
    // data being loaded is preferred (e.g. "Xrb::GlTexture").
    virtual std::string ResourceName () const = 0;
    // ordering method.  other.ResourceName() is guaranteed to match
    // ResourceName(), so that it's reasonable to do dynamic casting
    // inside the method.
    virtual bool IsLessThan (ResourceLoadParameters const &p) const = 0;
    // should return true iff this ResourceLoadParameters indicates to load
    // using the "fallback" parameters.
    virtual bool IsFallback () const = 0;
    // hook for fallback behavior -- if LoadFunction fails (see
    // ResourceLibrary::Load<>), then this method will be called on the
    // ResourceLoadParameters instance, and then LoadFunction will be
    // called again.  this allows handling of load failures, e.g. loading
    // a dummy texture if the real texture fails to load.
    virtual void Fallback () = 0;
    // for console spew.  should print all on one line, no trailing newline.
    virtual void Print (std::ostream &stream) const = 0;

    // returns the current error message (defaults to the empty string).
    std::string const &ErrorMessage () const { return m_error_message; }
    // the error message is a way for the resource loading code to indicate the cause of error
    void SetErrorMessage (std::string const &error_message) const { m_error_message = error_message; }
    
    // convenience function for casting up
    template <typename T>
    T const &As () const { return dynamic_cast<T const &>(*this); }
    // convenience function for casting up
    template <typename T>
    T &As () { return dynamic_cast<T &>(*this); }
    
    // for use in sorting ResourceLoadParameters instances
    struct LessThan
    {
        bool operator () (
            ResourceLoadParameters const *left,
            ResourceLoadParameters const *right) const
        {
            ASSERT1(left != NULL);
            ASSERT1(right != NULL);

            int comparison = left->ResourceName().compare(right->ResourceName());
            if (comparison < 0)
                return true;
            else if (comparison > 0)
                return false;
            else
                return left->IsLessThan(*right);
        }
    }; // end of struct ResourceLoadParameters::LessThan

private:

    mutable std::string m_error_message;
}; // end of class ResourceLoadParameters

} // end of namespace Xrb

#endif // !defined(_XRB_RESOURCELOADPARAMETERS_HPP_)

