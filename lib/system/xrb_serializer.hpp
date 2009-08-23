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

#include "xrb_color.hpp"
#include "xrb_enums.hpp"
#include "xrb_exception.hpp"
#include "xrb_ntuple.hpp"
#include "xrb_transform2.hpp"
#include "xrb_vector.hpp"

/*
Serializer redesign

idea for endian-handling: write little/big identifier at the beginning of the file, and write in the machine's endian (fast writes of raw bytes), and then on reading the file, do the endian switching (if need be), which can be done in-place since the read destinations are writable anyway.

*/

namespace Xrb
{

/** The interface is general enough so that more or less any form of I/O
  * can be accomplished (e.g. files, network streams, memory buffers, etc).
  *
  * Read and Write can only be used to read/write values that can be stored
  * by the CPU: bool, char, wchar_t, Sint##, Uint##, float and double (these
  * will be referred to as "word types").
  *
  * It is recommended to explicitly specify the template type when using Read
  * or Write so that changing the type of the variable passed to the function
  * doesn't inadvertently change the size of the read/written data.  Example:
  *
  * Sint32 value = 123;
  * Write(value); // bad, because if value is changed to a Sint16
  *               // then the serialized data size is different.
  * Write<Sint32>(value); // good, because if value is changed to a
  *                       // Sint16, the serialized data is the same.
  *
  * Another important justification is when writing an enum value.  The
  * size of an enum may vary between platforms, so this is a potentially
  * dangerous situation if you care about writing portable code.
  *
  * Instances of the @ref Exception class will be thrown to indicate error.
  * @brief Provides the abstract interface for serializing data streams.
  */
class Serializer
{
public:

    Serializer (IODirection direction) throw(Exception)
        :
        m_direction(direction)
    {
        ASSERT1(direction == IOD_READ || direction == IOD_WRITE);
    }
    virtual ~Serializer () throw() { }

    /** @brief Get the I/O direction of the serializer.
      * @return @c IOD_READ if reading, @c IOD_WRITE if writing.
      */
    IODirection Direction () const throw() { return m_direction; }
    /** @brief Get the end-of-stream condition.
      * @return True iff the stream is at the end (e.g. end of file, end of buffer).
      */
    virtual bool IsAtEnd () const throw(Exception) = 0;
    // used for preventing read/write operations from allocating/writing a
    // ridiculously huge array, if the read length is an invalid garbage value.
    virtual Uint32 MaxAllowableArraySize () const throw() = 0;

    // dest must point to an array holding at least word_size*word_count bytes.
    virtual void ReadRawWords (void *dest, Uint32 word_size, Uint32 word_count) throw(Exception) = 0;
    // source must point to an array holding at least word_size*word_count bytes.
    virtual void WriteRawWords (void const *source, Uint32 word_size, Uint32 word_count) throw(Exception) = 0;

    // used for reading/writing word types (bool, char, wchar_t, Sint##,
    // Uint##, float and double).
    template <typename T> T Read () throw(Exception)
    {
        T retval;
        Read<T>(retval);
        return retval;
    }
    template <typename T> void Read (T &dest) throw(Exception);
    template <typename T> void Write (T source) throw(Exception);

    // used for writing certain structs/classes (e.g. std::string, Xrb::Vector<>)
    template <typename T> T ReadAggregate () throw(Exception)
    {
        T retval;
        ReadAggregate<T>(retval);
        return retval;
    }
    template <typename T> void ReadAggregate (T &dest) throw(Exception);
    template <typename T> void WriteAggregate (T const &source) throw(Exception);

    // dest must be NULL, and upon return will point to the allocated buffer.
    template <typename WordType> void ReadSizedBuffer (WordType *&dest, Uint32 &length) throw(Exception);
    // source must point to a buffer holding at least sizeof(WordType)*length bytes.
    template <typename WordType> void WriteSizedBuffer (WordType const *source, Uint32 length) throw(Exception);

    // dest must point to an array holding at least sizeof(WordType)*length bytes.
    template <typename WordType> void ReadBuffer (WordType *dest, Uint32 length) throw(Exception);
    // source must point to an array holding at least sizeof(WordType)*length bytes.
    template <typename WordType> void WriteBuffer (WordType const *source, Uint32 length) throw(Exception);

private:

    template <typename ScalarType> void ReadScalar (ScalarType &dest) throw(Exception);
    template <typename ScalarType> void WriteScalar (ScalarType source) throw(Exception);

    void CheckArraySize (Uint32 size) throw(Exception)
    {
        if (size > MaxAllowableArraySize())
            throw Exception(FORMAT("array too long (got " << size << " bytes, but max allowable is " << MaxAllowableArraySize() << " bytes)"));
    }

    template <typename T> friend struct Aggregate;

