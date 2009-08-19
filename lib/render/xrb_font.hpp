// ///////////////////////////////////////////////////////////////////////////
// xrb_font.hpp by Victor Dods, created 2005/06/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_FONT_HPP_)
#define _XRB_FONT_HPP_

#include "xrb.hpp"

#include <string>
#include <vector>

#include "xrb_enums.hpp"
#include "xrb_fontface.hpp"
#include "xrb_gltexture.hpp"
#include "xrb_ntuple.hpp"
#include "xrb_resourcelibrary.hpp"
#include "xrb_screencoord.hpp"

namespace Xrb
{

class RenderContext;
class Serializer;
class Texture;

/** All fonts must use UTF8 encoding (which conveniently includes standard
  * 7-bit ASCII encoding).  Left-to-right and right-to-left fonts are
  * supported, but vertically-rendered fonts are not.
  * @brief Abstract interface class for all fonts.
  */
class Font
{
public:

    /** @brief Structure used in text formatting (word wrapping and alignment).
      */
    struct LineFormat
    {
        char const *m_ptr;
        ScreenCoord m_width;
        Uint32 m_glyph_count;
    }; // end of struct Font::LineFormat

    typedef std::vector<LineFormat> LineFormatVector;

    static Font *Create (std::string const &font_face_filename, ScreenCoord pixel_height);

    virtual ~Font () { }

    // you must implement at least one Create() method which will be used by ResourceLibrary

    std::string const &FontFaceFilename () const { return m_font_face_filename; }
    ScreenCoord PixelHeight () const { return m_pixel_height; }
    ScreenCoord GlyphWidth (char const *glyph) const
    {
        ScreenCoordVector2 pen_position_26_6(ScreenCoordVector2::ms_zero);
        MoveThroughGlyph(&pen_position_26_6, ScreenCoordVector2::ms_zero, glyph, NULL);
        return Abs(pen_position_26_6[Dim::X] >> 6);
    }

    // returns the rectangle containing the given string as rendered in
    // this font.  the rectangle's lower left corner is at (0, 0).
    ScreenCoordRect StringRect (char const *string) const;
    ScreenCoordRect StringRect (LineFormatVector const &line_format_vector) const;

    // draw the given string, starting at the given position.  the position's
    // meaning is indicated by the return value of InitialPenOrientation().
    void DrawString (
        RenderContext const &render_context,
        ScreenCoordVector2 const &initial_pen_position,
        char const *string) const;

    // generates the formatting necessary to draw text of alignment other than
    // (LEFT, TOP), or text with word-wrapping.
    void GenerateLineFormatVector (
        char const *source_string,
        LineFormatVector *dest_line_format_vector) const;

    // draws formatted text with advanced alignment and/or word-wrapping.
    void DrawLineFormattedText (
        RenderContext const &render_context,
        ScreenCoordRect const &draw_rect,
        char const *source_string,
        LineFormatVector const &line_format_vector,
        Alignment2 const &alignment) const;

    // ///////////////////////////////////////////////////////////////////////
    // public interface methods
    // ///////////////////////////////////////////////////////////////////////

    // moves the given pen position as if the given character was rendered.
    // the previous glyph is supplied so that kerning calculations can be
    // done.  the interpretation of the previous_glyph value is up to the
    // Font subclass implementation (for example, AsciiFont will interpret it
    // directly as an ASCII value, while UnicodeFont will interpret it as
    // a full unicode scalar value).  a value of 0 or newline for previous_glyph
    // indicates there was no previous glyph on this line, and no kerning
    // should be done.  previous_glyph can be NULL, but it can not point to
    // a NULL character.
    //
    // newline characters will increment the row/column, and return the other
    // component of the pen position to the value given by
    // carriage_return_position.
    //
    // tab characters will be interpreted on an implementation-dependent basis.
    //
    // the position must be updated here exactly as it would be by actually
    // drawing each glyph.  pen_position is given in 26.6 fixed point
    virtual void MoveThroughGlyph (
        ScreenCoordVector2 *pen_position_26_6,
        ScreenCoordVector2 const &initial_pen_position,
        char const *current_glyph,
        char const *next_glyph,
        Uint32 *remaining_glyph_count = NULL,
        ScreenCoord *major_space_26_6 = NULL) const = 0;

    // creates a word-wrapped string
    virtual void GenerateWordWrappedString (
        std::string const &source_string,
        std::string *dest_string,
        ScreenCoordVector2 const &text_area_size) const = 0;

protected:

    Font (std::string const &font_face_filename, ScreenCoord pixel_height)
        :
        m_font_face_filename(font_face_filename),
        m_pixel_height(pixel_height)
    { }

    // ///////////////////////////////////////////////////////////////////////
    // protected interface methods
    // ///////////////////////////////////////////////////////////////////////

