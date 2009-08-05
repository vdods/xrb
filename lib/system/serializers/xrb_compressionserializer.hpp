// ///////////////////////////////////////////////////////////////////////////
// xrb_compressionserializer.hpp by Victor Dods, created 2005/05/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_COMPRESSIONSERIALIZER_HPP_)
#define _XRB_COMPRESSIONSERIALIZER_HPP_

#include "xrb.hpp"

#include "xrb_serializer.hpp"
#include "xrb_bitcachedcompression.hpp"
#include "xrb_huffman.hpp"

namespace Xrb
{

class CompressionSerializer : public Serializer
{
public:

    /** @brief Construct a CompressionSerializer with the given Huffman
      *        object to be used for encoding to/decoding from a
      *        Serializer object which will be specified later.
      * @param huffman The Huffman object which contains the codes for the
      *                specific type of en/decoding that should be done.
      *                May not be null.
      * @post @c IsOpen() will return false.  You must use
      *       @c AttachSerializer() to attach the serializer which will do
      *       the actual reading/writing.
      */
    CompressionSerializer (Huffman const *huffman);
    virtual ~CompressionSerializer ();

    virtual bool IsAtEnd () const;
    virtual bool GetHasFewerThan8BitsLeft () const;

    /** @brief Attached serializer accessor.
      * @return The currently attached serializer, or null if none
      *         is attached currently.
      * @note Will not change the error value.
      */
    inline Serializer *AttachedSerializer () const
    {
        return m_cache.AttachedSerializer();
    }

    /** Once a serializer is attached in this manner, it must not itself
      * be closed (or even closed and reopened) until it is detached from
      * this object -- otherwise the data caching will not function properly.
      * @brief Attaches a serializer to use as the source/destination for
      *        the compressed data resulting from reads/writes on this
      *        object.
      * @param serializer The serializer to use for reading/writing; the
      *                   serializer must be open.  It must not be a
      *                   CompressionSerializer.
      * @pre There must not currently be a serializer attached (i.e.
      *      @c AttachedSerializer() must return null), and @c serializer
      *      must be open (its IsOpen() method must return true).
      * @post The error state is set to indicate the status of the operation.
      *       This does not cause the newly attached serializer to
      *       @c FlushWriteCache().
      */
    void AttachSerializer (Serializer *serializer);
    /** @brief Detaches the currently attached serializer from this object.
      * @pre There must currently be a serializer attached (i.e.
      *      @c AttachedSerializer() must return null), and the attached
      *      serializer must be open (its IsOpen() method must return true).
      * @post The error state is set to indicate the status of the operation.
      *       This does not cause the previously attached serializer to
      *       @c FlushWriteCache().
      */
    void DetachSerializer ();

    // ///////////////////////////////////////////////////////////////////////
    // reading/writing functions
    // ///////////////////////////////////////////////////////////////////////

    virtual Sint32 ReadSignedBits (Uint32 bit_count);
    virtual void WriteSignedBits (Sint32 value, Uint32 bit_count);

    virtual Uint32 ReadUnsignedBits (Uint32 bit_count);
    virtual void WriteUnsignedBits (Uint32 value, Uint32 bit_count);

    virtual bool ReadBool ();
    virtual void WriteBool (bool value);

    virtual Sint8 ReadSint8 ();
    virtual void WriteSint8 (Sint8 value);

    virtual Uint8 ReadUint8 ();
    virtual void WriteUint8 (Uint8 value);

    virtual Sint16 ReadSint16 ();
    virtual void WriteSint16 (Sint16 value);

    virtual Uint16 ReadUint16 ();
    virtual void WriteUint16 (Uint16 value);

    virtual Sint32 ReadSint32 ();
    virtual void WriteSint32 (Sint32 value);

    virtual Uint32 ReadUint32 ();
    virtual void WriteUint32 (Uint32 value);

    virtual void ReadFloat (float *destination);
    virtual void WriteFloat (float value);

    virtual void ReadFloat (double *destination);
    virtual void WriteFloat (double value);

    virtual Uint32 ReadBufferString (char *destination, Uint32 destination_size);
    virtual Uint32 WriteBufferString (char const *source, Uint32 source_size);

    virtual Color ReadColor ();
    virtual void WriteColor (Color const &value);

    virtual FloatVector2 ReadFloatVector2 ();
    virtual void WriteFloatVector2 (FloatVector2 const &value);

    virtual FloatSimpleTransform2 ReadFloatSimpleTransform2 ();
    virtual void WriteFloatSimpleTransform2 (FloatSimpleTransform2 const &value);

    virtual FloatMatrix2 ReadFloatMatrix2 ();
    virtual void WriteFloatMatrix2 (FloatMatrix2 const &value);

    virtual FloatTransform2 ReadFloatTransform2 ();
    virtual void WriteFloatTransform2 (FloatTransform2 const &value);

protected:

    /** This will call @code AttachedSerializer()->FlushWriteCache() @endcode
      * if @c AttachedSerializer() is not null.
      * @brief Causes any uncommitted writes to be committed (e.g. writing
      *        a memory buffer to disk, or writing a bit-packing buffer
      *        to a byte-aligned buffer).
      * @pre @c IsOpen() must return true, @c GetIODirection() must
      *      return IOD_WRITE, and @c IsAtEnd() must return false.
      * @post The error state is set to indicate the status of the operation.
      */
    virtual void FlushWriteCache ();

private:

    /// The BitCache object implemented for Huffman encoded serializer i/o,
    /// which will be used to do the dirty work of bit packing, caching and
    /// serializer i/o.
    BitCachedCompression m_cache;
}; // end of class CompressionSerializer

} // end of namespace Xrb

#endif // !defined(_XRB_COMPRESSIONSERIALIZER_HPP_)
