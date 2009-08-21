// ///////////////////////////////////////////////////////////////////////////
// xrb_binaryfileserializer by Victor Dods, created 2005/05/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_binaryfileserializer.hpp"

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

bool BinaryFileSerializer::IsAtEnd () const
{
    return m_cache.IsAtEnd();
}

bool BinaryFileSerializer::HasFewerThan8BitsLeft () const
{
    return m_cache.HasFewerThan8BitsLeft();
}

void BinaryFileSerializer::Open (
    char const *const path,
    char const *const mode)
{
    ASSERT1(strchr(mode, 't') == NULL &&
            "You cannot specify mode 't' for a binary file");

    // make sure that mode contains a 'b'
    std::string modified_mode(mode);
    if (strchr(modified_mode.c_str(), 'b') == NULL)
        modified_mode += 'b';

    m_cache.Open(path, modified_mode.c_str());
    SetIsOpen(m_cache.IsOpen());
    SetIODirection(m_cache.GetIODirection());
    SetError(m_cache.Error());
}

void BinaryFileSerializer::Close ()
{
    m_cache.Close();
    SetIsOpen(false);
    SetIODirection(IOD_NONE);
    SetError(m_cache.Error());
}

void BinaryFileSerializer::FlushWriteCache ()
{
    ASSERT1(IsOpen());
    ASSERT1(GetIODirection() == IOD_WRITE);
    m_cache.FlushWriteCache();
    SetError(m_cache.Error());
}

Sint32 BinaryFileSerializer::ReadSignedBits (Uint32 const bit_count)
{
    Sint32 retval = m_cache.ReadSignedBits(bit_count);
    SetError(m_cache.Error());
    return retval;
}

void BinaryFileSerializer::WriteSignedBits (
    Sint32 const value,
    Uint32 const bit_count)
{
    m_cache.WriteSignedBits(value, bit_count);
    SetError(m_cache.Error());
}

Uint32 BinaryFileSerializer::ReadUnsignedBits (Uint32 const bit_count)
{
    Uint32 retval = m_cache.ReadUnsignedBits(bit_count);
    SetError(m_cache.Error());
    return retval;
}

void BinaryFileSerializer::WriteUnsignedBits (
    Uint32 const value,
    Uint32 const bit_count)
{
    m_cache.WriteUnsignedBits(value, bit_count);
    SetError(m_cache.Error());
}

bool BinaryFileSerializer::ReadBool ()
{
    bool retval = m_cache.ReadBool();
    SetError(m_cache.Error());
    return retval;
}

void BinaryFileSerializer::WriteBool (bool const value)
{
    m_cache.WriteBool(value);
    SetError(m_cache.Error());
}

Sint8 BinaryFileSerializer::ReadSint8 ()
{
    Sint8 retval;
    m_cache.Read1ByteWordFromCache(&retval);
    SetError(m_cache.Error());
    return retval;
}

void BinaryFileSerializer::WriteSint8 (Sint8 value)
{
    m_cache.Write1ByteWordToCache(&value);
    SetError(m_cache.Error());
}

Uint8 BinaryFileSerializer::ReadUint8 ()
{
    Uint8 retval;
    m_cache.Read1ByteWordFromCache(&retval);
    SetError(m_cache.Error());
    return retval;
}

void BinaryFileSerializer::WriteUint8 (Uint8 value)
{
    m_cache.Write1ByteWordToCache(&value);
    SetError(m_cache.Error());
}

Sint16 BinaryFileSerializer::ReadSint16 ()
{
    Sint16 retval;
    m_cache.Read2ByteWordFromCache(&retval);
    SetError(m_cache.Error());
    return retval;
}

void BinaryFileSerializer::WriteSint16 (Sint16 value)
{
    m_cache.Write2ByteWordToCache(&value);
    SetError(m_cache.Error());
}

Uint16 BinaryFileSerializer::ReadUint16 ()
{
    Uint16 retval;
    m_cache.Read2ByteWordFromCache(&retval);
    SetError(m_cache.Error());
    return retval;
}

void BinaryFileSerializer::WriteUint16 (Uint16 value)
{
    m_cache.Write2ByteWordToCache(&value);
    SetError(m_cache.Error());
}

Sint32 BinaryFileSerializer::ReadSint32 ()
{
    Sint32 retval;
    m_cache.Read4ByteWordFromCache(&retval);
    SetError(m_cache.Error());
    return retval;
}

void BinaryFileSerializer::WriteSint32 (Sint32 value)
{
    m_cache.Write4ByteWordToCache(&value);
    SetError(m_cache.Error());
}

Uint32 BinaryFileSerializer::ReadUint32 ()
{
    Uint32 retval;
    m_cache.Read4ByteWordFromCache(&retval);
    SetError(m_cache.Error());
    return retval;
}

void BinaryFileSerializer::WriteUint32 (Uint32 value)
{
    m_cache.Write4ByteWordToCache(&value);
    SetError(m_cache.Error());
}

void BinaryFileSerializer::ReadFloat (float *const destination)
{
    m_cache.Read4ByteWordFromCache(destination);
    SetError(m_cache.Error());
}

void BinaryFileSerializer::WriteFloat (float value)
{
    m_cache.Write4ByteWordToCache(&value);
    SetError(m_cache.Error());
}

void BinaryFileSerializer::ReadFloat (double *const destination)
{
    m_cache.Read8ByteWordFromCache(destination);
    SetError(m_cache.Error());
}

