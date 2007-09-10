// ///////////////////////////////////////////////////////////////////////////
// xrb_endian.h by Victor Dods, created 2005/05/14
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined (_XRB_ENDIAN_H_)
#define _XRB_ENDIAN_H_

#include "xrb.h"

// this file has functions which perform conversion to/from
// different endian values

namespace Xrb
{

/** @brief Used to store enumerations and functions which deal with endian
  *        identification and conversion.
  */
namespace Endian
{
    /** 
      * @brief Enumerates the indicator values for little and big endian, as
      *        well as identifying what the machine's and serializer's
      *        endianness is.
      */
    enum Endianness
    {
        LITTLE = 0,
        BIG,

        // The machine's endianness is set from the value in config.h
#if defined(WORDS_BIGENDIAN)
        MACHINE = BIG,
#else // !defined(WORDS_BIGENDIAN)
        MACHINE = LITTLE,
#endif // !defined(WORDS_BIGENDIAN)

        // Serializer is set to the endianness of the omnipresent x86
        // architecture so that the machine-to-serializer and
        // serializer-to-machine are no-ops for the majoritory of machines.
        SERIALIZER = LITTLE
    };

    // ///////////////////////////////////////////////////////////////////////
    // byte-reordering functions
    // ///////////////////////////////////////////////////////////////////////

    /** Single-byte word endian conversion is a no-op.
      * @brief Switches the endianness of a 1-byte word.
      * @param word A pointer to the 1-byte word to be endian-switched.
      */
    inline void Switch1ByteWord (void *word)
    {
        ASSERT1(word != NULL);
        // nothing needs to be done
    }
    /** @brief Switches the endianness of a 2-byte word.
      * @param word A pointer to the 2-byte word to be endian-switched.
      */
    inline void Switch2ByteWord (void *word)
    {
        ASSERT1(word != NULL);
        Uint8 *bytes = static_cast<Uint8 *>(word);

        Uint8 temp   = *(bytes + 0);
        *(bytes + 0) = *(bytes + 1);
        *(bytes + 1) = temp;
    }
    /** @brief Switches the endianness of a 4-byte word.
      * @param word A pointer to the 4-byte word to be endian-switched.
      */
    inline void Switch4ByteWord (void *word)
    {
        ASSERT1(word != NULL);
        Uint8 *bytes = static_cast<Uint8 *>(word);

        Uint8 temp   = *(bytes + 0);
        *(bytes + 0) = *(bytes + 3);
        *(bytes + 3) = temp;

        temp         = *(bytes + 1);
        *(bytes + 1) = *(bytes + 2);
        *(bytes + 2) = temp;
    }
    /** @brief Switches the endianness of a 8-byte word.
      * @param word A pointer to the 8-byte word to be endian-switched.
      */
    inline void Switch8ByteWord (void *word)
    {
        ASSERT1(word != NULL);
        Uint8 *bytes = static_cast<Uint8 *>(word);

        Uint8 temp   = *(bytes + 0);
        *(bytes + 0) = *(bytes + 7);
        *(bytes + 7) = temp;

        temp         = *(bytes + 1);
        *(bytes + 1) = *(bytes + 6);
        *(bytes + 6) = temp;

        temp         = *(bytes + 2);
        *(bytes + 2) = *(bytes + 5);
        *(bytes + 5) = temp;

        temp         = *(bytes + 3);
        *(bytes + 3) = *(bytes + 4);
        *(bytes + 4) = temp;
    }

// if the serializer and machine have the same endianness, then
// the conversion functions should do nothing
#if MACHINE == SERIALIZER

    // ///////////////////////////////////////////////////////////////////////
    // conversion between machine and serializer
    // ///////////////////////////////////////////////////////////////////////
    
