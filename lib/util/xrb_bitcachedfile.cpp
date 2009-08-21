// ///////////////////////////////////////////////////////////////////////////
// xrb_bitcachedfile.cpp by Victor Dods, created 2005/05/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_bitcachedfile.hpp"

#include <string.h>

namespace Xrb
{

void BitCachedFile::Open (
    char const *const path,
    char const *const mode)
{
    ASSERT1(path != NULL);
    ASSERT1(mode != NULL);
    ASSERT1(!IsOpen());

    if (strlen(path) == 0)
    {
        SetError(IOE_INVALID_FILENAME);
        return;
    }

    // check for the presense of exactly one of 'r', 'w' or 'a'
    // for the stdio fopen mode string.
    bool mode_r = strchr(mode, 'r') != NULL;
    bool mode_w = strchr(mode, 'w') != NULL;
    bool mode_a = strchr(mode, 'a') != NULL;
    Uint32 mode_count = mode_r ? 1 : 0 +
                        mode_w ? 1 : 0 +
                        mode_a ? 1 : 0;
    // check for the presense of exactly one of 'b' or 't'
    // for the stdio fopen mode string.
    bool mode_b = strchr(mode, 'b') != NULL;
    bool mode_t = strchr(mode, 't') != NULL;
    Uint32 type_count = mode_b ? 1 : 0 +
                        mode_t ? 1 : 0;
    if (mode_count != 1 || type_count != 1)
    {
        SetError(IOE_INVALID_FILE_OPEN_MODE);
        return;
    }

    m_file_handle = fopen(path, mode);
    if (m_file_handle == NULL)
    {
        SetError(IOE_UNABLE_TO_OPEN_FILE);
        return;
    }

    m_path = path;
    m_mode = mode;

    if (mode_r)
        OpenForReading();
    else
        OpenForWriting();
}

void BitCachedFile::Close ()
{
    ASSERT1(IsOpen());

    BitCache::Close();

    if (fclose(m_file_handle) != 0)
        ASSERT1(false && "fclose() failed.");

    m_file_handle = NULL;
    m_path.clear();
    m_mode.clear();
}

Uint32 BitCachedFile::FlushBytes (
    Uint8 const *const bytes_to_flush,
    Uint32 const number_of_bytes_to_flush) const
{
    ASSERT1(bytes_to_flush != NULL);
    ASSERT1(number_of_bytes_to_flush > 0);
    ASSERT1(IsOpen());
    ASSERT1(GetIODirection() == IOD_WRITE);
    ASSERT1(m_file_handle != NULL);
    Uint32 bytes_written =
        fwrite(
            bytes_to_flush,
            sizeof(Uint8),
            number_of_bytes_to_flush,
            m_file_handle);

    return bytes_written;
}

Uint32 BitCachedFile::RenewBytes (
    Uint8 *const bytes_to_renew,
    Uint32 const number_of_bytes_to_renew) const
{
    ASSERT1(bytes_to_renew != NULL);
    ASSERT1(number_of_bytes_to_renew > 0);
    ASSERT1(IsOpen());
    ASSERT1(GetIODirection() == IOD_READ);
    ASSERT1(m_file_handle != NULL);
    Uint32 bytes_read =
        fread(
            bytes_to_renew,
            sizeof(Uint8),
            number_of_bytes_to_renew,
            m_file_handle);

    return bytes_read;
}

} // end of namespace Xrb
