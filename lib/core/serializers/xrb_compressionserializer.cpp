// ///////////////////////////////////////////////////////////////////////////
// xrb_compressionserializer.cpp by Victor Dods, created 2005/05/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_compressionserializer.h"

namespace Xrb
{

CompressionSerializer::CompressionSerializer (Huffman const *huffman)
    :
    Serializer(),
    m_cache(huffman, MAX_SUPPORTED_STRING_BUFFER_SIZE, Endian::SERIALIZER)
{
}

CompressionSerializer::~CompressionSerializer ()
{
    ASSERT1(GetAttachedSerializer() == NULL && "You must detach the serializer before the CompressionSerializer is destroyed")
}

bool CompressionSerializer::GetIsAtEnd () const
{
    return m_cache.GetIsAtEnd();
}

bool CompressionSerializer::GetHasFewerThan8BitsLeft () const
{
    return m_cache.GetHasFewerThan8BitsLeft();
}

void CompressionSerializer::AttachSerializer (Serializer *serializer)
{
    ASSERT1(serializer != NULL)
    ASSERT1(serializer->GetIsOpen())
    ASSERT1(serializer->GetIODirection() == IOD_READ ||
            serializer->GetIODirection() == IOD_WRITE)
    ASSERT1(!GetIsOpen())

    m_cache.AttachSerializer(serializer);
    SetIsOpen(true);
    SetIODirection(serializer->GetIODirection());
    SetError(IOE_NONE);
}

void CompressionSerializer::DetachSerializer ()
{
    ASSERT1(GetIsOpen())

    m_cache.DetachSerializer();
    SetIsOpen(false);
    SetIODirection(IOD_NONE);
    SetError(IOE_NONE);
}

Sint32 CompressionSerializer::ReadSignedBits (Uint32 bit_count)
{
    Sint32 retval = m_cache.ReadSignedBits(bit_count);
    SetError(m_cache.GetError());
    return retval;
}

void CompressionSerializer::WriteSignedBits (Sint32 value, Uint32 bit_count)
{
    m_cache.WriteSignedBits(value, bit_count);
    SetError(m_cache.GetError());
}

Uint32 CompressionSerializer::ReadUnsignedBits (Uint32 bit_count)
{
    Uint32 retval = m_cache.ReadUnsignedBits(bit_count);
    SetError(m_cache.GetError());
    return retval;
}

void CompressionSerializer::WriteUnsignedBits (Uint32 value, Uint32 bit_count)
{
    m_cache.WriteUnsignedBits(value, bit_count);
    SetError(m_cache.GetError());
}

bool CompressionSerializer::ReadBool ()
{
    bool retval = m_cache.ReadBool();
    SetError(m_cache.GetError());
    return retval;
}

void CompressionSerializer::WriteBool (bool value)
{
    m_cache.WriteBool(value);
    SetError(m_cache.GetError());
}

Sint8 CompressionSerializer::ReadSint8 ()
{
    Sint8 retval;
    m_cache.Read1ByteWordFromCache(&retval);
    SetError(m_cache.GetError());
    return retval;
}

void CompressionSerializer::WriteSint8 (Sint8 value)
{
    m_cache.Write1ByteWordToCache(&value);
    SetError(m_cache.GetError());
}

Uint8 CompressionSerializer::ReadUint8 ()
{
    Uint8 retval;
    m_cache.Read1ByteWordFromCache(&retval);
    SetError(m_cache.GetError());
    return retval;
}

void CompressionSerializer::WriteUint8 (Uint8 value)
{
    m_cache.Write1ByteWordToCache(&value);
    SetError(m_cache.GetError());
}

Sint16 CompressionSerializer::ReadSint16 ()
{
    Sint16 retval;
    m_cache.Read2ByteWordFromCache(&retval);
    SetError(m_cache.GetError());
    return retval;
}

void CompressionSerializer::WriteSint16 (Sint16 value)
{
    m_cache.Write2ByteWordToCache(&value);
    SetError(m_cache.GetError());
}

Uint16 CompressionSerializer::ReadUint16 ()
{
    Uint16 retval;
    m_cache.Read2ByteWordFromCache(&retval);
    SetError(m_cache.GetError());
    return retval;
}

void CompressionSerializer::WriteUint16 (Uint16 value)
{
    m_cache.Write2ByteWordToCache(&value);
    SetError(m_cache.GetError());
}

Sint32 CompressionSerializer::ReadSint32 ()
{
    Sint32 retval;
    m_cache.Read4ByteWordFromCache(&retval);
    SetError(m_cache.GetError());
    return retval;
}

void CompressionSerializer::WriteSint32 (Sint32 value)
{
    m_cache.Write4ByteWordToCache(&value);
    SetError(m_cache.GetError());
}

Uint32 CompressionSerializer::ReadUint32 ()
{
    Uint32 retval;
    m_cache.Read4ByteWordFromCache(&retval);
    SetError(m_cache.GetError());
    return retval;
}

void CompressionSerializer::WriteUint32 (Uint32 value)
{
    m_cache.Write4ByteWordToCache(&value);
    SetError(m_cache.GetError());
}

void CompressionSerializer::ReadFloat (float *destination)
{
    m_cache.Read4ByteWordFromCache(destination);
    SetError(m_cache.GetError());
}

void CompressionSerializer::WriteFloat (float value)
{
    m_cache.Write4ByteWordToCache(&value);
    SetError(m_cache.GetError());
}

void CompressionSerializer::ReadFloat (double *destination)
{
    m_cache.Read8ByteWordFromCache(destination);
    SetError(m_cache.GetError());
}

void CompressionSerializer::WriteFloat (double value)
{
    m_cache.Write8ByteWordToCache(&value);
    SetError(m_cache.GetError());
}

inline Uint32 CompressionSerializer::ReadBufferString (
    char *destination,
    Uint32 destination_length)
{
    Uint32 retval = m_cache.ReadBufferString(destination, destination_length);
    SetError(m_cache.GetError());
    return retval;
}

inline Uint32 CompressionSerializer::WriteBufferString (
    char const *source,
    Uint32 source_length)
{
    Uint32 retval = m_cache.WriteBufferString(source, source_length);
    SetError(m_cache.GetError());
    return retval;
}

//////////////////
////////////////////
///////////////////
////////////////////
//////////////////
//////////////////

Color CompressionSerializer::ReadColor ()
{
    Color retval;

    CompressionSerializer::ReadFloat(&retval[Dim::R]);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval[Dim::G]);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval[Dim::B]);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval[Dim::A]);
    return retval;
}

