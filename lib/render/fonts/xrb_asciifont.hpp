// ///////////////////////////////////////////////////////////////////////////
// xrb_asciifont.hpp by Victor Dods, created 2005/06/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ASCIIFONT_HPP_)
#define _XRB_ASCIIFONT_HPP_

#include "xrb.hpp"

#include <string>

#include "xrb_font.hpp"
#include "xrb_screencoord.hpp"

namespace Xrb
{

class GLTexture;
class RenderContext;
class Serializer;
class Texture;

/** This class generates pixelized font renderings and stores the
  * resulting data in an OpenGL texture, attempting to use a reasonably
  * small texture to store the set of all rendered glyphs.
  * @brief Fast and light @a Font implementation for 7-bit-ASCII text.
  */
class AsciiFont : public Font
{
public:

    /** @brief Destructor.  Deletes the gl texture.
      */
    virtual ~AsciiFont ()
    {
        Delete(m_gl_texture);
    }

    // ///////////////////////////////////////////////////////////////////////
    // font metrics and font bitmap rendering stuff
    // ///////////////////////////////////////////////////////////////////////

    enum
    {
        TAB_SIZE = 4,

        RENDERED_GLYPH_LOWEST = ' ',
        RENDERED_GLYPH_HIGHEST = '~',

        RENDERED_GLYPH_COUNT = RENDERED_GLYPH_HIGHEST - RENDERED_GLYPH_LOWEST + 1
    };

    struct GlyphSpecification
    {
        char m_ascii;
        ScreenCoordVector2 m_size;
        FontCoordVector2 m_bearing_26_6;
        FontCoord m_advance_26_6;
        ScreenCoordVector2 m_texture_coordinates;

        void Read (Serializer &serializer);
        void Write (Serializer &serializer) const;

        static int SortByWidthFirst (
            void const *left_operand,
            void const *right_operand);
        static int SortByHeightFirst (
            void const *left_operand,
            void const *right_operand);
    }; // end of class GlyphSpecification

    static Uint32 GlyphIndex (char ascii)
    {
        if (ascii >= RENDERED_GLYPH_LOWEST && ascii <= RENDERED_GLYPH_HIGHEST)
            return ascii - RENDERED_GLYPH_LOWEST;
        else
            return ms_error_glyph - RENDERED_GLYPH_LOWEST;
    }
    static char AsciiValue (Uint32 glyph_index)
    {
        if (glyph_index >= RENDERED_GLYPH_COUNT)
            return char(ms_error_glyph);
        else
            return glyph_index + RENDERED_GLYPH_LOWEST;
    }

    /** This is the means to construct a AsciiFont object from cached data on disk.
      * Using this class by dynamically allocated instances is preferred, so that
      * it can be used in @ref Xrb::Resource .
      * @brief Returns a pointer to a new instance of AsciiFont, loaded from the
      *        cached font metadata and bitmap files corresponding to the given
      *        path and pixel height, if said cache exists.
      * @param font_face_path The path of the font to load.
      * @param pixel_height The maximum height, in pixels, of the glyphs to render.
      */
    static AsciiFont *CreateFromCache (
        std::string const &font_face_path,
        ScreenCoord pixel_height);

    /** This is the means to construct a AsciiFont object using the font data
      * provided.  Using this class by dynamically allocated instances is
      * preferred, so that it can be used in @ref Xrb::Resource .  See
      * @ref SDLPal::LoadFont for example usage.
      * @brief Returns a pointer to a new instance of AsciiFont, loaded from the
      *        font metadata and bitmap provided.
      * @param font_face_path The path of the font loaded.
      * @param pixel_height The maximum height, in pixels, of the glyphs to render.
      * @param has_kerning True iff the font has kerning enabled.
      * @param baseline_height Pixel offset to the baseline of the font.
      * @param sorted_glyph_specification Data per-glyph
      * @param kern_pair_26_6 Kerning data in 26.6 fixed point format.
      * @param font_texture A Texture object holding the font bitmap.  This
      *                     method does not delete the font texture.
      */
    static AsciiFont *Create (
        std::string const &font_face_path,
        ScreenCoord pixel_height,
        bool has_kerning,
        ScreenCoord baseline_height,
        GlyphSpecification sorted_glyph_specification[RENDERED_GLYPH_COUNT],
        FontCoord kern_pair_26_6[RENDERED_GLYPH_COUNT*RENDERED_GLYPH_COUNT],
        Texture *font_texture);

    /** Analogous to CreateFromCache.
      * @brief Writes the font metadata and bitmap to disk.
      */
    bool CacheToDisk (Texture *font_texture) const;

    // ///////////////////////////////////////////////////////////////////////
    // public Font interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual void MoveThroughGlyph (
        FontCoordVector2 *pen_position_26_6,
        ScreenCoordVector2 const &initial_pen_position,
        char const *current_glyph,
        char const *next_glyph,
        Uint32 *remaining_glyph_count = NULL,
        FontCoord *major_space_26_6 = NULL) const;

    virtual void GenerateWordWrappedString (
        std::string const &source_string,
        std::string *dest_string,
        ScreenCoordVector2 const &text_area) const;

protected:

    /** Protected so that you must use @ref Xrb::AsciiFont::Create .
      * @brief Default constructor.
      */
    AsciiFont (std::string const &font_face_path, ScreenCoord pixel_height)
        :
        Font(font_face_path, pixel_height)
    {
        m_gl_texture = NULL;
    }

    // ///////////////////////////////////////////////////////////////////////
    // protected Font interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual void DrawGlyphSetup (RenderContext const &render_context) const;
    virtual void DrawGlyphShutdown (RenderContext const &render_context) const;
    virtual void DrawGlyph (
        RenderContext const &render_context,
        char const *glyph,
        FontCoordVector2 const &pen_position_26_6) const;

private:

    FontCoord KernPair_26_6 (char left, char right) const;

    // ///////////////////////////////////////////////////////////////////////
    // text justification stuff
    // ///////////////////////////////////////////////////////////////////////

    enum TokenClass
    {
        WHITESPACE = 0,
        WORD,
        NEWLINE,
        NULLCHAR
    }; // end of enum TokenClass

    static TokenClass GetTokenClass (char const c);
    static char const *StartOfNextToken (char const *string);
    FontCoord TokenWidth_26_6 (char const *string) const;

    // ///////////////////////////////////////////////////////////////////////
    // member vars
    // ///////////////////////////////////////////////////////////////////////

    // the unicode value of the "error glyph"
    static Uint32 const ms_error_glyph = '?';

    // true iff the font has kerning enabled
    bool m_has_kerning;
    // height from bottom of font glyph coordinates
    ScreenCoord m_baseline_height;
    // metadata for each glyph (texture coordinates, etc)
    GlyphSpecification m_glyph_specification[RENDERED_GLYPH_COUNT];
    // cached kerning data -- all possible pairs of glyphs
    FontCoord m_kern_pair_26_6[RENDERED_GLYPH_COUNT*RENDERED_GLYPH_COUNT];
    // pointer to the GLTexture containing the font bitmap
    GLTexture *m_gl_texture;
}; // end of class AsciiFont

} // end of namespace Xrb

#endif // !defined(_XRB_ASCIIFONT_HPP_)
