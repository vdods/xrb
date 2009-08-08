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

GLTexture *GLTexture::Create (std::string const &filename)
{
    GLTexture *retval = NULL;

    Texture *texture = Texture::Create(filename);
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

//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // font type of clamping
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // font type of clamping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

/*
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        texture->Width(),
        texture->Height(),
        1,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        texture->Data());
*/
    gluBuild2DMipmaps(
        GL_TEXTURE_2D,
        GL_RGBA,
        texture->Width(),
        texture->Height(),
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        texture->Data());

    // the texture data has now been loaded into GL texture memory, so
    // the original texture can now optionally be deleted.
}

void GLTexture::DeleteTexture ()
{
    ASSERT1(m_handle > 0);
    glDeleteTextures(1, &m_handle);
}

} // end of namespace Xrb