void CompressionSerializer::WriteColor (Color const &value)
{
    CompressionSerializer::WriteFloat(value[Dim::R]);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value[Dim::G]);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value[Dim::B]);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value[Dim::A]);
}

FloatVector2 CompressionSerializer::ReadFloatVector2 ()
{
    FloatVector2 retval;

    CompressionSerializer::ReadFloat(&retval[Dim::X]);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval[Dim::Y]);
    return retval;
}

void CompressionSerializer::WriteFloatVector2 (FloatVector2 const &value)
{
    CompressionSerializer::WriteFloat(value[Dim::X]);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value[Dim::Y]);
}

FloatSimpleTransform2 CompressionSerializer::ReadFloatSimpleTransform2 ()
{
    FloatSimpleTransform2 retval;

    CompressionSerializer::ReadFloat(&retval.m_scale_factors[Dim::X]);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval.m_scale_factors[Dim::Y]);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval.m_translation[Dim::X]);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval.m_translation[Dim::Y]);
    return retval;
}

void CompressionSerializer::WriteFloatSimpleTransform2 (
    FloatSimpleTransform2 const &value)
{
    CompressionSerializer::WriteFloat(value.m_scale_factors[Dim::X]);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value.m_scale_factors[Dim::Y]);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value.m_translation[Dim::X]);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value.m_translation[Dim::Y]);
}

FloatMatrix2 CompressionSerializer::ReadFloatMatrix2 ()
{
    FloatMatrix2 retval;

    CompressionSerializer::ReadFloat(&retval[FloatMatrix2::A]);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval[FloatMatrix2::B]);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval[FloatMatrix2::X]);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval[FloatMatrix2::C]);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval[FloatMatrix2::D]);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval[FloatMatrix2::Y]);
    return retval;
}

void CompressionSerializer::WriteFloatMatrix2 (FloatMatrix2 const &value)
{
    CompressionSerializer::WriteFloat(value[FloatMatrix2::A]);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value[FloatMatrix2::B]);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value[FloatMatrix2::X]);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value[FloatMatrix2::C]);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value[FloatMatrix2::D]);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value[FloatMatrix2::Y]);
}

