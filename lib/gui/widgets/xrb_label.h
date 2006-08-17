// ///////////////////////////////////////////////////////////////////////////
// xrb_label.h by Victor Dods, created 2004/09/12
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_LABEL_H_)
#define _XRB_LABEL_H_

#include "xrb.h"

#include "xrb_ntuple.h"
#include "xrb_textwidget.h"

namespace Xrb
{

typedef NTuple<Alignment, 2> Alignment2;

/** Displays pictures or text (justified or unjustified).  There is no
  * interaction at this level.  This class acts as a baseclass for several
  * widgets which have similar contents and rendering (e.g. Button).
  * @brief Non-interactive display widget, for pictures or text.
  */
class Label : public TextWidget
{
public:

    /** Constructs a Label using the specified text.  By default, no
      * justification will be done, though the text will be centered
      * horizontally and vertically.
      * @brief Constructs a text label.
      */
    Label (
        std::string const &text,
        ContainerWidget *const parent,
        std::string const &name = "Label");
    /** The picture texture is stretched to fill the area inside the
      * frame margins, adding the content margins.
      * @brief Constructs a picture label.
      */
    Label (
        Resource<GLTexture> const &picture,
        ContainerWidget *const parent,
        std::string const &name = "Label");
    /** @brief Destructor.
      */
    virtual ~Label () { }

    // ///////////////////////////////////////////////////////////////////////
    // public accessors
    // ///////////////////////////////////////////////////////////////////////

    /** Alignment values for the X and Y components.  The X component
      * alignment can be one of @c LEFT , @c CENTER , @c RIGHT , or
      * @c SPACED .  The Y component alignment can be one of @c TOP ,
      * @c CENTER , @c BOTTOM , or @c SPACED .
      * @brief Returns the alignment vector.
      */
    inline Alignment2 const &GetAlignment () const { return m_alignment; }
    /** @brief Returns true iff word-wrapping is enabled.
      */
    inline bool GetWordWrap () const { return m_word_wrap; }
    /** @brief Returns true if this is a picture label, false for text label.
      */
    inline bool GetIsPictureLabel () const { return m_is_picture_label; }
    /** If this is a text label, the returned resource will be invalid.
      * @brief Returns the resourced GLTexture picture for this label.
      */
    inline Resource<GLTexture> const &GetPicture () const { return m_picture; }
    /** Applies only when this is a picture label.  When the picture doesn't
      * fill up the entire content area, it will be centered appropriately.
      * @brief Iff true, indicates that the picture will be resized just small
      *        enough to preserve its aspect ratio.
      */
    inline bool GetPictureKeepsAspectRatio () const { return m_picture_keeps_aspect_ratio; }

    // ///////////////////////////////////////////////////////////////////////
    // public modifiers
    // ///////////////////////////////////////////////////////////////////////

    virtual void SetText (std::string const &text);
    void SetAlignment (Alignment2 const &alignment);
    void SetAlignment (Uint32 component, Alignment alignment);
    virtual void SetWordWrap (bool word_wrap);
    virtual void SetPicture (std::string const &picture_name);
    virtual void SetPicture (Resource<GLTexture> const &picture);
    inline void SetPictureKeepsAspectRatio (bool picture_keeps_aspect_ratio) { m_picture_keeps_aspect_ratio = picture_keeps_aspect_ratio; }

    virtual void Draw (RenderContext const &render_context) const;
    virtual ScreenCoordVector2 Resize (ScreenCoordVector2 const &size);

protected:

    void DrawText (RenderContext const &render_context) const;
    void DrawPicture (RenderContext const &render_context) const;

    inline Resource<GLTexture> const &GetRenderPicture () const { return m_render_picture; }

    virtual void SetRenderFont (Resource<Font> const &render_font);
    void SetRenderPicture (Resource<GLTexture> const &render_picture);

    virtual void HandleChangedFrameMargins ();
    virtual void HandleChangedContentMargins ();

    virtual void UpdateRenderFont ();
    virtual void UpdateRenderPicture ();

    virtual ScreenCoordRect GetTextRect () const;
    virtual void UpdateMinAndMaxSizesFromText ();

    inline void DirtyTextFormatting () { m_text_formatting_update_required = true; }

private:

    void UpdateCachedFormattedText () const;

    // the cached string which is used in word wrapping to speed up processing
    mutable std::string m_cached_formatted_text;
    // the string from which m_cached_formatted_text was generated
    mutable std::string const *m_line_format_vector_source;
    // line-indexed addressing for faster vertical access
    mutable Font::LineFormatVector m_line_format_vector;
    // indicates if the render properties have changed and require the
    // m_cached_formatted_text/m_line_format_vector to be updated
    mutable bool m_text_formatting_update_required;
    // the horizontal and vertical justification of the text
    Alignment2 m_alignment;
    // indicates if word wrapping is turned on
    bool m_word_wrap;
    // if true, this is exclusively a picture label.  false indicates a text label
    bool m_is_picture_label;
    // the picture texture (or 0 if no picture)
    Resource<GLTexture> m_picture;
    // the texture which will be used to render the picture
    Resource<GLTexture> m_render_picture;
    // indicates if the picture will maintain its aspect ratio, growing to
    // the largest size that will still fit inside this label.
    bool m_picture_keeps_aspect_ratio;
}; // end of class Label

} // end of namespace Xrb

#endif // !defined(_XRB_LABEL_H_)

