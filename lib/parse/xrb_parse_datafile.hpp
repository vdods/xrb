// ///////////////////////////////////////////////////////////////////////////
// xrb_parse_datafile_value.hpp by Victor Dods, created 2011/08/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_PARSE_DATAFILE_HPP_)
#define _XRB_PARSE_DATAFILE_HPP_

#include "xrb.hpp"

#include "xrb_resourcelibrary.hpp"
#include "xrb_resourceloadparameters.hpp"

namespace Xrb {
namespace Parse {
namespace DataFile {

class Structure;

// for use in loading data files via ResourceLibrary
class LoadParameters : public ResourceLoadParameters
{
public:

    LoadParameters (std::string const &path) : m_path(path) { }

    std::string const &Path () const { return m_path; }

    virtual std::string ResourceName () const { return "Xrb::Parse::DataFile::Structure"; }
    virtual bool IsLessThan (ResourceLoadParameters const &p) const
    {
        LoadParameters const &load_parameters = *DStaticCast<LoadParameters const *>(&p);
        return m_path < load_parameters.Path();
    }
    virtual void Fallback () { m_path.clear(); }
    virtual void Print (FILE *fptr) const
    {
        fprintf(stderr, "path = \"%s\"", m_path.c_str());
    }

private:

    std::string m_path;
}; // end of class LoadParameters

/// Wrapper for loading and parsing a file, so you don't have to touch DataFile::Parser.
Structure *ParseDataFileIntoStructure (std::string const &path);
/// For use in Parse::DataFile::Load only, unless you know what you're doing.
Structure *ParseDataFileIntoStructure (ResourceLoadParameters const &p);

/** NOTE: Unless you know what you're doing, use this instead of Create.
  * Generally in application code, you'll only use Resource<Parse::DataFile::Structure>
  * instead of Parse::DataFile::Structure directly.  This is a frontend to
  * Singleton::ResourceLibrary().Load<Parse::DataFile::Structure>()
  * @brief Loads and parses a file into a Parse::DataFile::Structure via the ResourceLibrary (convenience function)
  */
Resource<Structure> Load (std::string const &path);

} // end of namespace DataFile
} // end of namespace Parse
} // end of namespace Xrb

#endif // !defined(_XRB_PARSE_DATAFILE_HPP_)

