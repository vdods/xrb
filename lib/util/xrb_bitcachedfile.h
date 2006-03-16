// ///////////////////////////////////////////////////////////////////////////
// xrb_bitcachedfile.h by Victor Dods, created 2005/05/20
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_BITCACHEDFILE_H_)
#define _XRB_BITCACHEDFILE_H_

#include "xrb.h"

#include "xrb_bitcache.h"

#include <stdio.h>
#include <string>

namespace Xrb
{

class BitCachedFile : public BitCache
{
public:

    BitCachedFile (
        Uint32 cache_size_in_bytes,
        Endian::Endianness endianness)
        :
        BitCache(cache_size_in_bytes, endianness)
    { }
    virtual ~BitCachedFile () { }

    void Open (char const *filename, char const *mode);

    // this makes virtual calls and may not be used in the destructor.
    // non-virtual override.
    void Close ();

protected:

    virtual Uint32 FlushBytes (
        Uint8 const *bytes_to_flush,
        Uint32 number_of_bytes_to_flush) const;
    virtual Uint32 RenewBytes (
        Uint8 *bytes_to_renew,
        Uint32 number_of_bytes_to_renew) const;

private:

    // stdio file handle of the currently opened file
    FILE *m_file_handle;
    // the filename of the currently opened file.
    std::string m_filename;
    // mode argument to fopen() of the currently opened file.
    // e.g. "rt". or "wb+", etc (see the man page for fopen)
    std::string m_mode;
}; // end of class BitCachedFile

} // end of namespace Xrb

#endif // !defined(_XRB_BITCACHEDFILE_H_)
