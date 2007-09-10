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
    ASSERT1(m_face != NULL);
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

    ASSERT1(face != NULL);

    // check if there is a metrics file associated with this font file.
    // (this is sort of a hacky way to check for type1 fonts, but i don't
    // know of any better way).
    if (filename.find_last_of(".pfa") < filename.length() ||
        filename.find_last_of(".pfb") < filename.length())
    {
        std::string metrics_filename(filename.substr(0, filename.length()-4));
        metrics_filename += ".afm";
        // attempt to attach the font metrics file, but ignore errors,
        // since loading this file is not mandatory.
        FT_Attach_File(face, metrics_filename.c_str());

        metrics_filename = filename.substr(0, filename.length()-4);
        metrics_filename += ".pfm";
        // attempt to attach the font metrics file, but ignore errors,
        // since loading this file is not mandatory.
        FT_Attach_File(face, metrics_filename.c_str());
    }

    if (FT_HAS_KERNING(face))
        fprintf(stderr, "FontFace::Create(\"%s\"); loaded font with kerning\n", filename.c_str());
    else
        fprintf(stderr, "FontFace::Create(\"%s\"); loaded font without kerning\n", filename.c_str());

    retval = new FontFace;
    retval->m_face = face;

    return retval;
}

} // end of namespace Xrb
