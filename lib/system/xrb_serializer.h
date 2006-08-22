// ///////////////////////////////////////////////////////////////////////////
// xrb_serializer.h by Victor Dods, created 2005/02/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_SERIALIZER_H_)
#define _XRB_SERIALIZER_H_

#include "xrb.h"

#include <stdio.h>
#include <string>

#include "xrb_bitarray.h"
#include "xrb_bitcache.h"
#include "xrb_color.h"
#include "xrb_enums.h"
#include "xrb_matrix2.h"
#include "xrb_simpletransform2.h"
#include "xrb_transform2.h"
#include "xrb_vector.h"

namespace Xrb
{

// metanote: i use the terms serializer and stream interchangeably

/** The interface is general enough so that more or less any form of I/O
  * can be accomplished (e.g. files, network streams, compression streams,
  * memory buffers, etc).
  * @brief Provides the abstract interface for serialized data streams.
  */
class Serializer
{
protected:

    enum
    {
        // with null-terminating character
        MAX_SUPPORTED_STRING_BUFFER_SIZE = 65536,
    };

public:

    enum
    {
        // no null-terminating character
        MAX_SUPPORTED_STRING_LENGTH = MAX_SUPPORTED_STRING_BUFFER_SIZE - 1
    };

    Serializer ();
    virtual ~Serializer ();

    // ///////////////////////////////////////////////////////////////////////
    // state accessors
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Get the open state of the serializer.
      * @return True iff the stream is open for input/output.
      * @note Will not change the error value.
      */
    inline bool GetIsOpen () const
    {
        return m_is_open;
    }
    /** @brief Get the I/O direction of the serializer.
      * @return @c IOD_READ if reading, @c IOD_WRITE if writing,
      *         or IOD_NONE if neither is applicable (e.g. if the stream is
      *         not currently open).
      * @note Will not change the error value.
      */
    inline IODirection GetIODirection () const
    {
        return m_io_direction;
    }
    /** @brief Get the end-of-stream condition.
      * @return True iff the stream is at the end (e.g. end of file)
      * @note Will not change the error value.
      */
    virtual bool GetIsAtEnd () const = 0;
    /** This is necessary because GetIsAtEnd() doesn't work when a set
      * of data doesn't end exactly on a byte boundary.
      * @brief Get the less-than-one-byte-left-in-in-the-stream condition.
      * @return True iff the stream has fewer than 8 bits left.
      * @note Will not change the error value.
      */
    virtual bool GetHasFewerThan8BitsLeft () const = 0;
    /** @brief Get the error state of the most recently called procedure.
      * @return The most recent error state value.
      * @note Will not change the error value.
      */
    inline IOError GetError () const
    {
        return m_error;
    }
    /** @brief Get the string which describes the current error state.
      * @return The null-terminated string describing the current error state.
      * @note Will not change the error value.
      */
    inline char const *GetErrorString () const
    {
        return Util::GetIOErrorString(GetError());
    }

    // ///////////////////////////////////////////////////////////////////////
    // reading/writing functions
    // ///////////////////////////////////////////////////////////////////////

