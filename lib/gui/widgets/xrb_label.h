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

#include <string>

#include "xrb_color.h"
#include "xrb_widget.h"
#include "xrb_ntuple.h"
#include "xrb_font.h"
#include "xrb_resourcelibrary.h"

namespace Xrb
{

/** Displays pictures or text (justified or unjustified).  There is no
  * interaction at this level.  This class acts as a baseclass for several
  * widgets which have similar contents and rendering (e.g. Button).
  * @brief Non-interactive display widget, for pictures or text.
  */
class Label : public Widget
{
public:

    typedef NTuple<Alignment, 2> Alignment2;

    /** Constructs a Label using the specified text.  By default, no
      * justification will be done, though the text will be centered
      * horizontally and vertically.
      * @brief Constructs a text label.
      */
    Label (
        std::string const &text,
        Widget *const parent,
        std::string const &name = "Label");
    /** The picture texture is stretched to fill the area inside the
      * frame margins, adding the content margins.
      * @brief Constructs a picture label.
      */
    Label (
        Resource<GLTexture> const &picture,
        Widget *const parent,
        std::string const &name = "Label");
    /** Does nothing.
      * @brief Destructor.
      */
    virtual ~Label ();

    // ///////////////////////////////////////////////////////////////////////
    // public accessors
    // ///////////////////////////////////////////////////////////////////////

    /** If true, when the text's width changes, the minimum width of this
      * widget will change, and the widget may be resized.
      * @brief Returns true iff the label's minimum width is fixed to the
      *        width of its text.
      */
    inline bool GetIsMinWidthFixedToTextWidth () const
    {
        return m_is_min_width_fixed_to_text_width;
    }
    /** If true, when the text's width changes, the maximum width of this
      * widget will change, and the widget may be resized.
      * @brief Returns true iff the label's maximum width is fixed to the
      *        width of its text.
      */
    inline bool GetIsMaxWidthFixedToTextWidth () const
    {
        return m_is_max_width_fixed_to_text_width;
    }
    /** If true, when the text's height changes, the minimum height of this
      * widget will change, and the widget may be resized.
      * @brief Returns true iff the label's minimum height is fixed to the
      *        height of its text.
      */
    inline bool GetIsMinHeightFixedToTextHeight () const
    {
        return m_is_min_height_fixed_to_text_height;
    }
    /** If true, when the text's height changes, the maximum height of this
      * widget will change, and the widget may be resized.
      * @brief Returns true iff the label's maximum height is fixed to the
      *        height of its text.
      */
    inline bool GetIsMaxHeightFixedToTextHeight () const
    {
        return m_is_max_height_fixed_to_text_height;
    }
    /** @brief Returns the string containing the text of this label.
      */
    inline std::string const &GetText () const
    {
        return m_text;
    }
    /** This color isn't necessarily what will be used to render the text.
      * The render text color will be used to render the text, and this color
      * may be used to set the render text color.
      * @brief Returns the normal color of the text.
      */
    inline Color const &GetTextColor () const
    {
        return m_text_color;
    }
    /** Alignment values for the X and Y components.  The X component
      * alignment can be one of @c LEFT , @c CENTER , @c RIGHT , or
      * @c SPACED .  The Y component alignment can be one of @c TOP ,
      * @c CENTER , @c BOTTOM , or @c SPACED .
      * @brief Returns the alignment vector.
      */
    inline Alignment2 const &GetAlignment () const
    {
        return m_alignment;
    }
    /** @brief Returns true iff word-wrapping is enabled.
      */
    inline bool GetWordWrap () const
    {
        return m_word_wrap;
    }
    /** This value applies only when word-wrapping is enabled.
      * @brief Returns true iff paragraph indentation is enabled.
      */
    inline bool GetIndent () const
    {
        return m_indent;
    }
    /** @brief Returns the number of spaces that will be printed in place
      *        of a tab character.
      */
    inline Uint32 GetTabSize () const
    {
        return m_tab_size;
    }
    /** @brief Returns the resourced normal Font for this label.
      */
    inline Resource<Font> const &GetFont () const
    {
        return m_font;
    }
    /** If this is a text label, the returned resource will be invalid.
      * @brief Returns the resourced GLTexture picture for this label.
      */
    inline Resource<GLTexture> const &GetPicture () const
    {
        return m_picture;
    }
    /** Applies only when this is a picture label.  When the picture doesn't
      * fill up the entire content area, it will be centered appropriately.
      * @brief Iff true, indicates that the picture will be resized just small
      *        enough to preserve its aspect ratio.
      */
    inline bool GetPictureKeepsAspectRatio () const
    {
        return m_picture_keeps_aspect_ratio;
    }
    /** The content margins are added to the frame margins to indicate the
      * content area of the label.  The content margins can be negative (up to
      * the point that they totally cancel the frame margins).
      * @brief Returns the content margins for this label.
      */
    inline ScreenCoordVector2 const &GetContentMargins () const
    {
        return m_content_margins;
    }
    /** @brief Returns the rectangle representing the content area.
      */
    inline ScreenCoordRect GetContentsRect () const
    {
        return GetScreenRect().GetGrown(
            -(GetFrameMargins() + GetContentMargins()));
    }

    // ///////////////////////////////////////////////////////////////////////
    // SignalReceiver accessors
    // ///////////////////////////////////////////////////////////////////////
    inline SignalReceiver1<std::string const &> const *ReceiverSetText ()
    {
        return &m_receiver_set_text;
    }

