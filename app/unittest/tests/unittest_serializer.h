// ///////////////////////////////////////////////////////////////////////////
// serializerunittest.h by Victor Dods, created 2005/05/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_SERIALIZERUNITTEST_H_)
#define _SERIALIZERUNITTEST_H_

#include "../defines.h"

#include "unittest.h"
#include "../new_serializer.h"

using namespace Blah;

class Huffman;

class SerializerUnitTest : public UnitTest
{
public:

    SerializerUnitTest ();
    ~SerializerUnitTest ();

    virtual void Initialize ();

protected:

    void AtomEndOfFile ();

private:

    enum Implementation
    {
        BINARYFILE = 0,
        COMPRESSION,

        NUM_IMPLEMENTATIONS,

        LOWEST_IMPLEMENTATION = BINARYFILE,
        HIGHEST_IMPLEMENTATION = COMPRESSION
    };

    inline static void IncrementImplementation (Implementation &implementation)
    {
        implementation = static_cast<Implementation>(static_cast<int>(implementation) + 1);
    }

    static char const *GetImplementationString (Implementation implementation);

    Implementation m_atom_end_of_file_current_implementation;

    Serializer *OpenASerializerForWriting (Implementation implementation);
    Serializer *CloseSerializerAndOpenSameForReading ();
    void CloseSerializer ();

//     void GenerateABunchOfData ();

    Huffman const *m_huffman;

    Serializer *m_currently_opened_serializer;
    Implementation m_currently_opened_implementation;
}; // end of class SerializerUnitTest

#endif // !defined(_SERIALIZERUNITTEST_H_)

