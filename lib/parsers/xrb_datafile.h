// ///////////////////////////////////////////////////////////////////////////
// xrb_datafile.h by Victor Dods, created 2005/07/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_DATAFILE_H_)
#define _XRB_DATAFILE_H_

#include "xrb.h"

#include <stdio.h>

namespace Xrb
{

class DataFileStructure;

/** @brief Contains the parsing functions for the generalized data file
  *        format detailed by @ref Xrb::DataFileValue.
  */
namespace DataFile
{

    /** @c NULL will be returned upon parse error or file open error.
      * @brief Parse the datafile given by @c filename, returning the whole
      *        file as a pointer to a const DataFileStructure.
      * @param filename The filename to open for reading.
      */
    DataFileStructure const *Parse (char const *filename);
    /** @c NULL will be returned upon parse error.
      * @brief Parse the datafile given by @c filename, returning the whole
      *        file as a pointer to a const DataFileStructure.
      * @param fptr The already-opened-for-text-reading-mode FILE pointer
      *             to parse data from.
      */
    DataFileStructure const *Parse (FILE *fptr);

} // end of namespace DataFile

} // end of namespace Xrb

#endif // !defined(_XRB_DATAFILE_H_)