    /** The highest bit read in is used to fill the extra high bits in
      * the return value so the sign is correct.
      * @brief Read a specific number of signed bits from the stream.
      * @param bit_count The number of bits (up to 32) to read.
      * @return The bits read in from the stream, positioned in the least
      *         significant @c bit_count bits of the return value.
      * @note You must be careful to use enough bits when reading/writing
      *       bits from/to a signed integer value; the extra "sign bit"
      *       must be considered when deciding how many bits to use.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual Sint32 ReadSignedBits (Uint32 bit_count) = 0;
    /** @brief Write the least significant @c bit_count bits (up to 32)
      *        of the signed integer, @c value, to the stream.
      * @param value The value from which the lowest @c bit_count bits will
      *              used to write to the stream.
      * @note You must be careful to use enough bits when reading/writing
      *       bits from/to a signed integer value; the extra "sign bit"
      *       must be considered when deciding how many bits to use.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void WriteSignedBits (Sint32 value, Uint32 bit_count) = 0;

    /** @brief Read a specific number of unsigned bits from the stream.
      * @param bit_count The number of bits (up to 32) to read.
      * @return The bits read in from the stream, positioned in the least
      *         significant @c bit_count bits of the return value.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual Uint32 ReadUnsignedBits (Uint32 bit_count) = 0;
    /** @brief Write the least significant @c bit_count bits (up to 32)
      *        of the unsigned integer, @c value, to the stream.
      * @param value The value from which the lowest @c bit_count bits will
      *              used to write to the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void WriteUnsignedBits (Uint32 value, Uint32 bit_count) = 0;

    /** @brief Read a boolean value from the stream.
      * @return The boolean value true or false.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual bool ReadBool () = 0;
    /** @brief Write a boolean value to the stream.
      * @param value The boolean value to write to the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void WriteBool (bool value) = 0;

    /** The @c bit_count of the BitArray template instance must not be
      * above @code MAX_SUPPORTED_STRING_BUFFER_SIZE * 8 @endcode.
      * @brief Reads a BitArray<bit_count> object from the stream.
      * @returns A BitArray<bit_count> object with the read-in bits
      *          in the least-significant position.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    template <Uint32 bit_count>
    BitArray<bit_count> ReadBitArray ();
    /** The @c bit_count of the BitArray template instance must not be
      * above @code MAX_SUPPORTED_STRING_BUFFER_SIZE * 8 @endcode.
      * @brief Writes a BitArray<bit_count> object to the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    template <Uint32 bit_count>
    void WriteBitArray (BitArray<bit_count> const &value);

    /** The @c bit_count of the BitArray template instance must not be
      * above @code MAX_SUPPORTED_STRING_BUFFER_SIZE * 8 @endcode.
      * @brief Reads @c bits_to_read bits from the stream
      *        into a BitArray<bit_count> object.
      * @param bits_to_read The number of least-significant bits to read
      *                     into the BitArray object.  This value must be
      *                     greater than 0 and less than or equal to
      *                     @c bit_count.
      * @returns A BitArray<bit_count> object with the read-in bits
      *          in the least-significant position.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    template <Uint32 bit_count>
    BitArray<bit_count> ReadBitArray (Uint32 bits_to_read);
    /** The @c bit_count of the BitArray template instance must not be
      * above @code MAX_SUPPORTED_STRING_BUFFER_SIZE * 8 @endcode.
      * @brief Writes @c bits_to_read bits to the stream
      *        from the given BitArray<bit_count> object.
      * @param value The BitArray to write bits from.
      * @param bits_to_write The number of least-significant bits to write
      *                      from the BitArray object.  This value must be
      *                      greater than 0 and less than or equal to
      *                      @c bit_count.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    template <Uint32 bit_count>
    void WriteBitArray (BitArray<bit_count> const &value, Uint32 bits_to_write);

    /** @brief Read a signed, 8-bit integer value from the stream.
      * @return A signed, 8-bit integer value.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual Sint8 ReadSint8 () = 0;
    /** @brief Write a signed, 8-bit integer value to the stream.
      * @param value The signed, 8-bit integer value to write to the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void WriteSint8 (Sint8 value) = 0;

    /** @brief Read an unsigned, 8-bit integer value from the stream.
      * @return An unsigned, 8-bit integer value.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual Uint8 ReadUint8 () = 0;
    /** @brief Write an unsigned, 8-bit integer value to the stream.
      * @param value The unsigned, 8-bit integer value to write to the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void WriteUint8 (Uint8 value) = 0;

    /** @brief Read a signed, 16-bit integer value from the stream.
      * @return A signed, 16-bit integer value.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual Sint16 ReadSint16 () = 0;
    /** @brief Write a signed, 16-bit integer value to the stream.
      * @param value The signed, 16-bit integer value to write to the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void WriteSint16 (Sint16 value) = 0;

    /** @brief Read an unsigned, 16-bit integer value from the stream.
      * @return An unsigned, 16-bit integer value.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual Uint16 ReadUint16 () = 0;
    /** @brief Write an unsigned, 16-bit integer value to the stream.
      * @param value The unsigned, 16-bit integer value to write to the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void WriteUint16 (Uint16 value) = 0;

    /** @brief Read a signed, 32-bit integer value from the stream.
      * @return A signed, 32-bit integer value.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual Sint32 ReadSint32 () = 0;
    /** @brief Write a signed, 32-bit integer value to the stream.
      * @param value The signed, 32-bit integer value to write to the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void WriteSint32 (Sint32 value) = 0;

    /** @brief Read an unsigned, 32-bit integer value from the stream.
      * @return An unsigned, 32-bit integer value.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual Uint32 ReadUint32 () = 0;
    /** @brief Write an unsigned, 32-bit integer value to the stream.
      * @param value The unsigned, 32-bit integer value to write to the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void WriteUint32 (Uint32 value) = 0;

    /** @brief Read an IEEE single-precision floating point value from
      *        the stream.
      * @param destination A pointer to the IEEE single-precision
      *                    floating point value to be set to the value
      *                    read in from the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void ReadFloat (float *destination) = 0;
    /** @brief Write an IEEE single-precision floating point value to
      *        the stream.
      * @param value The IEEE single-precision floating point value to
      *              write to the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void WriteFloat (float value) = 0;

    /** @brief Read an IEEE double-precision floating point value from
      *        the stream.
      * @param destination A pointer to the IEEE double-precision
      *                    floating point value to be set to the value
      *                    read in from the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void ReadFloat (double *destination) = 0;
    /** @brief Write an IEEE double-precision floating point value to
      *        the stream.
      * @param value The IEEE double-precision floating point value to
      *              write to the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void WriteFloat (double value) = 0;

    /** The array @c destination must be already-allocated, and at least
      * @c destination_size elements long.  The number of elements to read is
      * at most @code destination_size - 1 @endcode, though the actual number
      * of elements read may be lower, depending on the content of the
      * stream.  If @c destination is longer than @c destination_size elements,
      * then the extra elements will not be changed.  A null-terminating
      * character will be placed at the end of the read-in string, but will
      * be no later in the string than the element
      * @code destination_size - 1 @endcode.
      * @brief Read a string from the stream into the provided array.
      * @param destination An already-allocated character array which
      *                    will store the string which is to be read in.
      *                    It must be at least @c destination_size elements
      *                    long.
      * @param destination_size The number of characters to read into
      *                         @c destination, including the null-terminating
      *                         character.  Indicates the minimum guaranteed
      *                         array size of @c destination.
      * @return The actual length of the read-in string, not including
      *         the null-terminating character.
      * @note Must guarantee that strings up to 65536 characters long,
      *       including the null-terminating character, are supported.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual Uint32 ReadBufferString (char *destination, Uint32 destination_size) = 0;
    /** The character array @c source must be at least @c source_size elements
      * long.  If it is longer, then the extra elements will be ignored.
      * The string up to either the first null-terminating character or
      * the maximum length will be written to the stream.  If the maximum
      * length is written, without encountering a null-terminating character,
      * a null-terminating character will be written to the stream in
      * place of the last character.
      * @brief Write the given string to the stream, but write no more
      *        than @c source_size characters (including the null-terminating
      *        character).
      * @param source The character string to write to the stream.  Must
      *               be at least @c source_size elements long.
      * @param source_size The maximum possible number of characters to write
      *                    to the stream, including the null-terminating
      *                    character.  Indicates the minimum guaranteed array
      *                    size of @c source.
      * @return The actual length of the written string, not including
      *         the null-terminating character.
      * @note Must not write a string that is longer than 65536 characters,
      *       including the null-terminating character.  If a line is
      *       longer than 65536 characters, including the null-terminating
      *       character, only 65536 characters will be written, including
      *       the null-terminating character.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual Uint32 WriteBufferString (char const *source, Uint32 source_size) = 0;

    /** The null-terminated string assigned to @c *destination is allocated
      * inside the function and must be manually deleted.
      * @brief Reads a character string from the stream.
      * @param destination A pointer to a char * which will be assigned
      *                    with the address of a newly allocated,
      *                    null-terminated string containing the read-in
      *                    contents.  The allocated string must be manually
      *                    deleted.  If an error occurred, null will be
      *                    assigned to @c *destination, and no allocation
      *                    will be made.  This parameter must not be null.
      * @return The length of the read-in string.
      * @note This is more or less an alias for the pure-virtual ReadString().
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual Uint32 ReadString (char **destination);
    /** @brief Writes the given null-terminated string to the stream.
      * @param source The null-terminated string to write to the stream.
      * @note This is more or less an alias for the pure-virtual WriteString().
      * @return The length of the written string.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual Uint32 WriteString (char const *source);

    /** @brief Reads a std::string character string from the stream.
      * @param string_length An optional pointer to a Uint32 which, if not
      *                      null, will be assigned the actual length of the
      *                      read-in string, not including the
      *                      null-terminating character.
      * @return A std::string character string read in from the stream.
      * @note This is more or less an alias for the pure-virtual ReadString().
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    inline std::string ReadStdString (Uint32 *string_length = NULL)
    {
        std::string retval;
        ReadStdString(&retval, string_length);
        return retval;
    }
    /** @brief Reads a std::string character string from the stream.
      * @param destination A pointer to a std::string object to read into.
      * @param string_length An optional pointer to a Uint32 which, if not
      *                      null, will be assigned the actual length of the
      *                      read-in string, not including the
      *                      null-terminating character.
      * @return A std::string character string read in from the stream.
      * @note This is more or less an alias for the pure-virtual ReadString().
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    void ReadStdString (std::string *destination, Uint32 *string_length = NULL);
    /** @brief Writes the given std::string character string to the stream.
      * @param source The std::string character string to write to the stream.
      * @param string_length An optional pointer to a Uint32 which, if not
      *                      null, will be assigned the actual length of the
      *                      read-in string, not including the
      *                      null-terminating character.
      * @note This is more or less an alias for the pure-virtual WriteString().
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    void WriteStdString (std::string const &source, Uint32 *string_length = NULL);

    /** @brief Reads a Color from the stream.
      * @return A Color read in from the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    inline Color ReadColor ()
    {
        Color retval;
        ReadColor(&retval);
        return retval;
    }
    /** @brief Reads a Color from the stream.
      * @param destination A pointer to the Color object to read into.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void ReadColor (Color *destination) = 0;
    /** @brief Writes the given std::string character string to the stream.
      * @param source The std::string character string to write to the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void WriteColor (Color const &value) = 0;

    /** @brief Reads a FloatVector2 from the stream.
      * @return A FloatVector2 read in from the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    inline FloatVector2 ReadFloatVector2 ()
    {
        FloatVector2 retval;
        ReadFloatVector2(&retval);
        return retval;
    }
    /** @brief Reads a FloatVector2 from the stream.
      * @param destination A pointer to the FloatVector2 object to read into.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void ReadFloatVector2 (FloatVector2 *destination) = 0;
    /** @brief Writes the given FloatVector2 to the stream.
      * @param source The FloatVector2 to write to the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void WriteFloatVector2 (FloatVector2 const &value) = 0;

    /** @brief Reads a FloatSimpleTransform2 from the stream.
      * @return A FloatSimpleTransform2 read in from the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    inline FloatSimpleTransform2 ReadFloatSimpleTransform2 ()
    {
        FloatSimpleTransform2 retval;
        ReadFloatSimpleTransform2(&retval);
        return retval;
    }
    /** @brief Reads a FloatSimpleTransform2 from the stream.
      * @param destination A pointer to the FloatSimpleTransform2 object to
      *        read into.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void ReadFloatSimpleTransform2 (FloatSimpleTransform2 *destination) = 0;
    /** @brief Writes the given FloatSimpleTransform2 to the stream.
      * @param source The FloatSimpleTransform2 to write to the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void WriteFloatSimpleTransform2 (FloatSimpleTransform2 const &value) = 0;

    /** @brief Reads a FloatMatrix2 from the stream.
      * @return A FloatMatrix2 read in from the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    inline FloatMatrix2 ReadFloatMatrix2 ()
    {
        FloatMatrix2 retval;
        ReadFloatMatrix2(&retval);
        return retval;
    }
    /** @brief Reads a FloatMatrix2 from the stream.
      * @param destination A pointer to the FloatMatrix2 object to read into.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void ReadFloatMatrix2 (FloatMatrix2 *destination) = 0;
    /** @brief Writes the given FloatMatrix2 to the stream.
      * @param source The FloatMatrix2 to write to the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void WriteFloatMatrix2 (FloatMatrix2 const &value) = 0;

    /** @brief Reads a FloatTransform2 from the stream.
      * @return A FloatTransform2 read in from the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    inline FloatTransform2 ReadFloatTransform2 ()
    {
        FloatTransform2 retval(true);
        ReadFloatTransform2(&retval);
        return retval;
    }
    /** @brief Reads a FloatTransform2 from the stream.
      * @param destination A pointer to the FloatTransform2 object to read
      *        into.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_READ, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void ReadFloatTransform2 (FloatTransform2 *destination) = 0;
    /** @brief Writes the given FloatTransform2 to the stream.
      * @param source The FloatTransform2 to write to the stream.
      * @pre @c GetIsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c GetIsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void WriteFloatTransform2 (FloatTransform2 const &value) = 0;

protected:

    // ///////////////////////////////////////////////////////////////////////
    // protected modifiers
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Used by subclasses of Serializer to set the is-open state.
      * @param is_open The value to set the is-open state to.
      * @post Will not change the error value.
      */
    inline void SetIsOpen (bool is_open)
    {
        m_is_open = is_open;
    }
    /** @brief Used by subclasses of Serializer to set the I/O direction.
      * @param io_direction The value to set the I/O direction to.
      * @post Will not change the error value.
      */
    inline void SetIODirection (IODirection io_direction)
    {
        m_io_direction = io_direction;
    }
    /** @brief Used by subclasses of Serializer to set the error value.
      * @param error The value to set the error to.
      */
    inline void SetError (IOError error) const
    {
        m_error = error;
    }

    // ///////////////////////////////////////////////////////////////////////
    // procedures
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Causes any uncommitted writes to be committed (e.g. writing
      *        a memory buffer to disk, or writing a bit-packing buffer
      *        to a byte-aligned buffer).
      * @pre @c GetIsOpen() must return true, and @c GetIODirection() must
      *      return IOD_WRITE.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void FlushWriteCache () = 0;
    
    /// The current open state.
    bool m_is_open;
    /// The current IO direction.
    IODirection m_io_direction;
    /// The most recent error state, which must be set by each procedure call.
    mutable IOError m_error;
}; // end of class Serializer

// template function definitions for Serializer
#include "xrb_serializer.tcpp"

} // end of namespace Xrb

#endif // !defined(_XRB_SERIALIZER_H_)