FloatTransform2 CompressionSerializer::ReadFloatTransform2 ()
{
    FloatVector2 translation = CompressionSerializer::ReadFloatVector2();
    if (GetError() != IOE_NONE)
        return FloatTransform2(true);

    FloatVector2 scale = CompressionSerializer::ReadFloatVector2();
    if (GetError() != IOE_NONE)
        return FloatTransform2(true);

    Float angle;
    CompressionSerializer::ReadFloat(&angle);
    if (GetError() != IOE_NONE)
        return FloatTransform2(true);

    bool post_translate = CompressionSerializer::ReadBool();
    if (GetError() != IOE_NONE)
        return FloatTransform2(true);

    return FloatTransform2(translation, scale, angle, post_translate);
}

void CompressionSerializer::WriteFloatTransform2 (FloatTransform2 const &value)
{
    CompressionSerializer::WriteFloatVector2(value.GetTranslation());
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloatVector2(value.GetScaleFactors());
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value.GetAngle());
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteBool(value.GetPostTranslate());
}
/*
Color CompressionSerializer::ReadColor ()
{
    Color retval;

    CompressionSerializer::ReadFloat(&retval[Dim::R]);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval[Dim::G]);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval[Dim::B]);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval[Dim::A]);
    return retval;
}

void CompressionSerializer::WriteColor (Color const &value)
{
    CompressionSerializer::WriteFloat(value[Dim::R]);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value[Dim::G]);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value[Dim::B]);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value[Dim::A]);
}

FloatVector2 CompressionSerializer::ReadFloatVector2 ()
{
    FloatVector2 retval(FloatVector2::ms_zero);

    CompressionSerializer::ReadFloat(&retval[Dim::X]);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval[Dim::Y]);
    return retval;
}

void CompressionSerializer::WriteFloatVector2 (FloatVector2 const &value)
{
    CompressionSerializer::WriteFloat(value[Dim::X]);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value[Dim::Y]);
}

FloatSimpleTransform2 CompressionSerializer::ReadFloatSimpleTransform2 ()
{
    FloatSimpleTransform2 retval(true);

    CompressionSerializer::ReadFloat(&retval.m_r);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval.m_s);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval.m_x);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval.m_y);
    return retval;
}

void CompressionSerializer::WriteSimpleTransform2 (SimpleTransform2 const &value)
{
    CompressionSerializer::WriteFloat(value.m_r);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value.m_s);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value.m_x);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value.m_y);
}

Matrix2 CompressionSerializer::ReadMatrix2 ()
{
    Matrix2 retval(true);

    CompressionSerializer::ReadFloat(&retval.m_a);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval.m_b);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval.m_c);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval.m_d);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval.m_e);
    if (GetError() != IOE_NONE)
        return retval;

    CompressionSerializer::ReadFloat(&retval.m_f);
    return retval;
}

void CompressionSerializer::WriteMatrix2 (Matrix2 const &value)
{
    CompressionSerializer::WriteFloat(value.m_a);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value.m_b);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value.m_c);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value.m_d);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value.m_e);
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value.m_f);
}

Transform2 CompressionSerializer::ReadTransform2 ()
{
    Vector2 translation = CompressionSerializer::ReadVector2();
    if (GetError() != IOE_NONE)
        return Transform2(true);

    Vector2 scale = CompressionSerializer::ReadVector2();
    if (GetError() != IOE_NONE)
        return Transform2(true);

    Float angle;
    CompressionSerializer::ReadFloat(&angle);
    if (GetError() != IOE_NONE)
        return Transform2(true);

    bool post_translate = CompressionSerializer::ReadBool();
    if (GetError() != IOE_NONE)
        return Transform2(true);

    return Transform2(translation, scale, angle, post_translate);
}

void CompressionSerializer::WriteTransform2 (Transform2 const &value)
{
    CompressionSerializer::WriteVector2(value.GetTranslation());
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteVector2(value.GetScale());
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteFloat(value.GetRotation());
    if (GetError() != IOE_NONE)
        return;

    CompressionSerializer::WriteBool(value.GetPostTranslate());
}
*/
void CompressionSerializer::FlushWriteCache ()
{
    m_cache.FlushWriteCache();
}

} // end of namespace Xrb
