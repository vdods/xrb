// ///////////////////////////////////////////////////////////////////////////
// xrb_fontface.hpp by Victor Dods, created 2005/06/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_FONTFACE_HPP_)
#define _XRB_FONTFACE_HPP_

#include "xrb.hpp"

#include <string>

#include "xrb_screencoord.hpp"

// FT_Face is typedef'ed as a pointer to one of these.
// this is a forward declaration so that a the idiotic pointer
// type hiding that FreeType does can be circumvented.
struct FT_FaceRec_;

namespace Xrb
{

/** Loading a font face is only half of what's necessary to actually use
  * the font in rendering.  The font face stores the vectorized font data,
  * from which the pixel-rendered font textures can be made.
  * @brief Class for loading scalable fonts using the FreeType library.
  */
class FontFace
{
public:

    /// Destructor.
    ~FontFace ();

    /** This is the means to construct a FontFace object.  Using this class
      * by pointed-to instances is preferred, so that it can be used in
      * @ref Xrb::Resource .
      * @brief Returns a pointer to a new instance of FontFace, loaded
      *        from the given filename.
      * @param filename The filename of the font face to load.
      */
    static FontFace *Create (std::string const &filename);

    /** The structure is necessary for FreeType font rendering calls.
      * @brief Returns the FreeType font face structure.
      */
    inline FT_FaceRec_ *FTFace () const
    {
        return m_face;
    }

protected:

    /** Protected so that you must use @ref Xrb::FontFace::Create .
      * @brief Default constructor.
      */
    FontFace ();

private:

    /** @brief Pointer to FreeType font face structure, loaded by
      *        @ref Xrb::FontFace::Create .
      */
    FT_FaceRec_ *m_face;
}; // end of class FontFace

} // end of namespace Xrb

#endif // !defined(_XRB_FONTFACE_HPP_)