    // this is called once by DrawString before any calls to DrawGlyph are made
    virtual void DrawGlyphSetup (RenderContext const &render_context) const = 0;
    // this is called once by DrawString after any calls to DrawGlyph are made
    virtual void DrawGlyphShutdown (RenderContext const &render_context) const = 0;
    // draw the given glyph, starting at the given position.  the position's
    // meaning is indicated by the return value of InitialPenOrientation().
    virtual void DrawGlyph (
        RenderContext const &render_context,
        char const *glyph,
        ScreenCoordVector2 const &pen_position_26_6) const = 0;

private:

    // this does not call DrawGlyphSetup or DrawGlyphShutdown
    void DrawStringPrivate (
        RenderContext const &render_context,
        ScreenCoordVector2 const &initial_pen_position,
        char const *string,
        char const *string_terminator = NULL,
        Uint32 remaining_glyph_count = 0,
        ScreenCoord remaining_space = 0) const;
    void TrackBoundingBox (
        ScreenCoordVector2 *pen_position_span_26_6,
        ScreenCoordVector2 const &pen_position_26_6) const;

    std::string const m_font_face_filename;
    ScreenCoord const m_pixel_height;
}; // end of class Font

/** This class uses FontFace to generate pixelized font renderings and
  * stores the resulting data in an OpenGL texture, attempting to use
  * a reasonably small texture to store the set of all rendered glyphs.
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
        ScreenCoordVector2 m_bearing_26_6;
        ScreenCoord m_advance_26_6;
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
      *        filename and pixel height, if said cache exists.
      * @param font_face_filename The filename of the font to load.
      * @param pixel_height The maximum height, in pixels, of the glyphs to render.
      */
    static AsciiFont *CreateFromCache (
        std::string const &font_face_filename,
        ScreenCoord pixel_height);

    /** This is the means to construct a AsciiFont object using the font data
      * provided.  Using this class by dynamically allocated instances is
      * preferred, so that it can be used in @ref Xrb::Resource .  See
      * @ref SDLPal::LoadFont for example usage.
      * @brief Returns a pointer to a new instance of AsciiFont, loaded from the
      *        font metadata and bitmap provided.
      * @param font_face_filename The filename of the font loaded.
      * @param pixel_height The maximum height, in pixels, of the glyphs to render.
      * @param has_kerning True iff the font has kerning enabled.
      * @param baseline_height Pixel offset to the baseline of the font.
      * @param sorted_glyph_specification Data per-glyph
      * @param kern_pair Kerning data.
      * @param font_texture A Texture object holding the font bitmap.  This
      *                     method does not delete the font texture.
      */
    static AsciiFont *Create (
        std::string const &font_face_filename,
        ScreenCoord pixel_height,
        bool has_kerning,
        ScreenCoord baseline_height,
        GlyphSpecification sorted_glyph_specification[RENDERED_GLYPH_COUNT],
        ScreenCoord kern_pair[RENDERED_GLYPH_COUNT*RENDERED_GLYPH_COUNT],
        Texture *font_texture);

    /** Analogous to CreateFromCache.
      * @brief Writes the font metadata and bitmap to disk.
      */
    bool CacheToDisk (Texture *font_texture) const;

    // ///////////////////////////////////////////////////////////////////////
    // public Font interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual void MoveThroughGlyph (
        ScreenCoordVector2 *pen_position_26_6,
        ScreenCoordVector2 const &initial_pen_position,
        char const *current_glyph,
        char const *next_glyph,
        Uint32 *remaining_glyph_count = NULL,
        ScreenCoord *major_space_26_6 = NULL) const;

    virtual void GenerateWordWrappedString (
        std::string const &source_string,
        std::string *dest_string,
        ScreenCoordVector2 const &text_area) const;

protected:

    /** Protected so that you must use @ref Xrb::AsciiFont::Create .
      * @brief Default constructor.
      */
    AsciiFont (std::string const &font_face_filename, ScreenCoord pixel_height)
        :
        Font(font_face_filename, pixel_height)
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
        ScreenCoordVector2 const &pen_position_26_6) const;

private:

    ScreenCoord KernPair (char left, char right) const;

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
    ScreenCoord TokenWidth_26_6 (char const *string) const;

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
    ScreenCoord m_kern_pair[RENDERED_GLYPH_COUNT*RENDERED_GLYPH_COUNT];
    // pointer to the GLTexture containing the font bitmap
    GLTexture *m_gl_texture;
}; // end of class AsciiFont

// ///////////////////////////////////////////////////////////////////////////
// TODO: UnicodeFont (supports an arbitrary set of glyphs; uses UTF8, slow & heavy)
// ///////////////////////////////////////////////////////////////////////////

} // end of namespace Xrb

#endif // !defined(_XRB_FONT_HPP_)
