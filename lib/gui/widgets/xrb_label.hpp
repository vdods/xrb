// ///////////////////////////////////////////////////////////////////////////
// xrb_label.hpp by Victor Dods, created 2004/09/12
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_LABEL_HPP_)
#define _XRB_LABEL_HPP_

#include "xrb.hpp"

#include "xrb_ntuple.hpp"
#include "xrb_textwidget.hpp"

namespace Xrb {

typedef NTuple<Alignment, 2> Alignment2;

/** Displays pictures or text (justified or unjustified).  There is no
  * interaction at this level.  This class acts as a baseclass for several
  * widgets which have similar contents and rendering (e.g. Button).
  * @brief Non-interactive display widget, for pictures or text.
  */
class Label : public TextWidget
{
public:

    /// Constructs a Label using the specified text.  By default, no justification will be done,
    /// though the text will be centered horizontally and vertically.
    Label (std::string const &text, WidgetContext &context, std::string const &name = "Label");
    /// Constructs a picture Label using the given texture.  The picture texture is stretched
    /// to fill the area inside the frame margins, adding the content margins.
    Label (Resource<GlTexture> const &picture, WidgetContext &context, std::string const &name = "Label");
    virtual ~Label () { }

    // ///////////////////////////////////////////////////////////////////////
    // public accessors
    // ///////////////////////////////////////////////////////////////////////

    /// @brief Returns the alignment vector.
    /// @details Alignment values for the X and Y components.  The X component alignment can be one of
    /// @c LEFT , @c CENTER , @c RIGHT , or @c SPACED .  The Y component alignment can be one of @c TOP ,
    /// @c CENTER , @c BOTTOM , or @c SPACED .
    Alignment2 const &GetAlignment () const { return m_alignment; }
    /// Returns true iff word-wrapping is enabled.
    bool WordWrap () const { return m_word_wrap; }
    /// Returns true if this is a picture label, false for text label.
    bool IsPictureLabel () const { return m_is_picture_label; }
    /// @brief Returns the resourced GlTexture picture for this label.  If this is a text label, the returned resource will be invalid.
    Resource<GlTexture> const &Picture () const { return m_picture; }
    /// @brief Iff true, indicates that the picture will be resized just small enough to preserve its aspect ratio.
    /// @details Applies only when this is a picture label.  When the picture doesn't fill up the entire
    /// content area, it will be centered appropriately.
    bool PictureKeepsAspectRatio () const { return m_picture_keeps_aspect_ratio; }

    // ///////////////////////////////////////////////////////////////////////
    // public modifiers
    // ///////////////////////////////////////////////////////////////////////

    virtual void SetText (std::string const &text);
    void SetAlignment (Alignment2 const &alignment);
    void SetAlignment (Uint32 component, Alignment alignment);
    virtual void SetWordWrap (bool word_wrap);
    virtual void SetPicture (std::string const &picture_name);
    virtual void SetPicture (Resource<GlTexture> const &picture);
    void SetPictureKeepsAspectRatio (bool picture_keeps_aspect_ratio) { m_picture_keeps_aspect_ratio = picture_keeps_aspect_ratio; }

    virtual void PreDraw ();
    virtual void Draw (RenderContext const &render_context) const;

    
    virtual ScreenCoordVector2 Resize (ScreenCoordVector2 const &size);

protected:

    void DrawText (RenderContext const &render_context) const;
    void DrawPicture (RenderContext const &render_context) const;

    Resource<GlTexture> const &RenderPicture () const { ASSERT1(!RenderPictureNeedsUpdate()); return m_render_picture; }
    Resource<GlTexture> const &RenderPicture ()
    {
        if (RenderPictureNeedsUpdate())
            UpdateRenderPicture();
        ASSERT1(!RenderPictureNeedsUpdate());
        return m_render_picture;
    }

    virtual void SetRenderFont (Resource<Font> const &render_font);
    void SetRenderPicture (Resource<GlTexture> const &render_picture);

    virtual void HandleChangedFrameMargins ();
    virtual void HandleChangedContentMargins ();

    virtual void UpdateRenderFont ();
    // subclasses should override this to specify how to set the render picture.
    virtual void UpdateRenderPicture ();

    bool RenderPictureNeedsUpdate () const { return m_render_picture_needs_update; }
    // indicates that UpdateRenderPicture should be called before Draw.
    void SetRenderPictureNeedsUpdate () { m_render_picture_needs_update = true; }

    virtual ScreenCoordRect TextRect () const;
    virtual void UpdateMinAndMaxSizesFromText ();

    void DirtyTextFormatting () { m_text_formatting_update_required = true; }

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
    Resource<GlTexture> m_picture;
    // the texture which will be used to render the picture
    Resource<GlTexture> m_render_picture;
    // indicates that UpdateRenderPicture should be called before Draw.
    bool m_render_picture_needs_update;
    // indicates if the picture will maintain its aspect ratio, growing to
    // the largest size that will still fit inside this label.
    bool m_picture_keeps_aspect_ratio;
}; // end of class Label

} // end of namespace Xrb

#endif // !defined(_XRB_LABEL_HPP_)

