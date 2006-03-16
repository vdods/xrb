// ///////////////////////////////////////////////////////////////////////////
// xrb_fontface.cpp by Victor Dods, created 2005/06/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_fontface.h"

#include "ft2build.h"
#include FT_FREETYPE_H

namespace Xrb
{

// this font face class should control only creating the rendered
// font glyphs.  it should not know anything about drawing them,
// or text alignment or justification.

FontFace::FontFace ()
{
}

FontFace::~FontFace ()
{
    ASSERT1(m_face != NULL)
    FT_Done_Face(m_face);
    m_face = NULL;
}

FontFace *FontFace::Create (std::string const &filename)
{
    FontFace *retval = NULL;
    FT_Error error;
    FT_FaceRec_ *face;

    error = FT_New_Face(
        Singletons::FTLibrary(),
        filename.c_str(),
        0,
        &face);
    if (error != 0)
        return retval;

    ASSERT1(face != NULL)
//     ASSERT1((face->internal->flags & FT_FACE_FLAG_SCALABLE) != 0)

    retval = new FontFace;
    retval->m_face = face;

    return retval;
}

} // end of namespace Xrb
