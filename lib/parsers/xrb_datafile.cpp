// ///////////////////////////////////////////////////////////////////////////
// xrb_datafile.cpp by Victor Dods, created 2005/07/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_datafile.h"

#include "xrb_datafilevalue.h"

// function prototypes for the scanner
int DataFile_lex_init (void **yyscanner);
int DataFile_lex_destroy (void *yyscanner);
void DataFile_set_in (FILE *in_str, void *yyscanner);
void DataFile_set_allocation_tracker (
    Xrb::DataFileValue::AllocationTracker *allocation_tracker,
    void *data_file_scanner);

// function prototype for the parser
int DataFile_parse (void *yyscanner, Xrb::DataFileStructure const **parsed_structure);

namespace Xrb
{

DataFileStructure const *DataFile::Parse (char const *const filename)
{
    DataFileStructure const *retval = NULL;
    FILE *fptr = fopen(filename, "rt");
    if (fptr == NULL)
        return retval;

    retval = Parse(fptr);
    fclose(fptr);
    return retval;
}

DataFileStructure const *DataFile::Parse (FILE *const fptr)
{
    ASSERT1(fptr != NULL)

// this is not yet supported in WIN32
#if defined(WIN32)
    return NULL;
#else // !defined(WIN32)
    void *data_file_scanner;
    DataFileStructure const *parsed_structure;
    DataFileValue::AllocationTracker allocation_tracker;

    // set up the scanner
    DataFile_lex_init(&data_file_scanner);
    // set the scanner to read from the requested file
    DataFile_set_in(fptr, data_file_scanner);
    // set the scanner to use this DataFile's allocation tracker
    DataFile_set_allocation_tracker(&allocation_tracker, data_file_scanner);
    // parse the file
    int parse_exit_code = DataFile_parse(data_file_scanner, &parsed_structure);
    // shut down the scanner
    DataFile_lex_destroy(data_file_scanner);

    // if the file was parsed correctly, then detach the parsed structure
    // from the allocation tracker (because the allocation tracker is
    // used to track dangling pointers)
    if (parse_exit_code == 0)
        DataFileValue::RemoveFromOwnerAllocationTracker(parsed_structure);
    // otherwise set the parsed structure to NULL (it will be deleted
    // by deleting the tracked allocations).
    else
        parsed_structure = NULL;

    // delete the contents of the allocation tracker.
    DataFileValue::DeleteAllocations(&allocation_tracker);
    ASSERT1(allocation_tracker.size() == 0)
    // return the parsed structure.
    return parsed_structure;
#endif // !defined(WIN32)
}

} // end of namespace Xrb

