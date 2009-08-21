// ///////////////////////////////////////////////////////////////////////////
// xrb_gltexture.cpp by Victor Dods, created 2005/06/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_gltexture.hpp"

#include "xrb_texture.hpp"

namespace Xrb
{

GLTexture::GLTexture ()
{
    m_handle = 0;
}

GLTexture::~GLTexture ()
{
    DeleteTexture();
}

GLTexture *GLTexture::Create (std::string const &path)
{
    GLTexture *retval = NULL;

    Texture *texture = Texture::Create(path);
    if (texture == NULL)
        return retval;

    retval = new GLTexture();
    retval->GenerateTexture(texture);
    delete texture;

    return retval;
}

GLTexture *GLTexture::Create (Texture *texture)
{
    GLTexture *retval = NULL;

    retval = new GLTexture();
    retval->GenerateTexture(texture);

    return retval;
}

void GLTexture::GenerateTexture (Texture *texture)
{
    ASSERT1(texture != NULL);

    m_size = texture->Size();

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &m_handle);
    glBindTexture(GL_TEXTURE_2D, m_handle);

    // TODO: add parameter to this method to use GL_REPEAT
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // there are restrictions by openGL ES (specifically for the PowerVR MBX
    // platform as used by the ipod touch and older versions of iphones).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // this is the other option within the restriction as described above.
    // it has lower visual quality, but may be faster.
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

    // this causes mipmaps to be generated whenever the base (level 0) texture is changed
    glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    // set the base (level 0) texture
    glTexImage2D(
        GL_TEXTURE_2D,      // target
        0,                  // level
        GL_RGBA,            // internal format
        texture->Width(),   // width
        texture->Height(),  // height
        0,                  // width of the border (must be 0 or 1)
        GL_RGBA,            // format of the input pixel data
        GL_UNSIGNED_BYTE,   // data type of the input pixel data
        texture->Data());   // input pixel data

    // the texture data has now been loaded into GL texture memory, so
    // the original texture can now optionally be deleted.
}

void GLTexture::DeleteTexture ()
{
    ASSERT1(m_handle > 0);
    glDeleteTextures(1, &m_handle);
}

} // end of namespace Xrb
