// ///////////////////////////////////////////////////////////////////////////
// xrb_font.h by Victor Dods, created 2005/06/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_FONT_H_)
#define _XRB_FONT_H_

#include "xrb.h"

#include <string>

#include "xrb_fontface.h"
#include "xrb_gltexture.h"
#include "xrb_ntuple.h"
#include "xrb_resourcelibrary.h"
#include "xrb_screencoord.h"

namespace Xrb
{

class RenderContext;
class Texture;

/** This class uses FontFace to generate pixelized font renderings and
  * stores the resulting data in an OpenGL texture, attempting to use
  * a reasonably small texture to store the set of all rendered glyphs.
  * @brief The font class which is used to render fonts to screen.
  */
class Font
{
public:

    /** @brief Destructor.  Deletes the gl texture.
      */
    inline ~Font ()
    {
        Delete(m_gl_texture);
    }

    /** This is the means to construct a Font object.  Using this class
      * by pointed-to instances is preferred, so that it can be used in
      * @ref Xrb::Resource .
      * @brief Returns a pointer to a new instance of Font, loaded from the
      *        given filename, with glyphs rendered to the given maximum
      *        height.
      * @param filename The filename of the font to load.
      * @param pixel_height The maximum height, in pixels, of the glyphs
      *                     to render.
      */
    static Font *Create (
        std::string const &filename,
        ScreenCoord pixel_height);
    /** This is the means to construct a Font object.  Using this class
      * by pointed-to instances is preferred, so that it can be used in
      * @ref Xrb::Resource .
      * @brief Returns a pointer to a new instance of Font, generated using
      *        the given font face, with glyphs rendered to the given maximum
      *        height.
      * @param font_face The FontFace to use when rendering glyphs.
      * @param pixel_height The maximum height, in pixels, of the glyphs
      *                     to render.
      */
    static Font *Create (
        Resource<FontFace> const &font_face,
        ScreenCoord pixel_height);

    /** @brief Returns the font face that was used to generate this Font.
      */
    inline Resource<FontFace> const &GetFontFace () const
    {
        return m_font_face;
    }
    /** @brief Returns the pixel height used to generate this Font.
      */
    inline ScreenCoord GetPixelHeight () const
    {
        return m_pixel_height;
    }
    /** @brief Returns the pixel height of a particular glyph.
      * @param unicode The unicode value of the glyph to be queried.
      */
    inline ScreenCoordVector2 GetGlyphPixelSize (Uint32 const unicode) const
    {
        return m_glyph_specification[GetGlyphIndex(unicode)].m_size;
    }
    /** @brief Returns the "advance" property of a particular glyph (see
      *        FreeType documentation).
      * @param unicode The unicode value of the glyph to be queried.
      */
    inline ScreenCoord GetGlyphPixelAdvance (Uint32 const unicode) const
    {
        return m_glyph_specification[GetGlyphIndex(unicode)].m_advance;
    }
    /** @brief Returns the horizontal kerning offset between the given glyphs.
      * @param left The unicode value of the (visually) left glyph.
      * @param right The unicode value of the (visually) right glyph.
      */
    ScreenCoord GetKerningPixelAdvance (Uint32 left, Uint32 right) const;
    /** @brief Returns the "bearing" property of a particular glyph (see
      *        FreeType documentation).
      * @param unicode The unicode value of the glyph to be queried.
      */
    inline ScreenCoordVector2 GetGlyphBearing (Uint32 const unicode) const
    {
        return m_glyph_specification[GetGlyphIndex(unicode)].m_bearing;
    }
    /** @brief Returns the top-left-corner OpenGL texture coordinates of a
      *        particular glyph (remember, texture coords are left-handed).
      * @param unicode The unicode value of the glyph to be queried.
      */
    inline ScreenCoordVector2 GetGlyphTextureCoordinates (Uint32 const unicode) const
    {
        return
            m_glyph_specification[GetGlyphIndex(unicode)].m_texture_coordinates;
    }
    /** The string is not justified.  It is formatted exactly as given,
      * newlines and all.
      * @brief Returns a rectangle exactly large enough to bound the glyphs
      *        in the given string, if it were rendered.
      */
    ScreenCoordRect GetStringRect (char const *string) const;
    /** The string is not justified.  It is formatted exactly as given,
      * newlines and all.
      * @brief Returns a the width of the given string, if it were rendered.
      */
    ScreenCoord GetStringWidth (char const *string) const;
    /** The string is not justified.  It is formatted exactly as given,
      * newlines and all.
      * @brief Returns a the height of the given string, if it were rendered.
      */
    ScreenCoord GetStringHeight (char const *string) const;
    /** @brief Returns the OpenGL texture handle associated with this Font.
      */
    inline GLuint GetTextureHandle () const
    {
        ASSERT1(m_gl_texture != NULL)
        return m_gl_texture->GetHandle();
    }
    /** @brief Returns the size of the OpenGL texture contained by this Font.
      */
    inline ScreenCoordVector2 const &GetTextureSize () const
    {
        return m_gl_texture->GetSize();
    }
    /** This value is the number of pixels up from the bottom of the font's
      * height box that glyphs' baselines should reside at.
      * @brief Returns the baseline height of the font.
      */
    inline ScreenCoord GetBaselineHeight () const
    {
        return m_baseline_height;
    }
    /** @brief Returns the unicode value of the glyph to display for glyphs
      *        that haven't been rendered into the font texture.
      */
    inline Uint32 GetErrorGlyph () const
    {
        return m_error_glyph;
    }

protected:

