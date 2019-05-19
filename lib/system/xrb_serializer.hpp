// ///////////////////////////////////////////////////////////////////////////
// xrb_serializer.hpp by Victor Dods, created 2005/02/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_SERIALIZER_HPP_)
#define _XRB_SERIALIZER_HPP_

#include "xrb.hpp"

#include <string>
#include <sstream>

#include "xrb_enums.hpp"
#include "xrb_exception.hpp"

namespace Xrb {

/// @brief Provides the abstract interface for serializing data streams.
/// @details The interface is general enough so that more or less any form of I/O
/// can be implemented (e.g. files, network streams, memory buffers, etc).
///
/// Read and Write can only be used to read/write values that can be stored
/// by the CPU: bool, char, wchar_t, Sint##, Uint##, float and double (these
/// will be referred to as "words").
///
/// It is recommended to explicitly specify the template type when using Read
/// or Write so that changing the type of the variable passed to the function
/// doesn't inadvertently change the size of the read/written data.  Example:
///
/// Sint32 value = 123;
/// Write(value); // bad, because if the type of value is changed to Sint16
///               // then the serialized data size is different.
/// Write<Sint32>(value); // good, because if value is changed to a
///                       // Sint16, the serialized data is the same.
///
/// Another important justification is when writing an enum value.  The
/// size of an enum may vary between platforms, so this is a potentially
/// dangerous situation if you care about writing portable code.
///
/// Certain structs and classes (which will be referred to as "aggregates")
/// can be written using ReadAggregate and WriteAggregate.  The aggregates
/// read/write function definitions are made within the respective headers
/// (e.g. @ref xrb_vector.hpp ).  Definitions for std::basic_string<> are included
/// in this header (@ref xrb_serializer.hpp ) for convenience.
///
/// You can add your own definitions for your own custom aggregate types
/// by defining a partial template specialization of the Aggregate struct
/// as seen toward the bottom of this header (@ref xrb_serializer.hpp ).
/// Use the one for std::basic_string<> as an example.
///
/// Again, it is recommended to specify the template type when using
/// ReadAggregate and WriteAggregate, as above with Read and Write.
///
/// Instances of the @ref Exception class will be thrown to indicate error.
class Serializer
{
public:

    /// Enum values for Seek.
    enum SeekRelativeTo { FROM_BEGINNING = 0, FROM_CURRENT_POSITION, FROM_END };
    
    /// The default value of the @c max_allowable_length parameter to @c ReadSizeAndAllocatedArray and @c WriteSizeAndArray.
    static Uint32 const ms_default_max_allowable_length = 0x10000;
    
    Serializer () { }
    virtual ~Serializer () throw() { }

    /// Should return true iff this Serializer can have read operations performed on it.
    /// @note It is permissible for Writable() and Readable() to both return true.
    virtual bool IsReadable () const throw() = 0;
    /// Should return true iff this Serializer can have write operations performed on it.
    /// @note It is permissible for Writable() and Readable() to both return true.
    virtual bool IsWritable () const throw() = 0;
    /// Should return true iff this Serializer supports the ReaderSeek operation.
    virtual bool IsReaderSeekable () const throw() = 0;
    /// Should return true iff this Serializer supports the WriterSeek operation.
    virtual bool IsWriterSeekable () const throw() = 0;
    /// Should return true iff the stream is at the end (e.g. end of file, end of buffer).
    virtual bool IsAtEnd () const = 0;

    /// @brief Sets the position of this Serializer for the purposes of reading if @c IsReaderSeekable returns true.
    /// @details If @c IsReaderSeekable returns false, then an exception will be thrown.
    /// @param relative_to Specifies where the seeking should be done from.  @see SeekRelativeTo.
    /// @param offset The offset to seek to relative to the position specified by @c realative_to.
    /// @note A particular implementation of Serializer does not need to support all combinations of parameters.
    virtual void ReaderSeek (Sint32 offset, SeekRelativeTo relative_to = FROM_BEGINNING) = 0;
    /// @brief Sets the position of this Serializer for the purposes of writing if @c IsWriterSeekable returns true.
    /// @details If @c IsWriterSeekable returns false, then an exception will be thrown.
    /// @param relative_to Specifies where the seeking should be done from.  @see SeekRelativeTo.
    /// @param offset The offset to seek to relative to the position specified by @c realative_to.
    /// @note A particular implementation of Serializer does not need to support all combinations of parameters.
    virtual void WriterSeek (Sint32 offset, SeekRelativeTo relative_to = FROM_BEGINNING) = 0;