    /** Single-byte word endian conversion is a no-op.
      * @brief Converts a 1-byte word from machine to serializer endianness.
      * @param word A pointer to the 1-byte word to be serializer-endian'ed.
      */
    inline void ConvertMachineToSerializer1ByteWord (void *word) { }
    /** Single-byte word endian conversion is a no-op.
      * @brief Converts a 1-byte word from serializer to machine endianness.
      * @param word A pointer to the 1-byte word to be machine-endian'ed.
      */
    inline void ConvertSerializerToMachine1ByteWord (void *word) { }
    /** If this machine and serializer endianness match, this is a no-op.
      * @brief Converts a 2-byte word from machine to serializer endianness.
      * @param word A pointer to the 2-byte word to be serializer-endian'ed.
      */
    inline void ConvertMachineToSerializer2ByteWord (void *word) { }
    /** If this machine and serializer endianness match, this is a no-op.
      * @brief Converts a 2-byte word from serializer to machine endianness.
      * @param word A pointer to the 2-byte word to be machine-endian'ed.
      */
    inline void ConvertSerializerToMachine2ByteWord (void *word) { }
    /** If this machine and serializer endianness match, this is a no-op.
      * @brief Converts a 4-byte word from machine to serializer endianness.
      * @param word A pointer to the 4-byte word to be serializer-endian'ed.
      */
    inline void ConvertMachineToSerializer4ByteWord (void *word) { }
    /** If this machine and serializer endianness match, this is a no-op.
      * @brief Converts a 4-byte word from serializer to machine endianness.
      * @param word A pointer to the 4-byte word to be machine-endian'ed.
      */
    inline void ConvertSerializerToMachine4ByteWord (void *word) { }
    /** If this machine and serializer endianness match, this is a no-op.
      * @brief Converts a 8-byte word from machine to serializer endianness.
      * @param word A pointer to the 8-byte word to be serializer-endian'ed.
      */
    inline void ConvertMachineToSerializer8ByteWord (void *word) { }
    /** If this machine and serializer endianness match, this is a no-op.
      * @brief Converts a 8-byte word from serializer to machine endianness.
      * @param word A pointer to the 8-byte word to be machine-endian'ed.
      */
    inline void ConvertSerializerToMachine8ByteWord (void *word) { }

// otherwise, the byte order should switch
#else // MACHINE != SERIALIZER

    // ///////////////////////////////////////////////////////////////////////
    // conversion between machine and serializer
    // ///////////////////////////////////////////////////////////////////////
    
    /** Single-byte word endian conversion is a no-op.
      * @brief Converts a 1-byte word from machine to serializer endianness.
      * @param word A pointer to the 1-byte word to be serializer-endian'ed.
      */
    inline void ConvertMachineToSerializer1ByteWord (void *word)
    {
        Switch1ByteWord(word);
    }
    /** Single-byte word endian conversion is a no-op.
      * @brief Converts a 1-byte word from serializer to machine endianness.
      * @param word A pointer to the 1-byte word to be machine-endian'ed.
      */
    inline void ConvertSerializerToMachine1ByteWord (void *word)
    {
        Switch1ByteWord(word);
    }
    /** If this machine and serializer endianness match, this is a no-op.
      * @brief Converts a 2-byte word from machine to serializer endianness.
      * @param word A pointer to the 2-byte word to be serializer-endian'ed.
      */
    inline void ConvertMachineToSerializer2ByteWord (void *word)
    {
        Switch2ByteWord(word);
    }
    /** If this machine and serializer endianness match, this is a no-op.
      * @brief Converts a 2-byte word from serializer to machine endianness.
      * @param word A pointer to the 2-byte word to be machine-endian'ed.
      */
    inline void ConvertSerializerToMachine2ByteWord (void *word)
    {
        Switch2ByteWord(word);
    }
    /** If this machine and serializer endianness match, this is a no-op.
      * @brief Converts a 4-byte word from machine to serializer endianness.
      * @param word A pointer to the 4-byte word to be serializer-endian'ed.
      */
    inline void ConvertMachineToSerializer4ByteWord (void *word)
    {
        Switch4ByteWord(word);
    }
    /** If this machine and serializer endianness match, this is a no-op.
      * @brief Converts a 4-byte word from serializer to machine endianness.
      * @param word A pointer to the 4-byte word to be machine-endian'ed.
      */
    inline void ConvertSerializerToMachine4ByteWord (void *word)
    {
        Switch4ByteWord(word);
    }
    /** If this machine and serializer endianness match, this is a no-op.
      * @brief Converts a 8-byte word from machine to serializer endianness.
      * @param word A pointer to the 8-byte word to be serializer-endian'ed.
      */
    inline void ConvertMachineToSerializer8ByteWord (void *word)
    {
        Switch4ByteWord(word);
    }
    /** If this machine and serializer endianness match, this is a no-op.
      * @brief Converts a 8-byte word from serializer to machine endianness.
      * @param word A pointer to the 8-byte word to be machine-endian'ed.
      */
    inline void ConvertSerializerToMachine8ByteWord (void *word)
    {
        Switch4ByteWord(word);
    }

#endif // MACHINE != SERIALIZER

#if MACHINE == BIG

    // ///////////////////////////////////////////////////////////////////////
    // conversion between machine and big
    // ///////////////////////////////////////////////////////////////////////
    
