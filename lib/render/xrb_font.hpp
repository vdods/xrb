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
#include "xrb_ntuple.hpp"
#include "xrb_resourcelibrary.hpp"
#include "xrb_resourceloadparameters.hpp"
#include "xrb_screencoord.hpp"
#include "xrb_singleton.hpp"

namespace Xrb {

class RenderContext;
class Serializer;
class Texture;

// coordinates used by Font and its subclasses, using a 26.6
// fixed point representation (this comes from FreeType).
// the convention is to suffix any variable of type FontCoord
// or FontCoordVector2 with _26_6.
typedef Sint32 FontCoord;
typedef Vector<FontCoord, 2> FontCoordVector2;

#define FONTCOORD_LOWER_BOUND SINT32_LOWER_BOUND
#define FONTCOORD_UPPER_BOUND SINT32_UPPER_BOUND

inline FontCoord ScreenToFontCoord (ScreenCoord v)
{
    return FontCoord(v) << 6;
}
inline ScreenCoord FontToScreenCoord (FontCoord v)
{
    ASSERT1((v >> 6) >= SCREENCOORD_LOWER_BOUND && (v >> 6) <= SCREENCOORD_UPPER_BOUND);
    return ScreenCoord(v >> 6);
}

FontCoordVector2 ScreenToFontCoordVector2 (ScreenCoordVector2 const &v);
ScreenCoordVector2 FontToScreenCoordVector2 (FontCoordVector2 const &v);

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

    class LoadParameters : public ResourceLoadParameters
    {
    public:

        LoadParameters (std::string const &path, ScreenCoord pixel_height)
            :
            m_path(path),
            m_pixel_height(pixel_height)
        {
            ASSERT1(m_pixel_height > 0);
        }

        std::string const &Path () const { return m_path; }
        ScreenCoord PixelHeight () const { return m_pixel_height; }

        virtual std::string ResourceName () const;
        virtual bool IsLessThan (ResourceLoadParameters const &p) const;
        virtual bool IsFallback () const;
        virtual void Fallback ();
        virtual void Print (std::ostream &stream) const;

    private:

        std::string m_path;
        ScreenCoord m_pixel_height;
    }; // end of class Font::LoadParameters

    static Resource<Font> Load (std::string const &path, ScreenCoord pixel_height)
    {
        return Singleton::ResourceLibrary().Load<Font>(Font::Create, new LoadParameters(path, pixel_height));
    }
    static Resource<Font> LoadMissing ()
    {
        LoadParameters *load_parameters = new LoadParameters("", 10); // arbitrary, since Fallback changes this.
        load_parameters->Fallback();
        return Singleton::ResourceLibrary().Load<Font>(Font::Create, load_parameters);
    }

    static Font *Create (ResourceLoadParameters const &p);

    virtual ~Font () { }

    // you must implement at least one Create() method which will be used by ResourceLibrary

    std::string const &FontFacePath () const { return m_font_face_path; }
    ScreenCoord PixelHeight () const { return m_pixel_height; }
    ScreenCoord GlyphWidth (char const *glyph) const
    {
        FontCoordVector2 pen_position_26_6(FontCoordVector2::ms_zero);
        MoveThroughGlyph(&pen_position_26_6, ScreenCoordVector2::ms_zero, glyph, NULL);
        return Abs(FontToScreenCoord(pen_position_26_6[Dim::X]));
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
        FontCoordVector2 *pen_position_26_6,
        ScreenCoordVector2 const &initial_pen_position,
        char const *current_glyph,
        char const *next_glyph,
        Uint32 *remaining_glyph_count = NULL,
        FontCoord *major_space_26_6 = NULL) const = 0;

    // creates a word-wrapped string
    virtual void GenerateWordWrappedString (
        std::string const &source_string,
        std::string *dest_string,
        ScreenCoordVector2 const &text_area_size) const = 0;

protected:

    Font (std::string const &font_face_path, ScreenCoord pixel_height)
        :
        m_font_face_path(font_face_path),
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
        FontCoordVector2 const &pen_position_26_6) const = 0;

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
        FontCoordVector2 *pen_position_span_26_6,
        FontCoordVector2 const &pen_position_26_6) const;

    std::string const m_font_face_path;
    ScreenCoord const m_pixel_height;
}; // end of class Font

// ///////////////////////////////////////////////////////////////////////////
// TODO: UnicodeFont (supports an arbitrary set of glyphs; uses UTF8, slow & heavy)
// ///////////////////////////////////////////////////////////////////////////

} // end of namespace Xrb

#endif // !defined(_XRB_FONT_HPP_)
