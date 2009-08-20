// ///////////////////////////////////////////////////////////////////////////
// xrb_texture.cpp by Victor Dods, created 2005/04/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_texture.hpp"

#include <string.h>

#include "xrb_pal.hpp"

namespace Xrb
{

Texture::~Texture ()
{
    DeleteArrayAndNullify(m_data);
}

Texture *Texture::Create (std::string const &filename)
{
    return Singleton::Pal().LoadImage(filename.c_str());
}

Texture *Texture::Create (
    ScreenCoordVector2 const &size,
    bool const zero_out_the_data)
{
    ASSERT1(size[Dim::X] > 0 && size[Dim::Y] > 0);

    Texture *retval = Create(size, new Uint8[size[Dim::X]*size[Dim::Y]*4]);
    ASSERT1(retval->m_size == size);
    ASSERT1(retval->m_bit_depth == 32);
    ASSERT1(retval->m_data_length == Uint32(size[Dim::X]*size[Dim::Y]*4));
    if (zero_out_the_data)
        memset(retval->m_data, 0, retval->m_data_length);

    return retval;
}

Texture *Texture::Create (ScreenCoordVector2 const &size, Uint8 *data)
{
    ASSERT1(data != NULL && "hand in non-NULL data");

    ASSERT1(size[Dim::X] > 0 && size[Dim::Y] > 0);

    Texture *retval = new Texture();
    retval->m_size = size;
    retval->m_bit_depth = 32;
    retval->m_data_length = size[Dim::X]*size[Dim::Y]*4;
    retval->m_data = data;

    return retval;
}

Texture::Texture ()
    :
    m_bit_depth(0),
    m_data_length(0),
    m_data(NULL)
{ }

} // end of namespace Xrb