    /** Single-byte word endian conversion is a no-op.
      * @brief Converts a 1-byte word from machine to big endianness.
      * @param word A pointer to the 1-byte word to be big-endian'ed.
      */
    inline void ConvertMachineToBig1ByteWord (void *word) { }
    /** Single-byte word endian conversion is a no-op.
      * @brief Converts a 1-byte word from big to machine endianness.
      * @param word A pointer to the 1-byte word to be machine-endian'ed.
      */
    inline void ConvertBigToMachine1ByteWord (void *word) { }
    /** If this machine is big-endian, then this function is a no-op.
      * @brief Converts a 2-byte word from machine to big endianness.
      * @param word A pointer to the 2-byte word to be big-endian'ed.
      */
    inline void ConvertMachineToBig2ByteWord (void *word) { }
    /** If this machine is big-endian, then this function is a no-op.
      * @brief Converts a 2-byte word from big to machine endianness.
      * @param word A pointer to the 2-byte word to be machine-endian'ed.
      */
    inline void ConvertBigToMachine2ByteWord (void *word) { }
    /** If this machine is big-endian, then this function is a no-op.
      * @brief Converts a 4-byte word from machine to big endianness.
      * @param word A pointer to the 4-byte word to be big-endian'ed.
      */
    inline void ConvertMachineToBig4ByteWord (void *word) { }
    /** If this machine is big-endian, then this function is a no-op.
      * @brief Converts a 4-byte word from big to machine endianness.
      * @param word A pointer to the 4-byte word to be machine-endian'ed.
      */
    inline void ConvertBigToMachine4ByteWord (void *word) { }
    /** If this machine is big-endian, then this function is a no-op.
      * @brief Converts a 8-byte word from machine to big endianness.
      * @param word A pointer to the 8-byte word to be big-endian'ed.
      */
    inline void ConvertMachineToBig8ByteWord (void *word) { }
    /** If this machine is big-endian, then this function is a no-op.
      * @brief Converts a 8-byte word from big to machine endianness.
      * @param word A pointer to the 8-byte word to be machine-endian'ed.
      */
    inline void ConvertBigToMachine8ByteWord (void *word) { }

    // ///////////////////////////////////////////////////////////////////////
    // conversion between machine and little
    // ///////////////////////////////////////////////////////////////////////
    
    /** Single-byte word endian conversion is a no-op.
      * @brief Converts a 1-byte word from machine to little endianness.
      * @param word A pointer to the 1-byte word to be little-endian'ed.
      */
    inline void ConvertMachineToLittle1ByteWord (void *word)
    {
        Switch1ByteWord(word);
    }
    /** Single-byte word endian conversion is a no-op.
      * @brief Converts a 1-byte word from little to machine endianness.
      * @param word A pointer to the 1-byte word to be machine-endian'ed.
      */
    inline void ConvertLittleToMachine1ByteWord (void *word)
    {
        Switch1ByteWord(word);
    }
    /** If this machine is little-endian, then this function is a no-op.
      * @brief Converts a 2-byte word from machine to little endianness.
      * @param word A pointer to the 2-byte word to be little-endian'ed.
      */
    inline void ConvertMachineToLittle2ByteWord (void *word)
    {
        Switch2ByteWord(word);
    }
    /** If this machine is little-endian, then this function is a no-op.
      * @brief Converts a 2-byte word from little to machine endianness.
      * @param word A pointer to the 2-byte word to be machine-endian'ed.
      */
    inline void ConvertLittleToMachine2ByteWord (void *word)
    {
        Switch2ByteWord(word);
    }
    /** If this machine is little-endian, then this function is a no-op.
      * @brief Converts a 4-byte word from machine to little endianness.
      * @param word A pointer to the 4-byte word to be little-endian'ed.
      */
    inline void ConvertMachineToLittle4ByteWord (void *word)
    {
        Switch4ByteWord(word);
    }
    /** If this machine is little-endian, then this function is a no-op.
      * @brief Converts a 4-byte word from little to machine endianness.
      * @param word A pointer to the 4-byte word to be machine-endian'ed.
      */
    inline void ConvertLittleToMachine4ByteWord (void *word)
    {
        Switch4ByteWord(word);
    }
    /** If this machine is little-endian, then this function is a no-op.
      * @brief Converts a 8-byte word from machine to little endianness.
      * @param word A pointer to the 8-byte word to be little-endian'ed.
      */
    inline void ConvertMachineToLittle8ByteWord (void *word)
    {
        Switch8ByteWord(word);
    }
    /** If this machine is little-endian, then this function is a no-op.
      * @brief Converts a 8-byte word from little to machine endianness.
      * @param word A pointer to the 8-byte word to be machine-endian'ed.
      */
    inline void ConvertLittleToMachine8ByteWord (void *word)
    {
        Switch8ByteWord(word);
    }

#else // MACHINE != BIG (implying MACHINE == LITTLE)