    /// @brief Reads the template-specified POD type, returning it by value.  E.g. @code bool initialized = Read<bool>(); @endcode
    /// @details This method is really just a frontend for @code void Read (T &dest); @endcode
    /// @note A POD (Plain Ol' Data) type is a built-in type such as bool, char, wchar_t, Sint##, Uint##, float, and double.
    template <typename T>
    T Read ()
    {
        T retval;
        Read<T>(retval);
        return retval;
    }
    /// @brief Reads the template-specified POD type, storing it in the reference provided.  E.g. @code Uint32 n; Read<Uint32>(n); @endcode
    /// @note A POD (Plain Ol' Data) type is a built-in type such as bool, char, wchar_t, Sint##, Uint##, float, and double.
    template <typename T>
    void Read (T &dest);
    /// @brief Writes the template-specified POD type value provided.  E.g. @code float x = 1.0f; Write<float>(x); @endcode
    /// @note A POD (Plain Ol' Data) type is a built-in type such as bool, char, wchar_t, Sint##, Uint##, float, and double.
    template <typename T>
    void Write (T source);

    /// @brief Reads the template-specified aggregate type, returning it by value.
    /// @details This method is really just a frontend for @code void ReadAggregate (T &dest); @endcode  A partial template
    /// specialization of @code Aggregate<std::string> @endcode has been defined in this header file for convenience, allowing
    /// @code ReadAggregate<std::string>(); @endcode without additional work.
    /// @note An aggregate in a non-POD type for which a partial template specialization for @ref Aggregate has been defined.
    template <typename T>
    T ReadAggregate ()
    {
        T retval;
        ReadAggregate<T>(retval);
        return retval;
    }
    /// @brief Reads the template-specified aggregate type, storing it in the reference provided.
    /// @note An aggregate in a non-POD type for which a partial template specialization for @ref Aggregate has been defined.
    template <typename T>
    void ReadAggregate (T &dest);
    /// @brief Writes the template-specified aggregate type value provided.
    /// @note An aggregate in a non-POD type for which a partial template specialization for @ref Aggregate has been defined.
    template <typename T>
    void WriteAggregate (T const &source);

    /// @brief Reads an array of words from the Serializer, preceded by the number of elements, allocating an array to store the data.
    /// @details Reads a Uint32 (into the @c length parameter) which signifies the number of elements in the array to follow,
    /// allocates an array of that length (the returned pointer is stored in the @c dest parameter), and reads that number
    /// of words into the allocated array.  The @c max_allowable_length parameter provides a way to prevent unintentionally
    /// allocating a giant array and blowing out the system when garbage is read for the size (this can happen due to data
    /// corruption or during the development phase of a program which uses Serializer).
    /// @param dest Must be NULL, and upon return will point to the allocated array.
    /// @param length Will be set to the number of words in the allocated array.
    /// @param max_allowable_length The maximum length this method will tolerate -- an exception will be thrown if the length exceeds it.
    template <typename WordType> void ReadSizeAndAllocatedArray (
        WordType *&dest,
        Uint32 &length,
        Uint32 max_allowable_length = ms_default_max_allowable_length);
    // source must point to a buffer holding at least sizeof(WordType)*length bytes.
    /// @brief Writes an array of words to the Serializer, preceded by the number of elements.
    /// @details Writes the @c length parameter which signifies the number of elements in the array, then writes that number
    /// of elements from the array pointed to by @c source.  The @c max_allowable_length parameter for this method is technically
    /// unnecessary, but allows a way to avoid writing an array that will not be readable using @c ReadSizeAndAllocatedArray.
    /// @param source Must point to an array holding at least @c length elements.
    /// @param length Specifies the number of words to write from the array pointed to by @c source.
    /// @param max_allowable_length The maximum length this method will tolerate -- an exception will be thrown if the length exceeds it.
    template <typename WordType> void WriteSizeAndArray (
        WordType const *source,
        Uint32 length,
        Uint32 max_allowable_length = ms_default_max_allowable_length);

