// ///////////////////////////////////////////////////////////////////////////
// serializerunittest.cpp by Victor Dods, created 2005/05/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "serializerunittest.hpp"

#include "../binaryfileserializer.hpp"
#include "../compressionserializer.hpp"
#include "../huffman.hpp"

using namespace Blah;

#define BINARYFILESERIALIZER_DATA_FILENAME "BinaryFileSerializer.data"
#define COMPRESSIONSERIALIZER_DATA_FILENAME "CompressionSerializer.data"

SerializerUnitTest::SerializerUnitTest ()
    :
    UnitTest("Serializer")
{
    for (Implementation i = LOWEST_IMPLEMENTATION;
         i <= HIGHEST_IMPLEMENTATION;
         IncrementImplementation(i))
    {
        RegisterAtom(&SerializerUnitTest::AtomEndOfFile);
    }

    Uint32 byte_weights[256];
    for (Uint32 i = 0; i < 256; ++i)
        byte_weights[i] = i;
    byte_weights[0xDE] = 100000000;
    byte_weights[0xAD] = 20000000;
    byte_weights[0xBE] = 40000000;
    byte_weights[0xEF] = 100000000;
    m_huffman = new Huffman(byte_weights);
//     m_huffman->FprintCompressedByteWeights(stderr);
}

SerializerUnitTest::~SerializerUnitTest ()
{
    ASSERT1(m_currently_opened_serializer == NULL);
}

void SerializerUnitTest::Initialize ()
{
    m_atom_end_of_file_current_implementation = LOWEST_IMPLEMENTATION;
}

void SerializerUnitTest::AtomEndOfFile ()
{
    Serializer *serializer;

    ASSERT1(m_atom_end_of_file_current_implementation <= HIGHEST_IMPLEMENTATION);

    // write a 4-byte-long file

    printf("\tIteration for \"%s\"\n",
           GetImplementationString(m_atom_end_of_file_current_implementation));

    serializer = OpenASerializerForWriting(m_atom_end_of_file_current_implementation);

    serializer->WriteUint32(0xDEADBEEF);
    TestCritical(serializer->GetError() == IOE_NONE,
                 "wrote a Uint32.  GetError() is %s and should be %s",
                 serializer->GetErrorString(),
                 GetIOErrorString(IOE_NONE));

    // read the same file back, testing the IsAtEnd() and other end conditions
    serializer = CloseSerializerAndOpenSameForReading();

    Test(!serializer->GetHasFewerThan8BitsLeft(),
         "the stream should have more than 8 bits left to read");

    Uint32 read_value = serializer->ReadUint32();
    Test(serializer->IsAtEnd(),
         "read a Uint32.  serializer should be exactly at the end.");

    Test(read_value == 0xDEADBEEF,
         "the read value is %u and should be %u",
         read_value,
         0xDEADBEEF);

    Test(serializer->GetHasFewerThan8BitsLeft(),
         "the stream should now have less than 8 bits left "
         "to read (because it is at the end)");

    serializer->ReadUint8();
    Test(serializer->GetError() == IOE_IS_AT_END,
         "read an additional Uint8.  GetError() is %s and should be %s",
         serializer->GetErrorString(),
         GetIOErrorString(IOE_IS_AT_END));

    CloseSerializer();

    IncrementImplementation(m_atom_end_of_file_current_implementation);
}

char const *SerializerUnitTest::GetImplementationString (Implementation implementation)
{
    ASSERT1(LOWEST_IMPLEMENTATION == 0);
    static char const *implementation_string[NUM_IMPLEMENTATIONS];

    implementation_string[BINARYFILE] = "BinaryFileSerializer";
    implementation_string[COMPRESSION] = "CompressionSerializer";

    ASSERT1(implementation >= LOWEST_IMPLEMENTATION &&
            implementation <= HIGHEST_IMPLEMENTATION);

    return implementation_string[implementation];
}

Serializer *SerializerUnitTest::OpenASerializerForWriting (
    Implementation implementation)
{
    ASSERT1(m_currently_opened_serializer == NULL &&
            "You must not already have a serializer open");

    m_currently_opened_implementation = implementation;
    switch (m_currently_opened_implementation)
    {
        case BINARYFILE:
        {
            BinaryFileSerializer *serializer = new BinaryFileSerializer;

            serializer->Open(BINARYFILESERIALIZER_DATA_FILENAME, "w");
            TestCritical(serializer->GetError() == IOE_NONE,
                         "opened BinaryFileSerializer for writing.  GetError() "
                         "is %s and should be %s",
                         serializer->GetErrorString(),
                         GetIOErrorString(IOE_NONE));

            m_currently_opened_serializer = serializer;
            break;
        }

        case COMPRESSION:
        {
            BinaryFileSerializer *subordinate_serializer = new BinaryFileSerializer;

            subordinate_serializer->Open(COMPRESSIONSERIALIZER_DATA_FILENAME, "w");
            TestCritical(subordinate_serializer->GetError() == IOE_NONE,
                         "opened BinaryFileSerializer for use in "
                         "CompressionSerializer.  GetError() is %s and should be %s",
                         subordinate_serializer->GetErrorString(),
                         GetIOErrorString(IOE_NONE));

            ASSERT1(m_huffman != NULL);
            CompressionSerializer *serializer = new CompressionSerializer(m_huffman);

            serializer->AttachSerializer(subordinate_serializer);
            TestCritical(serializer->GetError() == IOE_NONE,
                         "attached BinaryFileSerializer to CompressionSerializer. "
                         "GetError() is %s and should be %s",
                         serializer->GetErrorString(),
                         GetIOErrorString(IOE_NONE));

            m_currently_opened_serializer = serializer;
            break;
        }

        default:
        {
            ASSERT0(false && "Invalid Implementation");
            m_currently_opened_serializer = NULL;
            break;
        }
    }

    return m_currently_opened_serializer;
}