    // ///////////////////////////////////////////////////////////////////////
    // conversion between machine and big
    // ///////////////////////////////////////////////////////////////////////
    
    /** Single-byte word endian conversion is a no-op.
      * @brief Converts a 1-byte word from machine to big endianness.
      * @param word A pointer to the 1-byte word to be big-endian'ed.
      */
    inline void ConvertMachineToBig1ByteWord (void *word)
    {
        Switch1ByteWord(word);
    }
    /** Single-byte word endian conversion is a no-op.
      * @brief Converts a 1-byte word from big to machine endianness.
      * @param word A pointer to the 1-byte word to be machine-endian'ed.
      */
    inline void ConvertBigToMachine1ByteWord (void *word)
    {
        Switch1ByteWord(word);
    }
    /** If this machine is big-endian, then this function is a no-op.
      * @brief Converts a 2-byte word from machine to big endianness.
      * @param word A pointer to the 2-byte word to be big-endian'ed.
      */
    inline void ConvertMachineToBig2ByteWord (void *word)
    {
        Switch2ByteWord(word);
    }
    /** If this machine is big-endian, then this function is a no-op.
      * @brief Converts a 2-byte word from big to machine endianness.
      * @param word A pointer to the 2-byte word to be machine-endian'ed.
      */
    inline void ConvertBigToMachine2ByteWord (void *word)
    {
        Switch2ByteWord(word);
    }
    /** If this machine is big-endian, then this function is a no-op.
      * @brief Converts a 4-byte word from machine to big endianness.
      * @param word A pointer to the 4-byte word to be big-endian'ed.
      */
    inline void ConvertMachineToBig4ByteWord (void *word)
    {
        Switch4ByteWord(word);
    }
    /** If this machine is big-endian, then this function is a no-op.
      * @brief Converts a 4-byte word from big to machine endianness.
      * @param word A pointer to the 4-byte word to be machine-endian'ed.
      */
    inline void ConvertBigToMachine4ByteWord (void *word)
    {
        Switch4ByteWord(word);
    }
    /** If this machine is big-endian, then this function is a no-op.
      * @brief Converts a 8-byte word from machine to big endianness.
      * @param word A pointer to the 8-byte word to be big-endian'ed.
      */
    inline void ConvertMachineToBig8ByteWord (void *word)
    {
        Switch8ByteWord(word);
    }
    /** If this machine is big-endian, then this function is a no-op.
      * @brief Converts a 8-byte word from big to machine endianness.
      * @param word A pointer to the 8-byte word to be machine-endian'ed.
      */
    inline void ConvertBigToMachine8ByteWord (void *word)
    {
        Switch8ByteWord(word);
    }

    // ///////////////////////////////////////////////////////////////////////
    // conversion between machine and little
    // ///////////////////////////////////////////////////////////////////////
    
    /** Single-byte word endian conversion is a no-op.
      * @brief Converts a 1-byte word from machine to little endianness.
      * @param word A pointer to the 1-byte word to be little-endian'ed.
      */
    inline void ConvertMachineToLittle1ByteWord (void *word) { }
    /** Single-byte word endian conversion is a no-op.
      * @brief Converts a 1-byte word from little to machine endianness.
      * @param word A pointer to the 1-byte word to be machine-endian'ed.
      */
    inline void ConvertLittleToMachine1ByteWord (void *word) { }
    /** If this machine is little-endian, then this function is a no-op.
      * @brief Converts a 2-byte word from machine to little endianness.
      * @param word A pointer to the 2-byte word to be little-endian'ed.
      */
    inline void ConvertMachineToLittle2ByteWord (void *word) { }
    /** If this machine is little-endian, then this function is a no-op.
      * @brief Converts a 2-byte word from little to machine endianness.
      * @param word A pointer to the 2-byte word to be machine-endian'ed.
      */
    inline void ConvertLittleToMachine2ByteWord (void *word) { }
    /** If this machine is little-endian, then this function is a no-op.
      * @brief Converts a 4-byte word from machine to little endianness.
      * @param word A pointer to the 4-byte word to be little-endian'ed.
      */
    inline void ConvertMachineToLittle4ByteWord (void *word) { }
    /** If this machine is little-endian, then this function is a no-op.
      * @brief Converts a 4-byte word from little to machine endianness.
      * @param word A pointer to the 4-byte word to be machine-endian'ed.
      */
    inline void ConvertLittleToMachine4ByteWord (void *word) { }
    /** If this machine is little-endian, then this function is a no-op.
      * @brief Converts a 8-byte word from machine to little endianness.
      * @param word A pointer to the 8-byte word to be little-endian'ed.
      */
    inline void ConvertMachineToLittle8ByteWord (void *word) { }
    /** If this machine is little-endian, then this function is a no-op.
      * @brief Converts a 8-byte word from little to machine endianness.
      * @param word A pointer to the 8-byte word to be machine-endian'ed.
      */
    inline void ConvertLittleToMachine8ByteWord (void *word) { }

#endif // MACHINE != BIG (impling MACHINE == LITTLE)