void BinaryFileSerializer::WriteFloat (double value)
{
    m_cache.Write8ByteWordToCache(&value);
    SetError(m_cache.Error());
}

inline Uint32 BinaryFileSerializer::ReadBufferString (
    char *const destination,
    Uint32 const destination_length)
{
    Uint32 retval = m_cache.ReadBufferString(destination, destination_length);
    SetError(m_cache.Error());
    return retval;
}

inline Uint32 BinaryFileSerializer::WriteBufferString (
    char const *const source,
    Uint32 const source_length)
{
    Uint32 retval = m_cache.WriteBufferString(source, source_length);
    SetError(m_cache.Error());
    return retval;
}

void BinaryFileSerializer::ReadColor (Color *const destination)
{
    ASSERT1(destination != NULL);

    BinaryFileSerializer::ReadFloat(&(*destination)[Dim::R]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::ReadFloat(&(*destination)[Dim::G]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::ReadFloat(&(*destination)[Dim::B]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::ReadFloat(&(*destination)[Dim::A]);
}

void BinaryFileSerializer::WriteColor (Color const &value)
{
    BinaryFileSerializer::WriteFloat(value[Dim::R]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[Dim::G]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[Dim::B]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[Dim::A]);
}

void BinaryFileSerializer::ReadScreenCoordVector2 (ScreenCoordVector2 *destination)
{
    ASSERT1(destination != NULL);

    (*destination)[Dim::X] = BinaryFileSerializer::ReadScreenCoord();
    if (Error() != IOE_NONE)
        return;

    (*destination)[Dim::Y] = BinaryFileSerializer::ReadScreenCoord();
}

void BinaryFileSerializer::WriteScreenCoordVector2 (ScreenCoordVector2 const &value)
{
    BinaryFileSerializer::WriteScreenCoord(value[Dim::X]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteScreenCoord(value[Dim::Y]);
}

void BinaryFileSerializer::ReadFloatVector2 (FloatVector2 *const destination)
{
    ASSERT1(destination != NULL);

    BinaryFileSerializer::ReadFloat(&(*destination)[Dim::X]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::ReadFloat(&(*destination)[Dim::Y]);
}

void BinaryFileSerializer::WriteFloatVector2 (FloatVector2 const &value)
{
    BinaryFileSerializer::WriteFloat(value[Dim::X]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[Dim::Y]);
}

void BinaryFileSerializer::ReadFloatSimpleTransform2 (
    FloatSimpleTransform2 *const destination)
{
    ASSERT1(destination != NULL);

    BinaryFileSerializer::ReadFloat(&(*destination).m_scale_factors[Dim::X]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::ReadFloat(&(*destination).m_scale_factors[Dim::Y]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::ReadFloat(&(*destination).m_translation[Dim::X]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::ReadFloat(&(*destination).m_translation[Dim::Y]);
}

void BinaryFileSerializer::WriteFloatSimpleTransform2 (
    FloatSimpleTransform2 const &value)
{
    BinaryFileSerializer::WriteFloat(value.m_scale_factors[Dim::X]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value.m_scale_factors[Dim::Y]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value.m_translation[Dim::X]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value.m_translation[Dim::Y]);
}

void BinaryFileSerializer::ReadFloatMatrix2 (FloatMatrix2 *const destination)
{
    ASSERT1(destination != NULL);

    BinaryFileSerializer::ReadFloat(&(*destination)[FloatMatrix2::A]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::ReadFloat(&(*destination)[FloatMatrix2::B]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::ReadFloat(&(*destination)[FloatMatrix2::X]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::ReadFloat(&(*destination)[FloatMatrix2::C]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::ReadFloat(&(*destination)[FloatMatrix2::D]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::ReadFloat(&(*destination)[FloatMatrix2::Y]);
}

void BinaryFileSerializer::WriteFloatMatrix2 (FloatMatrix2 const &value)
{
    BinaryFileSerializer::WriteFloat(value[FloatMatrix2::A]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[FloatMatrix2::B]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[FloatMatrix2::X]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[FloatMatrix2::C]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[FloatMatrix2::D]);
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value[FloatMatrix2::Y]);
}

void BinaryFileSerializer::ReadFloatTransform2 (
    FloatTransform2 *const destination)
{
    ASSERT1(destination != NULL);

    {
        FloatVector2 translation;
        BinaryFileSerializer::ReadFloatVector2(&translation);
        if (Error() != IOE_NONE)
            return;
        destination->SetTranslation(translation);
    }

    {
        FloatVector2 scale_factors;
        BinaryFileSerializer::ReadFloatVector2(&scale_factors);
        if (Error() != IOE_NONE)
            return;
        destination->SetScaleFactors(scale_factors);
    }

    {
        Float angle;
        BinaryFileSerializer::ReadFloat(&angle);
        if (Error() != IOE_NONE)
            return;
        destination->SetAngle(angle);
    }

    {
        bool post_translate = BinaryFileSerializer::ReadBool();
        if (Error() != IOE_NONE)
            return;
        destination->SetPostTranslate(post_translate);
    }
}

void BinaryFileSerializer::WriteFloatTransform2 (FloatTransform2 const &value)
{
    BinaryFileSerializer::WriteFloatVector2(value.Translation());
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloatVector2(value.ScaleFactors());
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteFloat(value.Angle());
    if (Error() != IOE_NONE)
        return;

    BinaryFileSerializer::WriteBool(value.PostTranslate());
}

} // end of namespace Xrb