Serializer *SerializerUnitTest::CloseSerializerAndOpenSameForReading ()
{
    ASSERT1(m_currently_opened_serializer != NULL &&
            "This must be called after OpenASerializerForWriting()");

    switch (m_currently_opened_implementation)
    {
        case BINARYFILE:
        {
            BinaryFileSerializer *serializer =
                dynamic_cast<BinaryFileSerializer *>(m_currently_opened_serializer);
            ASSERT1(serializer != NULL);

            serializer->Close();
            Test(serializer->GetError() == IOE_NONE,
                 "closed BinaryFileSerializer.  GetError() is %s and should be %s",
                 serializer->GetErrorString(),
                 GetIOErrorString(IOE_NONE));

            serializer->Open(BINARYFILESERIALIZER_DATA_FILENAME, "r");
            TestCritical(serializer->GetError() == IOE_NONE,
                         "opened BinaryFileSerializer for writing.  GetError() "
                         "is %s and should be %s",
                         serializer->GetErrorString(),
                         GetIOErrorString(IOE_NONE));
            break;
        }

        case COMPRESSION:
        {
            CompressionSerializer *serializer =
                dynamic_cast<CompressionSerializer *>(m_currently_opened_serializer);
            ASSERT1(serializer != NULL);
            BinaryFileSerializer *subordinate_serializer =
                dynamic_cast<BinaryFileSerializer *>(serializer->AttachedSerializer());
            ASSERT1(subordinate_serializer != NULL);

            serializer->DetachSerializer();
            TestCritical(serializer->GetError() == IOE_NONE,
                         "detached BinaryFileSerializer from CompressionSerializer.  "
                         "GetError() is %s and should be %s",
                         serializer->GetErrorString(),
                         GetIOErrorString(IOE_NONE));

            subordinate_serializer->Close();
            Test(subordinate_serializer->GetError() == IOE_NONE,
                 "closing BinaryFileSerializer that is attached to "
                 "CompressionSerializer.  GetError() is %s and should be %s",
                 subordinate_serializer->GetErrorString(),
                 GetIOErrorString(IOE_NONE));

            subordinate_serializer->Open(COMPRESSIONSERIALIZER_DATA_FILENAME, "r");
            TestCritical(subordinate_serializer->GetError() == IOE_NONE,
                         "opening BinaryFileSerializer for use in "
                         "CompressionSerializer.  GetError() is %s and should be %s",
                         subordinate_serializer->GetErrorString(),
                         GetIOErrorString(IOE_NONE));

            serializer->AttachSerializer(subordinate_serializer);
            TestCritical(serializer->GetError() == IOE_NONE,
                         "attached BinaryFileSerializer to CompressionSerializer. "
                         "GetError() is %s and should be %s",
                         serializer->GetErrorString(),
                         GetIOErrorString(IOE_NONE));
            break;
        }

        default:
        {
            ASSERT0(false && "Invalid implementation");
            m_currently_opened_serializer = NULL;
            break;
        }
    }

    return m_currently_opened_serializer;
}

void SerializerUnitTest::CloseSerializer ()
{
    ASSERT1(m_currently_opened_serializer != NULL &&
            "This must be called after OpenASerializerForWriting()");

    switch (m_currently_opened_implementation)
    {
        case BINARYFILE:
        {
            BinaryFileSerializer *serializer =
                dynamic_cast<BinaryFileSerializer *>(m_currently_opened_serializer);
            ASSERT1(serializer != NULL);

            serializer->Close();
            Test(serializer->GetError() == IOE_NONE,
                 "closing BinaryFileSerializer.  GetError() is %s and should be %s",
                 serializer->GetErrorString(),
                 GetIOErrorString(IOE_NONE));

            delete serializer;
            break;
        }

        case COMPRESSION:
        {
            CompressionSerializer *serializer =
                dynamic_cast<CompressionSerializer *>(m_currently_opened_serializer);
            ASSERT1(serializer != NULL);
            BinaryFileSerializer *subordinate_serializer =
                dynamic_cast<BinaryFileSerializer *>(serializer->AttachedSerializer());
            ASSERT1(subordinate_serializer != NULL);

            serializer->DetachSerializer();
            TestCritical(serializer->GetError() == IOE_NONE,
                         "detached BinaryFileSerializer from CompressionSerializer.  "
                         "GetError() is %s and should be %s",
                         serializer->GetErrorString(),
                         GetIOErrorString(IOE_NONE));

            subordinate_serializer->Close();
            Test(subordinate_serializer->GetError() == IOE_NONE,
                 "closing BinaryFileSerializer that is attached to "
                 "CompressionSerializer.  GetError() is %s and should be %s",
                 subordinate_serializer->GetErrorString(),
                 GetIOErrorString(IOE_NONE));

            delete subordinate_serializer;
            delete serializer;
            break;
        }

        default:
        {
            ASSERT0(false && "Invalid implementation");
            break;
        }
    }

    m_currently_opened_serializer = NULL;
}