    // ///////////////////////////////////////////////////////////////////////
    // conversion between machine and given endianness
    // ///////////////////////////////////////////////////////////////////////

    /** Single-byte word endian conversion is a no-op.
      * @brief Converts a 1-byte word from machine to the given endianness.
      * @param word A pointer to the 1-byte word to be converted.
      * @param endianness The desired endianness of the word.
      */
    inline void ConvertMachineToGiven1ByteWord (void *word, Endianness const endianness)
    {
        if (MACHINE != endianness)
            Switch1ByteWord(word);
    }
    /** Single-byte word endian conversion is a no-op.
      * @brief Converts a 1-byte word from the given endianness to machine.
      * @param endianness The current endianness of the word.
      * @param word A pointer to the 1-byte word to be converted.
      */
    inline void ConvertGivenToMachine1ByteWord (Endianness const endianness, void *word)
    {
        if (MACHINE != endianness)
            Switch1ByteWord(word);
    }
    /** If the given endianness matches the machine's, then this function is a no-op.
      * @brief Converts a 2-byte word from machine to the given endianness.
      * @param word A pointer to the 2-byte word to be converted.
      * @param endianness The desired endianness of the word.
      */
    inline void ConvertMachineToGiven2ByteWord (void *word, Endianness const endianness)
    {
        if (MACHINE != endianness)
            Switch2ByteWord(word);
    }
    /** If the given endianness matches the machine's, then this function is a no-op.
      * @brief Converts a 2-byte word from the given endianness to machine.
      * @param endianness The current endianness of the word.
      * @param word A pointer to the 2-byte word to be converted.
      */
    inline void ConvertGivenToMachine2ByteWord (Endianness const endianness, void *word)
    {
        if (MACHINE != endianness)
            Switch2ByteWord(word);
    }
    /** If the given endianness matches the machine's, then this function is a no-op.
      * @brief Converts a 4-byte word from machine to the given endianness.
      * @param word A pointer to the 4-byte word to be converted.
      * @param endianness The desired endianness of the word.
      */
    inline void ConvertMachineToGiven4ByteWord (void *word, Endianness const endianness)
    {
        if (MACHINE != endianness)
            Switch4ByteWord(word);
    }
    /** If the given endianness matches the machine's, then this function is a no-op.
      * @brief Converts a 4-byte word from the given endianness to machine.
      * @param endianness The current endianness of the word.
      * @param word A pointer to the 4-byte word to be converted.
      */
    inline void ConvertGivenToMachine4ByteWord (Endianness const endianness, void *word)
    {
        if (MACHINE != endianness)
            Switch4ByteWord(word);
    }
    /** If the given endianness matches the machine's, then this function is a no-op.
      * @brief Converts a 8-byte word from machine to the given endianness.
      * @param word A pointer to the 8-byte word to be converted.
      * @param endianness The desired endianness of the word.
      */
    inline void ConvertMachineToGiven8ByteWord (void *word, Endianness const endianness)
    {
        if (MACHINE != endianness)
            Switch8ByteWord(word);
    }
    /** If the given endianness matches the machine's, then this function is a no-op.
      * @brief Converts a 8-byte word from the given endianness to machine.
      * @param endianness The current endianness of the word.
      * @param word A pointer to the 8-byte word to be converted.
      */
    inline void ConvertGivenToMachine8ByteWord (Endianness const endianness, void *word)
    {
        if (MACHINE != endianness)
            Switch8ByteWord(word);
    }
} // end of namespace endian

} // end of namespace Xrb

#endif // !defined(_XRB_ENDIAN_H_)
