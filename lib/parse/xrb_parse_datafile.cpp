// ///////////////////////////////////////////////////////////////////////////
// xrb_parse_datafile.cpp by Victor Dods, created 2011/08/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_parse_datafile.hpp"

#include "xrb_parse_datafile_parser.hpp"
#include "xrb_parse_datafile_value.hpp"

namespace Xrb {
namespace Parse {
namespace DataFile {

Structure *ParseDataFileIntoStructure (std::string const &path)
{
    DataFile::Parser parser;
    // if the parse didn't work for whatever reason, don't change the values.
    if (parser.Parse(path) == Parser::RC_SUCCESS)
    {
        DataFile::Structure *root = parser.StealAcceptedStructure();
        ASSERT1(root != NULL);
        return root;
    }
    else
        return NULL;
}

Structure *ParseDataFileIntoStructure (ResourceLoadParameters const &p)
{
    LoadParameters const &load_parameters = *DStaticCast<LoadParameters const *>(&p);

    // an empty path indicates we should load an empty Structure
    if (load_parameters.Path().empty())
        return new Structure();

    return ParseDataFileIntoStructure(load_parameters.Path());
}

Resource<Structure> Load (std::string const &path)
{
    return Singleton::ResourceLibrary().Load<Structure>(ParseDataFileIntoStructure, new LoadParameters(path));
}

} // end of namespace DataFile
} // end of namespace Parse
} // end of namespace Xrb

