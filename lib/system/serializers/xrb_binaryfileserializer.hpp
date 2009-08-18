// ///////////////////////////////////////////////////////////////////////////
// xrb_binaryfileserializer.hpp by Victor Dods, created 2005/05/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_BINARYFILESERIALIZER_HPP_)
#define _XRB_BINARYFILESERIALIZER_HPP_

#include "xrb.hpp"

#include "xrb_serializer.hpp"
#include "xrb_bitcachedfile.hpp"
#include "xrb_endian.hpp"

namespace Xrb
{

// implements the Serializer interface for binary files.
class BinaryFileSerializer : public Serializer
{
public:

    BinaryFileSerializer ();
    virtual ~BinaryFileSerializer ();

    virtual bool IsAtEnd () const;
    virtual bool HasFewerThan8BitsLeft () const;

    void Open (char const *filename, char const *mode);
    void Close ();

    // the following are bit-packed

    virtual Sint32 ReadSignedBits (Uint32 bit_count);
    virtual void WriteSignedBits (Sint32 value, Uint32 bit_count);

    virtual Uint32 ReadUnsignedBits (Uint32 bit_count);
    virtual void WriteUnsignedBits (Uint32 value, Uint32 bit_count);

    virtual bool ReadBool ();
    virtual void WriteBool (bool value);

    // the following are byte-aligned

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

    virtual ScreenCoord ReadScreenCoord ();
    virtual void WriteScreenCoord (ScreenCoord value);

    virtual void ReadFloat (float *destination);
    virtual void WriteFloat (float value);

    virtual void ReadFloat (double *destination);
    virtual void WriteFloat (double value);

    virtual Uint32 ReadBufferString (char *destination, Uint32 destination_length);
    virtual Uint32 WriteBufferString (char const *source, Uint32 source_length);

    virtual void ReadColor (Color *destination);
    virtual void WriteColor (Color const &value);

    virtual void ReadScreenCoordVector2 (ScreenCoordVector2 *destination);
    virtual void WriteScreenCoordVector2 (ScreenCoordVector2 const &value);

    virtual void ReadFloatVector2 (FloatVector2 *destination);
    virtual void WriteFloatVector2 (FloatVector2 const &value);

    virtual void ReadFloatSimpleTransform2 (FloatSimpleTransform2 *destination);
    virtual void WriteFloatSimpleTransform2 (FloatSimpleTransform2 const &value);

    virtual void ReadFloatMatrix2 (FloatMatrix2 *destination);
    virtual void WriteFloatMatrix2 (FloatMatrix2 const &value);

    virtual void ReadFloatTransform2 (FloatTransform2 *destination);
    virtual void WriteFloatTransform2 (FloatTransform2 const &value);

protected:

    // causes any unused bits in the current byte to be skipped
    virtual void FlushWriteCache ();

private:

    /// The BitCache object implemented for file i/o, which will be used
    /// to do the dirty work of bit packing, caching and file i/o.
    BitCachedFile m_cache;
}; // end of class BinaryFileSerializer

} // end of namespace Xrb

#endif // !defined(_XRB_BINARYFILESERIALIZER_HPP_)