    // ///////////////////////////////////////////////////////////////////////
    // public modifiers
    // ///////////////////////////////////////////////////////////////////////

    void SetIsMinWidthFixedToTextWidth (bool is_min_width_fixed_to_text_width);
    void SetIsMaxWidthFixedToTextWidth (bool is_max_width_fixed_to_text_width);
    void SetIsWidthFixedToTextWidth (bool is_width_fixed_to_text_width);
    void SetIsMinHeightFixedToTextHeight (bool is_min_height_fixed_to_text_height);
    void SetIsMaxHeightFixedToTextHeight (bool is_max_height_fixed_to_text_height);
    void SetIsHeightFixedToTextHeight (bool is_height_fixed_to_text_height);
    void SetIsSizeFixedToTextSize (bool is_size_fixed_to_text_size);
    virtual void SetText (std::string const &text);
    virtual void SetTextColor (Color const &color);
    void SetAlignment (Alignment2 const &alignment);
    void SetAlignment (Uint32 component, Alignment alignment);
    virtual void SetWordWrap (bool word_wrap);
    void SetIndent (bool indent);
    void SetTabSize (Uint32 tab_size);
    void SetFont (Resource<Font> const &font);
    void SetFontFaceFilename (std::string const &font_face_filename);
    void SetFontHeightRatio (Float font_height_ratio);
    void SetFontHeight (ScreenCoord font_height);
    virtual void SetPicture (std::string const &picture_name);
    virtual void SetPicture (Resource<GLTexture> const &picture);
    inline void SetPictureKeepsAspectRatio (bool picture_keeps_aspect_ratio)
    {
        m_picture_keeps_aspect_ratio = picture_keeps_aspect_ratio;
    }
    void SetContentMargins (ScreenCoordVector2 const &content_margins);
    void SetContentMarginRatios (FloatVector2 const &content_margin_ratios);

    virtual void Draw (RenderContext const &render_context) const;
    virtual ScreenCoordVector2 Resize (ScreenCoordVector2 const &size);

protected:

    // NOT part of WidgetSkinHandler
    virtual void HandleChangedLabelFont ();
    virtual void HandleChangedFrameMargins ();
    virtual void HandleChangedContentMargins ();

    void DrawText (RenderContext const &render_context) const;
    void DrawPicture (RenderContext const &render_context) const;

    inline Color const &GetRenderTextColor () const
    {
        return m_render_text_color;
    }
    inline Resource<Font> const &GetRenderFont () const
    {
        return m_render_font;
    }
    inline Resource<GLTexture> const &GetRenderPicture () const
    {
        return m_render_picture;
    }

    inline void SetRenderTextColor (Color const &render_text_color)
    {
        m_render_text_color = render_text_color;
    }
    void SetRenderFont (Resource<Font> const &render_font);
    void SetRenderPicture (Resource<GLTexture> const &render_picture)
    {
        m_render_picture = render_picture;
    }

    virtual void UpdateRenderTextColor ();
    virtual void UpdateRenderFont ();
    virtual void UpdateRenderPicture ();

    inline void DirtyTextFormatting ()
    {
        m_text_formatting_update_required = true;
    }
    void UpdateMinAndMaxSizes ();
//     void UpdateMinWidth ();
//     void UpdateMaxWidth ();
//     void UpdateMinHeight ();
//     void UpdateMaxHeight ();

    // the text of the label
    std::string m_text;
    // move the drawn text by this offset (used by LineEdit)
    ScreenCoordVector2 m_text_offset;

    // NOTE: this min/max width/height stuff does not apply when
    // word wrapping is turned on.

    // indicates if the text width dictates the minimum width
    bool m_is_min_width_fixed_to_text_width;
    // indicates if the text width dictates the maximum width
    bool m_is_max_width_fixed_to_text_width;
    // indicates if the text height dictates the minimum height
    bool m_is_min_height_fixed_to_text_height;
    // indicates if the text height dictates the maximum height
    bool m_is_max_height_fixed_to_text_height;

private:

    void UpdateCachedFormattedText () const;

    void Initialize ();

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
    // indicates if indenting should happen after newlines (but this
    // is only applicable if word wrapping is turned on)
    bool m_indent;
    // the number of spaces in a tab
    Uint32 m_tab_size;
    // the basic font for this label
    Resource<Font> m_font;
    // the font which will be used for text drawing calculations
    Resource<Font> m_render_font;
    // color of the text
    Color m_text_color;
    // color to render the text in
    Color m_render_text_color;
    // the picture texture (or 0 if no picture)
    Resource<GLTexture> m_picture;
    // the texture which will be used to render the picture
    Resource<GLTexture> m_render_picture;
    // indicates if the picture will maintain its aspect ratio, growing to
    // the largest size that will still fit inside this label.
    bool m_picture_keeps_aspect_ratio;
    // the content margins (which are the margins inside of the frame margins
    // to denote the bounding box for the contents of this Label)
    ScreenCoordVector2 m_content_margins;

    //////////////////////////////////////////////////////////////////////////
    // SignalReceivers
    SignalReceiver1<std::string const &> m_receiver_set_text;
    // end SignalReceivers
    //////////////////////////////////////////////////////////////////////////
}; // end of class Label

} // end of namespace Xrb

#endif // !defined(_XRB_LABEL_H_)

