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

Texture *Texture::Create (ScreenCoordVector2 const &size, InitialState initial_state)
{
    ASSERT1(size[Dim::X] >= 0 && size[Dim::Y] >= 0);

    Texture *retval = Create(size, new Uint8[size[Dim::X]*size[Dim::Y]*4]);
    ASSERT1(retval->m_size == size);
    ASSERT1(retval->m_bit_depth == 32);
    ASSERT1(retval->m_data_length == Uint32(size[Dim::X]*size[Dim::Y]*4));
    if (initial_state == CLEAR)
        memset(retval->m_data, 0, retval->m_data_length);

    return retval;
}

Texture *Texture::Create (ScreenCoordVector2 const &size, Uint8 *data)
{
    ASSERT1(data != NULL && "hand in non-NULL data");
    ASSERT1(size[Dim::X] >= 0 && size[Dim::Y] >= 0);

    Texture *retval = new Texture();
    retval->m_size = size;
    retval->m_bit_depth = 32;
    retval->m_data_length = size[Dim::X]*size[Dim::Y]*4;
    retval->m_data = data;

    return retval;
}

Texture *Texture::CreateMipmap () const
{
    ASSERT1(Math::IsAPowerOf2(Width()) && "this method is supported only for power-of-2-sized textures");
    ASSERT1(Math::IsAPowerOf2(Height()) && "this method is supported only for power-of-2-sized textures");

    ScreenCoordVector2 size(m_size/2);

    Texture *retval = new Texture();
    retval->m_size = size;
    retval->m_bit_depth = 32;
    retval->m_data_length = size[Dim::X]*size[Dim::Y]*4;
    retval->m_data = new Uint8[retval->m_data_length];

    // now filter the data using a simple box filter (4 pixels average down to 1)
    for (ScreenCoord y = 0; y < size[Dim::Y]; ++y)
    {
        ScreenCoord this_y = 2*y;
        for (ScreenCoord x = 0; x < size[Dim::X]; ++x)
        {
            ScreenCoord this_x = 2*x;
            Uint32 v[4] = { 0, 0, 0, 0 };
            for (ScreenCoord yoff = 0; yoff < 2; ++yoff)
            {
                for (ScreenCoord xoff = 0; xoff < 2; ++xoff)
                {
                    Uint8 *pixel = Pixel(this_x+xoff, this_y+yoff);
                    for (Uint32 color = 0; color < 4; ++color)
                    {
                        v[color] += pixel[color];
                    }
                }
            }
            Uint8 *target_pixel = retval->Pixel(x, y);
            for (Uint32 color = 0; color < 4; ++color)
            {
                ASSERT1(v[color] <= 4*255);
                target_pixel[color] = v[color] / 4;
            }
        }
    }

    return retval;
}

Pal::Status Texture::Save (std::string const &path) const
{
    return Singleton::Pal().SaveImage(path.c_str(), *this);
}

Texture::Texture ()
    :
    m_bit_depth(0),
    m_data_length(0),
    m_data(NULL)
{ }

} // end of namespace Xrb