    /// @brief Reads an array of the specified length from the Serializer.
    /// @details The length of the array is known to the program and is not read from the Serializer -- compare with @c ReadSizeAndAllocateArray.
    /// @param dest Must point to an array holding at least @c length elements.
    /// @param length Specifies the number of words to read into the array pointed to by @c dest.
    template <typename WordType> void ReadArray (WordType *dest, Uint32 length);
    // source must point to an array holding at least sizeof(WordType)*length bytes.
    /// @brief Writes an array of the specified length to the Serializer.
    /// @details The length of the array is known to the program and is not written to the Serializer -- compare with @c WriteSizeAndArray.
    /// @param source Must point to an array holding at least @c length elements.
    /// @param length Specifies the number of words to write from the array pointed to by @c source.
    template <typename WordType> void WriteArray (WordType const *source, Uint32 length);

protected:

    /// @brief Reads a specified number of words into memory.
    /// @param dest Should point to a piece of memory at least word_size*word_count bytes in length, where the read data will be put.
    /// @param word_size Specifies the size of each word to read (e.g. sizeof(Sint16) if reading an array of Sint16).
    /// @param word_count Specifies the number of words to read (e.g. the array length if reading an array).
    /// @details This is the only read operation that must be implemented by a Serializer subclass.  Subclasses may perform
    /// byte-order switching on each word during this operation, the specifics of which are left to the implementation.
    /// This method should not be called by anything outside of Serializer.
    virtual void ReadRawWords (Uint8 *dest, Uint32 word_size, Uint32 word_count) = 0;
    /// @brief Writes a specified number of words from memory.
    /// @param source Should point to a piece of memory at least word_size*word_count bytes in length, from which the data will be written.
    /// @param word_size Specifies the size of each word to written (e.g. sizeof(Sint16) if writing an array of Sint16).
    /// @param word_count Specifies the number of words to written (e.g. the array length if writing an array).
    /// @details This is the only write operation that must be implemented by a Serializer subclass.  Subclasses may perform
    /// byte-order switching on each word during this operation, the specifics of which are left to the implementation.
    /// This method should not be called by anything outside of Serializer.
    virtual void WriteRawWords (Uint8 const *source, Uint32 word_size, Uint32 word_count) = 0;
    
private:

    template <typename ScalarType> void ReadScalar (ScalarType &dest);
    template <typename ScalarType> void WriteScalar (ScalarType source);

