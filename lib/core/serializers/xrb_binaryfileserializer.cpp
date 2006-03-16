// ///////////////////////////////////////////////////////////////////////////
// xrb_binaryfileserializer by Victor Dods, created 2005/05/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_binaryfileserializer.h"

#include <string.h>

namespace Xrb
{

BinaryFileSerializer::BinaryFileSerializer ()
    :
    Serializer(),
    m_cache(MAX_SUPPORTED_STRING_BUFFER_SIZE, Endian::SERIALIZER)
{
}

BinaryFileSerializer::~BinaryFileSerializer ()
{
}

bool BinaryFileSerializer::GetIsAtEnd () const
{
    return m_cache.GetIsAtEnd();
}

bool BinaryFileSerializer::GetHasFewerThan8BitsLeft () const
{
    return m_cache.GetHasFewerThan8BitsLeft();
}

void BinaryFileSerializer::Open (
    char const *const filename,
    char const *const mode)
{
    ASSERT1(strchr(mode, 't') == NULL &&
            "You cannot specify mode 't' for a binary file")

    // make sure that mode contains a 'b'
    std::string modified_mode(mode);
    if (strchr(modified_mode.c_str(), 'b') == NULL)
        modified_mode += 'b';

    m_cache.Open(filename, modified_mode.c_str());
    SetIsOpen(true);
    SetIODirection(m_cache.GetIODirection());
    SetError(m_cache.GetError());
}

void BinaryFileSerializer::Close ()
{
    m_cache.Close();
    SetIsOpen(false);
    SetIODirection(IOD_NONE);
    SetError(m_cache.GetError());
}

void BinaryFileSerializer::FlushWriteCache ()
{
    ASSERT1(GetIsOpen())
    ASSERT1(GetIODirection() == IOD_WRITE)
    m_cache.FlushWriteCache();
    SetError(m_cache.GetError());
}

Sint32 BinaryFileSerializer::ReadSignedBits (Uint32 const bit_count)
{
    Sint32 retval = m_cache.ReadSignedBits(bit_count);
    SetError(m_cache.GetError());
    return retval;
}

void BinaryFileSerializer::WriteSignedBits (
    Sint32 const value,
    Uint32 const bit_count)
{
    m_cache.WriteSignedBits(value, bit_count);
    SetError(m_cache.GetError());
}

Uint32 BinaryFileSerializer::ReadUnsignedBits (Uint32 const bit_count)
{
    Uint32 retval = m_cache.ReadUnsignedBits(bit_count);
    SetError(m_cache.GetError());
    return retval;
}

void BinaryFileSerializer::WriteUnsignedBits (
    Uint32 const value,
    Uint32 const bit_count)
{
    m_cache.WriteUnsignedBits(value, bit_count);
    SetError(m_cache.GetError());
}

bool BinaryFileSerializer::ReadBool ()
{
    bool retval = m_cache.ReadBool();
    SetError(m_cache.GetError());
    return retval;
}

void BinaryFileSerializer::WriteBool (bool const value)
{
    m_cache.WriteBool(value);
    SetError(m_cache.GetError());
}

Sint8 BinaryFileSerializer::ReadSint8 ()
{
    Sint8 retval;
    m_cache.Read1ByteWordFromCache(&retval);
    SetError(m_cache.GetError());
    return retval;
}

void BinaryFileSerializer::WriteSint8 (Sint8 value)
{
    m_cache.Write1ByteWordToCache(&value);
    SetError(m_cache.GetError());
}

Uint8 BinaryFileSerializer::ReadUint8 ()
{
    Uint8 retval;
    m_cache.Read1ByteWordFromCache(&retval);
    SetError(m_cache.GetError());
    return retval;
}

void BinaryFileSerializer::WriteUint8 (Uint8 value)
{
    m_cache.Write1ByteWordToCache(&value);
    SetError(m_cache.GetError());
}

Sint16 BinaryFileSerializer::ReadSint16 ()
{
    Sint16 retval;
    m_cache.Read2ByteWordFromCache(&retval);
    SetError(m_cache.GetError());
    return retval;
}

void BinaryFileSerializer::WriteSint16 (Sint16 value)
{
    m_cache.Write2ByteWordToCache(&value);
    SetError(m_cache.GetError());
}

Uint16 BinaryFileSerializer::ReadUint16 ()
{
    Uint16 retval;
    m_cache.Read2ByteWordFromCache(&retval);
    SetError(m_cache.GetError());
    return retval;
}

void BinaryFileSerializer::WriteUint16 (Uint16 value)
{
    m_cache.Write2ByteWordToCache(&value);
    SetError(m_cache.GetError());
}

Sint32 BinaryFileSerializer::ReadSint32 ()
{
    Sint32 retval;
    m_cache.Read4ByteWordFromCache(&retval);
    SetError(m_cache.GetError());
    return retval;
}

void BinaryFileSerializer::WriteSint32 (Sint32 value)
{
    m_cache.Write4ByteWordToCache(&value);
    SetError(m_cache.GetError());
}

Uint32 BinaryFileSerializer::ReadUint32 ()
{
    Uint32 retval;
    m_cache.Read4ByteWordFromCache(&retval);
    SetError(m_cache.GetError());
    return retval;
}

void BinaryFileSerializer::WriteUint32 (Uint32 value)
{
    m_cache.Write4ByteWordToCache(&value);
    SetError(m_cache.GetError());
}

void BinaryFileSerializer::ReadFloat (float *const destination)
{
    ASSERT1(sizeof(float) == 4)
    m_cache.Read4ByteWordFromCache(destination);
    SetError(m_cache.GetError());
}

void BinaryFileSerializer::WriteFloat (float value)
{
    ASSERT1(sizeof(float) == 4)
    m_cache.Write4ByteWordToCache(&value);
    SetError(m_cache.GetError());
}

void BinaryFileSerializer::ReadFloat (double *const destination)
{
    ASSERT1(sizeof(double) == 8)
    m_cache.Read8ByteWordFromCache(destination);
    SetError(m_cache.GetError());
}

void BinaryFileSerializer::WriteFloat (double value)
{
    ASSERT1(sizeof(double) == 8)
    m_cache.Write8ByteWordToCache(&value);
    SetError(m_cache.GetError());
}

inline Uint32 BinaryFileSerializer::ReadBufferString (
    char *const destination,
    Uint32 const destination_length)
{
    Uint32 retval = m_cache.ReadBufferString(destination, destination_length);
    SetError(m_cache.GetError());
    return retval;
}

inline Uint32 BinaryFileSerializer::WriteBufferString (
    char const *const source,
    Uint32 const source_length)
{
    Uint32 retval = m_cache.WriteBufferString(source, source_length);
    SetError(m_cache.GetError());
    return retval;
}

Color BinaryFileSerializer::ReadColor ()
{
    Color retval;

    BinaryFileSerializer::ReadFloat(&retval[Dim::R]);
    if (GetError() != IOE_NONE)
        return retval;

    BinaryFileSerializer::ReadFloat(&retval[Dim::G]);
    if (GetError() != IOE_NONE)
        return retval;

    BinaryFileSerializer::ReadFloat(&retval[Dim::B]);
    if (GetError() != IOE_NONE)
        return retval;

    BinaryFileSerializer::ReadFloat(&retval[Dim::A]);
    return retval;
}

void BinaryFileSerializer::WriteColor (Color const &value)
{
    BinaryFileSerializer::WriteFloat(value[Dim::R]);
    if (GetError() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[Dim::G]);
    if (GetError() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[Dim::B]);
    if (GetError() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[Dim::A]);
}

FloatVector2 BinaryFileSerializer::ReadFloatVector2 ()
{
    FloatVector2 retval;

    BinaryFileSerializer::ReadFloat(&retval[Dim::X]);
    if (GetError() != IOE_NONE)
        return retval;

    BinaryFileSerializer::ReadFloat(&retval[Dim::Y]);
    return retval;
}

void BinaryFileSerializer::WriteFloatVector2 (FloatVector2 const &value)
{
    BinaryFileSerializer::WriteFloat(value[Dim::X]);
    if (GetError() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[Dim::Y]);
}

FloatSimpleTransform2 BinaryFileSerializer::ReadFloatSimpleTransform2 ()
{
    FloatSimpleTransform2 retval;

    BinaryFileSerializer::ReadFloat(&retval.m_scale_factors[Dim::X]);
    if (GetError() != IOE_NONE)
        return retval;

    BinaryFileSerializer::ReadFloat(&retval.m_scale_factors[Dim::Y]);
    if (GetError() != IOE_NONE)
        return retval;

    BinaryFileSerializer::ReadFloat(&retval.m_translation[Dim::X]);
    if (GetError() != IOE_NONE)
        return retval;

    BinaryFileSerializer::ReadFloat(&retval.m_translation[Dim::Y]);
    return retval;
}

void BinaryFileSerializer::WriteFloatSimpleTransform2 (
    FloatSimpleTransform2 const &value)
{
    BinaryFileSerializer::WriteFloat(value.m_scale_factors[Dim::X]);
    if (GetError() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value.m_scale_factors[Dim::Y]);
    if (GetError() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value.m_translation[Dim::X]);
    if (GetError() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value.m_translation[Dim::Y]);
}

FloatMatrix2 BinaryFileSerializer::ReadFloatMatrix2 ()
{
    FloatMatrix2 retval;

    BinaryFileSerializer::ReadFloat(&retval[FloatMatrix2::A]);
    if (GetError() != IOE_NONE)
        return retval;

    BinaryFileSerializer::ReadFloat(&retval[FloatMatrix2::B]);
    if (GetError() != IOE_NONE)
        return retval;

    BinaryFileSerializer::ReadFloat(&retval[FloatMatrix2::X]);
    if (GetError() != IOE_NONE)
        return retval;

    BinaryFileSerializer::ReadFloat(&retval[FloatMatrix2::C]);
    if (GetError() != IOE_NONE)
        return retval;

    BinaryFileSerializer::ReadFloat(&retval[FloatMatrix2::D]);
    if (GetError() != IOE_NONE)
        return retval;

    BinaryFileSerializer::ReadFloat(&retval[FloatMatrix2::Y]);
    return retval;
}

void BinaryFileSerializer::WriteFloatMatrix2 (FloatMatrix2 const &value)
{
    BinaryFileSerializer::WriteFloat(value[FloatMatrix2::A]);
    if (GetError() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[FloatMatrix2::B]);
    if (GetError() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[FloatMatrix2::X]);
    if (GetError() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[FloatMatrix2::C]);
    if (GetError() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[FloatMatrix2::D]);
    if (GetError() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[FloatMatrix2::Y]);
}

FloatTransform2 BinaryFileSerializer::ReadFloatTransform2 ()
{
    FloatVector2 translation = BinaryFileSerializer::ReadFloatVector2();
    if (GetError() != IOE_NONE)
        return FloatTransform2(true);

    FloatVector2 scale = BinaryFileSerializer::ReadFloatVector2();
    if (GetError() != IOE_NONE)
        return FloatTransform2(true);

    Float angle;
    BinaryFileSerializer::ReadFloat(&angle);
    if (GetError() != IOE_NONE)
        return FloatTransform2(true);

    bool post_translate = BinaryFileSerializer::ReadBool();
    if (GetError() != IOE_NONE)
        return FloatTransform2(true);

    return FloatTransform2(translation, scale, angle, post_translate);
}

void BinaryFileSerializer::WriteFloatTransform2 (FloatTransform2 const &value)
{
    BinaryFileSerializer::WriteFloatVector2(value.GetTranslation());
    if (GetError() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloatVector2(value.GetScaleFactors());
    if (GetError() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value.GetAngle());
    if (GetError() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteBool(value.GetPostTranslate());
}

} // end of namespace Xrb