    /** Protected so that you must use @ref Xrb::Font::Create .
      * @brief Default constructor.
      */
    inline Font ()
    {
        m_gl_texture = NULL;
        m_error_glyph = '~';
    }

private:

    enum
    {
        RENDERED_GLYPH_LOWEST = ' ',
        RENDERED_GLYPH_HIGHEST = '~',

        RENDERED_GLYPH_COUNT = RENDERED_GLYPH_HIGHEST - RENDERED_GLYPH_LOWEST + 1
    };

    // each glyph's properties are:
    //
    // bitmap width
    // bitmap height
    // bearing x
    // bearing y
    // advance
    // ascender - or some way to know what the actual distance from the top
    //            of the font coordinates to the baseline is.

    struct GlyphSpecification
    {
        Uint32 m_unicode;
        ScreenCoordVector2 m_size;
        ScreenCoordVector2 m_bearing;
        ScreenCoord m_advance;
        ScreenCoordVector2 m_texture_coordinates;

        static int SortByWidthFirst (
            void const *left_operand,
            void const *right_operand);
        static int SortByHeightFirst (
            void const *left_operand,
            void const *right_operand);
    }; // end of class GlyphSpecification

    inline Uint32 GetGlyphIndex (Uint32 const unicode) const
    {
        if (unicode >= RENDERED_GLYPH_LOWEST && unicode <= RENDERED_GLYPH_HIGHEST)
            return unicode - RENDERED_GLYPH_LOWEST;
        else
            return m_error_glyph - RENDERED_GLYPH_LOWEST;
    }

    // helper functions for generating the font texture
    void PopulateGlyphSpecification (Resource<FontFace> const &font_face);
    ScreenCoordVector2 FindSmallestFittingTextureSize (
        GlyphSpecification *const *sorted_glyph_specification);
    Uint32 GetUsedTextureArea (
        ScreenCoordVector2 const &texture_size,
        GlyphSpecification *const *sorted_glyph_specification);
    void GenerateTexture (ScreenCoordVector2 const &texture_size);

    Resource<FontFace> m_font_face;
    bool m_has_kerning;
    ScreenCoord m_pixel_height;
    GLTexture *m_gl_texture;
    GlyphSpecification m_glyph_specification[RENDERED_GLYPH_COUNT];
    // height from bottom of font glyph coordinates
    ScreenCoord m_baseline_height;
    // the unicode value of the "error glyph"
    Uint32 m_error_glyph;
}; // end of class Font

// ///////////////////////////////////////////////////////////////////////////
// reworked font class
// ///////////////////////////////////////////////////////////////////////////

/*

class hierarchy

Font (pure virtual)
    AsciiFont (uses only ascii chars; fast & light)
    UnicodeFont (supports an arbitrary set of glyphs; uses UTF8, slow & heavy)

*/

// this is an abstract interface for Font metrics and rendering.  the goal
// should be to hide all the ugly ascent/advance/kerning/etc away from the
// programmer, so s/he only must make calls like font->DrawString("blah");
class FontBase
{
public:

    virtual ~FontBase () { }

    // you must implement at least one Create() method which will be used by ResourceLibrary

    inline ScreenCoord GetPixelHeight () const { return m_pixel_height; }
    // indicates the orientation of the pen relative to the glyphs being rendered.
    // for example, english text is rendered left-to-right, top-to-bottom,
    // so its initial position would be LEFT for the X dimension, and TOP
    // for the Y dimension.
    inline Alignment2 const &GetInitialPenOrientation () const { return m_initial_pen_orientation; }

    // returns the rectangle containing the given string as rendered in
    // this font.  the rectangle's lower left corner is at (0, 0).
    ScreenCoordRect GetStringRect (
        char const *string,
        ScreenCoordVector2 const &initial_pen_position) const;

    // draw the given string, starting at the given position.  the position's
    // meaning is indicated by the return value of GetInitialPenOrientation().
    // TODO: add left/right/spaced justification
    void DrawString (
        RenderContext const &render_context,
        char const *string,
        ScreenCoordVector2 const &initial_pen_position) const;

protected:

    FontBase (ScreenCoord pixel_height, Alignment2 const &initial_pen_orientation)
        :
        m_pixel_height(pixel_height),
        m_initial_pen_orientation(initial_pen_orientation)
    {
        ASSERT1(initial_pen_orientation[Dim::X] == LEFT || initial_pen_orientation[Dim::X] ==  RIGHT)
        ASSERT1(initial_pen_orientation[Dim::Y] ==  TOP || initial_pen_orientation[Dim::Y] == BOTTOM)
    }

    // ///////////////////////////////////////////////////////////////////////
    // protected interface methods
    // ///////////////////////////////////////////////////////////////////////

    // because a glyph can be made up of a sequence of chars, this method
    // provides a way for the implementation subclass to retrieve the next
    // glyph in a string.  if current_glyph represents the end of the string,
    // then this method should return current_glyph.
    virtual char const *GetNextGlyph (char const *current_glyph) const = 0;

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
        char const *next_glyph) const = 0;

    // this is called once by DrawString before any calls to DrawGlyph are made
    virtual void DrawGlyphSetup (RenderContext const &render_context) const = 0;
    // this is called once by DrawString after any calls to DrawGlyph are made
    virtual void DrawGlyphShutdown (RenderContext const &render_context) const = 0;
    // draw the given glyph, starting at the given position.  the position's
    // meaning is indicated by the return value of GetInitialPenOrientation().
    virtual void DrawGlyph (
        RenderContext const &render_context,
        char const *glyph,
        ScreenCoordVector2 const &pen_position_26_6) const = 0;

private:

    ScreenCoord const m_pixel_height;
    Alignment2 const m_initial_pen_orientation;
}; // end of class FontBase




/** This class uses FontFace to generate pixelized font renderings and
  * stores the resulting data in an OpenGL texture, attempting to use
  * a reasonably small texture to store the set of all rendered glyphs.
  * @brief The font class which is used to render fonts to screen.
  */
class AsciiFont : public FontBase
{
public:

    /** @brief Destructor.  Deletes the gl texture.
      */
    virtual ~AsciiFont ()
    {
        Delete(m_gl_texture);
    }

    /** This is the means to construct a AsciiFont object.  Using this class
      * by pointed-to instances is preferred, so that it can be used in
      * @ref Xrb::Resource .
      * @brief Returns a pointer to a new instance of AsciiFont, loaded from the
      *        given filename, with glyphs rendered to the given maximum
      *        height.
      * @param filename The filename of the font to load.
      * @param pixel_height The maximum height, in pixels, of the glyphs
      *                     to render.
      */
    static FontBase *Create (
        std::string const &filename,
        ScreenCoord pixel_height);
    /** This is the means to construct a AsciiFont object.  Using this class
      * by pointed-to instances is preferred, so that it can be used in
      * @ref Xrb::Resource .
      * @brief Returns a pointer to a new instance of AsciiFont, generated using
      *        the given font face, with glyphs rendered to the given maximum
      *        height.
      * @param font_face The FontFace to use when rendering glyphs.
      * @param pixel_height The maximum height, in pixels, of the glyphs
      *                     to render.
      */
    static FontBase *Create (
        Resource<FontFace> const &font_face,
        ScreenCoord pixel_height);

protected:

    /** Protected so that you must use @ref Xrb::AsciiFont::Create .
      * @brief Default constructor.
      */
    AsciiFont (ScreenCoord pixel_height)
        :
        FontBase(pixel_height, Alignment2(LEFT, TOP))
    {
        m_gl_texture = NULL;
        m_error_glyph = '~';
    }

    // ///////////////////////////////////////////////////////////////////////
    // protected FontBase interface methods
    // ///////////////////////////////////////////////////////////////////////

    virtual char const *GetNextGlyph (char const *current_glyph) const;

    virtual void MoveThroughGlyph (
        ScreenCoordVector2 *pen_position_26_6,
        ScreenCoordVector2 const &initial_pen_position,
        char const *current_glyph,
        char const *next_glyph) const;

    virtual void DrawGlyphSetup (RenderContext const &render_context) const;
    virtual void DrawGlyphShutdown (RenderContext const &render_context) const;
    virtual void DrawGlyph (
        RenderContext const &render_context,
        char const *glyph,
        ScreenCoordVector2 const &pen_position_26_6) const;

private:

    enum
    {
        RENDERED_GLYPH_LOWEST = ' ',
        RENDERED_GLYPH_HIGHEST = '~',

        RENDERED_GLYPH_COUNT = RENDERED_GLYPH_HIGHEST - RENDERED_GLYPH_LOWEST + 1
    };

    // each glyph's properties are:
    //
    // bitmap width
    // bitmap height
    // bearing x
    // bearing y
    // advance
    // ascender - or some way to know what the actual distance from the top
    //            of the font coordinates to the baseline is.

    struct GlyphSpecification
    {
        char m_ascii;
        ScreenCoordVector2 m_size;
        ScreenCoordVector2 m_bearing_26_6;
        ScreenCoord m_advance_26_6;
        ScreenCoordVector2 m_texture_coordinates;

        static int SortByWidthFirst (
            void const *left_operand,
            void const *right_operand);
        static int SortByHeightFirst (
            void const *left_operand,
            void const *right_operand);
    }; // end of class GlyphSpecification

    /** @brief Returns the horizontal kerning offset between the given glyphs.
      * @param left The ascii value of the (visually) left glyph.
      * @param right The ascii value of the (visually) right glyph.
      */
    ScreenCoord GetKerningPixelAdvance_26_6 (char left, char right) const;

    inline Uint32 GetGlyphIndex (char const ascii) const
    {
        if (ascii >= RENDERED_GLYPH_LOWEST && ascii <= RENDERED_GLYPH_HIGHEST)
            return ascii - RENDERED_GLYPH_LOWEST;
        else
            return m_error_glyph - RENDERED_GLYPH_LOWEST;
    }

    // helper functions for generating the font texture
    void PopulateGlyphSpecification (Resource<FontFace> const &font_face);
    ScreenCoordVector2 FindSmallestFittingTextureSize (
        GlyphSpecification *const *sorted_glyph_specification);
    Uint32 GetUsedTextureArea (
        ScreenCoordVector2 const &texture_size,
        GlyphSpecification *const *sorted_glyph_specification);
    void GenerateTexture (ScreenCoordVector2 const &texture_size);

    Resource<FontFace> m_font_face;
    bool m_has_kerning;
    ScreenCoord m_pixel_height;
    GLTexture *m_gl_texture;
    GlyphSpecification m_glyph_specification[RENDERED_GLYPH_COUNT];
    // height from bottom of font glyph coordinates
    ScreenCoord m_baseline_height;
    // the unicode value of the "error glyph"
    Uint32 m_error_glyph;
}; // end of class AsciiFont

} // end of namespace Xrb

#endif // !defined(_XRB_FONT_H_)