    template <typename T> friend struct Aggregate;
}; // end of class Serializer

// helper for the ReadAggregate<T>(T &) and WriteAggregate<T> methods
template <typename T>
struct Aggregate
{
    static void Read (Serializer &serializer, T &dest);
    static void Write (Serializer &serializer, T const &source);
};

// ///////////////////////////////////////////////////////////////////////////
// template function definitions
// ///////////////////////////////////////////////////////////////////////////

// the definitions of Serializer::Read<T>(T &) and Serializer::Write<T>(T) are
// type-specific overloads (see below).

template <typename T>
void Serializer::ReadAggregate (T &dest)
{
    Aggregate<T>::Read(*this, dest);
}
template <typename T>
void Serializer::WriteAggregate (T const &source)
{
    Aggregate<T>::Write(*this, source);
}

template <typename WordType>
void Serializer::ReadSizeAndAllocatedArray (WordType *&dest, Uint32 &length, Uint32 max_allowable_length)
{
    ASSERT1(dest == NULL);
    Read<Uint32>(length);
    if (length > max_allowable_length)
        throw Exception("length value read exceeded max_allowable_length");
    dest = new WordType[length];
    ReadArray<WordType>(dest, length);
}
template <typename WordType>
void Serializer::WriteSizeAndArray (WordType const *source, Uint32 length, Uint32 max_allowable_length)
{
    ASSERT1(source != NULL);
    if (length > max_allowable_length)
        throw Exception("length value specified exceeded max_allowable_length");
    Write<Uint32>(length);
    WriteArray<WordType>(source, length);
}

template <typename WordType>
void Serializer::ReadArray (WordType *dest, Uint32 length)
{
    ASSERT1(dest != NULL);
    ReadRawWords(reinterpret_cast<Uint8 *>(dest), sizeof(WordType), length);
}
template <typename WordType>
void Serializer::WriteArray (WordType const *source, Uint32 length)
{
    ASSERT1(source != NULL);
    WriteRawWords(reinterpret_cast<Uint8 const *>(source), sizeof(WordType), length);
}

template <typename ScalarType>
void Serializer::ReadScalar (ScalarType &dest)
{
    ReadRawWords(reinterpret_cast<Uint8 *>(&dest), sizeof(ScalarType), 1);
}
template <typename ScalarType>
void Serializer::WriteScalar (ScalarType source)
{
    WriteRawWords(reinterpret_cast<Uint8 const *>(&source), sizeof(ScalarType), 1);
}

// ///////////////////////////////////////////////////////////////////////////
// template function definitions
// ///////////////////////////////////////////////////////////////////////////

#define DEFINE_READ_AND_WRITE_FOR(Type) \
    template <> \
    inline void Serializer::Read<Type> (Type &dest) \
    { \
        ReadScalar<Type>(dest); \
    } \
    template <> \
    inline void Serializer::Write<Type> (Type source) \
    { \
        WriteScalar<Type>(source); \
    }

// bool is handled uniquely
DEFINE_READ_AND_WRITE_FOR(char)
DEFINE_READ_AND_WRITE_FOR(wchar_t)
DEFINE_READ_AND_WRITE_FOR(Sint8)
DEFINE_READ_AND_WRITE_FOR(Uint8)
DEFINE_READ_AND_WRITE_FOR(Sint16)
DEFINE_READ_AND_WRITE_FOR(Uint16)
DEFINE_READ_AND_WRITE_FOR(Sint32)
DEFINE_READ_AND_WRITE_FOR(Uint32)
#if defined(XRB_ENABLE_64BIT_INTEGER_TYPES)
DEFINE_READ_AND_WRITE_FOR(Sint64)
DEFINE_READ_AND_WRITE_FOR(Uint64)
#endif
DEFINE_READ_AND_WRITE_FOR(float)
DEFINE_READ_AND_WRITE_FOR(double)

// specializations for bool, since sizeof(bool) may vary by machine.
// these make bool take up exactly one byte in the io stream.
template <>
inline void Serializer::Read<bool> (bool &dest)
{
    dest = Read<Uint8>() != 0;
}
template <>
inline void Serializer::Write<bool> (bool source)
{
    Write<Uint8>(Uint8(source ? 0xFF : 0x00));
}

// ///////////////////////////////////////////////////////////////////////////
// only partial template specializations are provided for the Aggregate helper
// so that only certain types may be used in Serializer::ReadAggregate and
// Serializer::WriteAggregate.  make sure that the Read and Write methods
// are declared static.
// ///////////////////////////////////////////////////////////////////////////

template <typename Char, typename Traits, typename Alloc>
struct Aggregate<std::basic_string<Char,Traits,Alloc> >
{
    typedef std::basic_string<Char,Traits,Alloc> StringType; // for convenience

    static void Read (Serializer &serializer, StringType &dest)
    {
        // we essentially do the work of Serializer::ReadSizeAndAllocatedArray here,
        // but take care of the allocation ourselves.
        Uint32 length;
        serializer.Read<Uint32>(length);
        if (length > Serializer::ms_default_max_allowable_length)
            throw Exception("length value read exceeded max_allowable_length");
        dest.resize(length, 'x');
        ASSERT1(dest.length() == length);
        // slight abuse of std::basic_string, but who gives a shit?  this is
        // to avoid std::basic_string's constructor doing a memcpy of a temp
        // buffer which we would just delete anyway.
        serializer.ReadArray<typename StringType::value_type>(
            const_cast<typename StringType::value_type *>(dest.data()),
            length);
    }
    static void Write (Serializer &serializer, StringType const &source)
    {
        serializer.WriteSizeAndArray<typename StringType::value_type>(source.data(), source.length());
    }
};

} // end of namespace Xrb

#endif // !defined(_XRB_SERIALIZER_HPP_)