    IODirection const m_direction;
}; // end of class Serializer

// helper for the ReadAggregate<T>(T &) and WriteAggregate<T> methods
template <typename T>
struct Aggregate
{
    static void Read (Serializer &serializer, T &dest) throw(Exception);
    static void Write (Serializer &serializer, T const &source) throw(Exception);
};

// ///////////////////////////////////////////////////////////////////////////
// template function definitions
// ///////////////////////////////////////////////////////////////////////////

// the definitions of Serializer::Read<T>(T &) and Serializer::Write<T>(T) are
// type-specific overloads (see below).

template <typename T>
void Serializer::ReadAggregate (T &dest) throw(Exception)
{
    Aggregate<T>::Read(*this, dest);
}
template <typename T>
void Serializer::WriteAggregate (T const &source) throw(Exception)
{
    Aggregate<T>::Write(*this, source);
}

template <typename WordType>
void Serializer::ReadSizedBuffer (WordType *&dest, Uint32 &length) throw(Exception)
{
    ASSERT1(dest == NULL);
    Read<Uint32>(length);
    CheckArraySize(sizeof(WordType)*length);
    dest = new WordType[length];
    ReadBuffer<WordType>(dest, length);
}
template <typename WordType>
void Serializer::WriteSizedBuffer (WordType const *source, Uint32 length) throw(Exception)
{
    ASSERT1(source != NULL);
    CheckArraySize(sizeof(WordType)*length);
    Write<Uint32>(length);
    WriteBuffer<WordType>(source, length);
}

template <typename WordType>
void Serializer::ReadBuffer (WordType *dest, Uint32 length) throw(Exception)
{
    ASSERT1(dest != NULL);
    ReadRawWords(dest, sizeof(WordType), length);
}
template <typename WordType>
void Serializer::WriteBuffer (WordType const *source, Uint32 length) throw(Exception)
{
    ASSERT1(source != NULL);
    WriteRawWords(source, sizeof(WordType), length);
}

template <typename ScalarType>
void Serializer::ReadScalar (ScalarType &dest) throw(Exception)
{
    ReadRawWords(&dest, sizeof(ScalarType), 1);
}
template <typename ScalarType>
void Serializer::WriteScalar (ScalarType source) throw(Exception)
{
    WriteRawWords(&source, sizeof(ScalarType), 1);
}

// ///////////////////////////////////////////////////////////////////////////
// template function definitions
// ///////////////////////////////////////////////////////////////////////////

#define DEFINE_READ_AND_WRITE_FOR(Type) \
    template <> \
    inline void Serializer::Read<Type> (Type &dest) throw(Exception) \
    { \
        ReadScalar<Type>(dest); \
    } \
    template <> \
    inline void Serializer::Write<Type> (Type source) throw(Exception) \
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
inline void Serializer::Read<bool> (bool &dest) throw(Exception)
{
    dest = Read<Uint8>() != 0;
}
template <>
inline void Serializer::Write<bool> (bool source) throw(Exception)
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

    static void Read (Serializer &serializer, StringType &dest) throw(Exception)
    {
        // we essentially do the work of Serializer::ReadSizedBuffer here,
        // but take care of the allocation ourselves.
        Uint32 length;
        serializer.Read<Uint32>(length);
        serializer.CheckArraySize(sizeof(typename StringType::value_type)*length);
        dest.resize(length, 'x');
        ASSERT1(dest.length() == length);
        // slight abuse of std::basic_string, but who gives a shit?  this is
        // to avoid std::basic_string's constructor doing a memcpy of a temp
        // buffer which we would just delete anyway.
        serializer.ReadBuffer<typename StringType::value_type>(
            const_cast<typename StringType::value_type *>(dest.data()),
            length);
    }
    static void Write (Serializer &serializer, StringType const &source) throw(Exception)
    {
        serializer.WriteSizedBuffer<typename StringType::value_type>(source.data(), source.length());
    }
};

template <typename T, Uint32 dimension>
struct Aggregate<Vector<T,dimension> >
{
    static void Read (Serializer &serializer, Vector<T,dimension> &dest) throw(Exception)
    {
        serializer.ReadBuffer<T>(dest.m, LENGTHOF(dest.m));
    }
    static void Write (Serializer &serializer, Vector<T,dimension> const &source) throw(Exception)
    {
        serializer.WriteBuffer<T>(source.m, LENGTHOF(source.m));
    }
};

template <typename T>
struct Aggregate<Transform2<T> >
{
    static void Read (Serializer &serializer, Transform2<T> &dest) throw(Exception)
    {
        {
            FloatVector2 translation;
            serializer.ReadAggregate<FloatVector2>(translation);
            dest.SetTranslation(translation);
        }
        {
            FloatVector2 scale_factors;
            serializer.ReadAggregate<FloatVector2>(scale_factors);
            dest.SetScaleFactors(scale_factors);
        }
        {
            Float angle;
            serializer.Read<Float>(angle);
            dest.SetAngle(angle);
        }
        {
            bool post_translate;
            serializer.Read<bool>(post_translate);
            dest.SetPostTranslate(post_translate);
        }
    }
    static void Write (Serializer &serializer, Transform2<T> const &source) throw(Exception)
    {
        serializer.WriteAggregate<FloatVector2>(source.Translation());
        serializer.WriteAggregate<FloatVector2>(source.ScaleFactors());
        serializer.Write<Float>(source.Angle());
        serializer.Write<bool>(source.PostTranslate());
    }
};

template <>
struct Aggregate<Color>
{
    static void Read (Serializer &serializer, Color &dest) throw(Exception)
    {
        serializer.ReadBuffer<ColorCoord>(dest.m, LENGTHOF(dest.m));
    }
    static void Write (Serializer &serializer, Color const &source) throw(Exception)
    {
        serializer.WriteBuffer<ColorCoord>(source.m, LENGTHOF(source.m));
    }
};

template <typename T, Uint32 size>
struct Aggregate<NTuple<T,size> >
{
    static void Read (Serializer &serializer, NTuple<T,size> &dest) throw(Exception)
    {
        serializer.ReadBuffer<T>(dest.m, LENGTHOF(dest.m));
    }
    static void Write (Serializer &serializer, NTuple<T,size> const &source) throw(Exception)
    {
        serializer.WriteBuffer<T>(source.m, LENGTHOF(source.m));
    }
};

} // end of namespace Xrb

#endif // !defined(_XRB_SERIALIZER_HPP_)
