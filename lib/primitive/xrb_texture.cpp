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

#include "xrb_math.hpp"
#include "xrb_pal.hpp"

namespace Xrb
{

Texture::~Texture ()
{
    DeleteArrayAndNullify(m_data);
}

Texture *Texture::Create (std::string const &path)
{
    return Singleton::Pal().LoadImage(path.c_str());
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

Texture *Texture::CreateMipmap () const
{
    ASSERT1(Math::IsAPowerOf2(Width()) && Width() > 1 && "this method is supported only for power-of-2 sizes greater than 1");
    ASSERT1(Math::IsAPowerOf2(Height()) && Height() > 1 && "this method is supported only for power-of-2 sizes greater than 1");

    ScreenCoordVector2 size(m_size/2);

    Texture *retval = new Texture();
    retval->m_size = size;
    retval->m_bit_depth = 32;
    retval->m_data_length = size[Dim::X]*size[Dim::Y]*4;
    retval->m_data = new Uint8[retval->m_data_length];

    // now filter the data using a simple box filter (4 pixels average down to 1)
    for (Uint32 y = 0; y < Uint32(size[Dim::Y]); ++y)
    {
        Uint32 this_y = 2*y;
        for (Uint32 x = 0; x < Uint32(size[Dim::X]); ++x)
        {
            Uint32 this_x = 2*x;
            Uint32 v[4] = { 0, 0, 0, 0 };
            for (Uint32 yoff = 0; yoff < 2; ++yoff)
            {
                for (Uint32 xoff = 0; xoff < 2; ++xoff)
                {
                    for (Uint32 color = 0; color < 4; ++color)
                    {
                        v[color] += m_data[((this_y+yoff)*m_size[Dim::X] + (this_x+xoff))*4 + color];
                    }
                }
            }
            for (Uint32 color = 0; color < 4; ++color)
            {
                ASSERT1(v[color]/4 < 256);
                retval->m_data[(y*size[Dim::X] + x)*4 + color] = Uint8(v[color]/4);
            }
        }
    }

    return retval;
}

Texture::Texture ()
    :
    m_bit_depth(0),
    m_data_length(0),
    m_data(NULL)
{ }

} // end of namespace Xrb
