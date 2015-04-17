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
#include "xrb_singleton.hpp"

namespace Xrb {
namespace Parse {
namespace DataFile {

Structure *ParseDataFileIntoStructure (std::string const &path, DataFile::Parser::ReturnCode *return_code)
{
    DataFile::Parser parser;
    DataFile::Parser::ReturnCode rc = parser.Parse(path);
    // if the return_code pointer was provided, store the return code at the pointed-at location.
    if (return_code != NULL)
        *return_code = rc;
    // if the parse didn't work for whatever reason, don't change the values.
    if (rc == Parser::RC_SUCCESS)
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
    LoadParameters const &load_parameters = p.As<LoadParameters>();

    // sentinel value for Fallback -- load empty Structure
    if (load_parameters.Path() == "internal://missing")
        return new Structure();

    DataFile::Parser::ReturnCode return_code = Parser::RC_SUCCESS;
    Structure *structure = ParseDataFileIntoStructure(load_parameters.Path(), &return_code);
    switch (return_code)
    {
        case DataFile::Parser::RC_SUCCESS:              break; // no problem
        case DataFile::Parser::RC_INVALID_FILENAME:     load_parameters.SetErrorMessage("RC_INVALID_FILENAME"); break;
        case DataFile::Parser::RC_FILE_OPEN_FAILURE:    load_parameters.SetErrorMessage("RC_FILE_OPEN_FAILURE"); break;
        case DataFile::Parser::RC_PARSE_ERROR:          load_parameters.SetErrorMessage("RC_PARSE_ERROR"); break;
        case DataFile::Parser::RC_ERRORS_ENCOUNTERED:   load_parameters.SetErrorMessage("RC_ERRORS_ENCOUNTERED"); break;
        default: ASSERT1(false && "unhandled DataFile::Parser::ReturnCode"); break;
    }
    return structure;
}

Resource<Structure> Load (std::string const &path)
{
    return Singleton::ResourceLibrary().Load<Structure>(ParseDataFileIntoStructure, new LoadParameters(path));
}

} // end of namespace DataFile
} // end of namespace Parse
} // end of namespace Xrb

