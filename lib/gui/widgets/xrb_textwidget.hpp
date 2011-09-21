// ///////////////////////////////////////////////////////////////////////////
// xrb_textwidget.hpp by Victor Dods, created 2006/06/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_TEXTWIDGET_HPP_)
#define _XRB_TEXTWIDGET_HPP_

#include "xrb.hpp"

#include "xrb_color.hpp"
#include "xrb_widget.hpp"
#include "xrb_font.hpp"
#include "xrb_resourcelibrary.hpp"

namespace Xrb {

class TextWidget : public Widget
{
public:

    /// Constructs a text label using the specified text.
    TextWidget (std::string const &text, WidgetContext &context, std::string const &name = "TextWidget");
    virtual ~TextWidget () { }

    // ///////////////////////////////////////////////////////////////////////
    // public accessors
    // ///////////////////////////////////////////////////////////////////////

    /// Returns the string containing the text of this label.
    std::string const &Text () const { return m_text; }
    /// This color isn't necessarily what will be used to render the text.
    /// The render text color will be used to render the text, and this color
    /// may be used to set the render text color.
    /// @brief Returns the normal color of the text.
    Color const &TextColor () const { return m_text_color; }

    /// Returns true iff there is no font style, meaning that the basic font is specified explicitly.
    bool FontStyleIsOverridden () const { return m_font_style.empty(); }
    /// If nonempty, gives the StyleSheet type of font to use as the basic font.  @see StyleSheet::Font*.
    /// If empty, the font is accessible via GetFont.
    std::string const &FontStyle () const { return m_font_style; }
    /// If the font style is overridden, returns the overridden font path.  Otherwise returns the stylized font path.
    std::string const &FontPath () const;
    /// If the font style is overridden, returns the overridden font height ratio.  Otherwise returns the stylized font height ratio.
    Float FontHeightRatio () const;
    /// If the font style is overridden, returns the overridden font pixel height.  Otherwise returns the stylized font pixel height.
    ScreenCoord FontPixelHeight () const;
    /// If the font style is overridden, returns the overridden font.  Otherwise returns the stylized font.
    Resource<Font> const &GetFont () const;
    
    /// @brief Returns true iff the label's minimum width is fixed to the width of its text.
    /// @details If true, when the text's width changes, the minimum width of this widget will change, and the widget may be resized.
    bool IsMinWidthFixedToTextWidth () const { return m_is_min_width_fixed_to_text_width; }
    /// @brief Returns true iff the label's maximum width is fixed to the width of its text.
    /// @details If true, when the text's width changes, the maximum width of this widget will change, and the widget may be resized.
    bool IsMaxWidthFixedToTextWidth () const { return m_is_max_width_fixed_to_text_width; }
    /// @brief Returns true iff the label's minimum height is fixed to the height of its text.
    /// @details If true, when the text's height changes, the minimum height of this widget will change, and the widget may be resized.
    bool IsMinHeightFixedToTextHeight () const { return m_is_min_height_fixed_to_text_height; }
    /// @brief Returns true iff the label's maximum height is fixed to the height of its text.
    /// @details If true, when the text's height changes, the maximum height of this widget will change, and the widget may be resized.
    bool IsMaxHeightFixedToTextHeight () const { return m_is_max_height_fixed_to_text_height; }

    // ///////////////////////////////////////////////////////////////////////
    // SignalReceiver accessors
    // ///////////////////////////////////////////////////////////////////////

    SignalReceiver1<std::string const &> const *ReceiverSetText () { return &m_receiver_set_text; }
    SignalReceiver1<std::string> const *ReceiverSetTextV () { return &m_receiver_set_text_v; }

    // ///////////////////////////////////////////////////////////////////////
    // public modifiers
    // ///////////////////////////////////////////////////////////////////////

    virtual void SetText (std::string const &text);
    void SetTextColor (Color const &color);

    /// @brief Sets the basic font style (see StyleSheet), overriding any manually specified basic font.
    /// @details The style string can't be empty -- that indicates that the font is explicitly
    /// specified -- Use SetFont for this purpose instead.
    void SetFontStyle (std::string const &style);
    /// Overrides the font style and sets the path for the basic font for this widget.
    void SetFontPath (std::string const &font_path);
    /// Overrides the font style and sets the height ratio for the basic font for this widget.
    void SetFontHeightRatio (Float font_height_ratio);
    /// Overrides the font style and sets the pixel height for the basic font for this widget.
    void SetFontPixelHeight (ScreenCoord font_pixel_height);
    /// Overrides the font style and sets the basic font for this widget.
    void SetFont (std::string const &font_path, Float font_height_ratio);
    /// Overrides the font style and sets the basic font for this widget.
    void SetFont (Resource<Font> const &font);
    
    void SetIsMinWidthFixedToTextWidth (bool is_min_width_fixed_to_text_width);
    void SetIsMaxWidthFixedToTextWidth (bool is_max_width_fixed_to_text_width);
    void SetIsWidthFixedToTextWidth (bool is_width_fixed_to_text_width);
    void SetIsMinHeightFixedToTextHeight (bool is_min_height_fixed_to_text_height);
    void SetIsMaxHeightFixedToTextHeight (bool is_max_height_fixed_to_text_height);
    void SetIsHeightFixedToTextHeight (bool is_height_fixed_to_text_height);
    void SetIsMinSizeFixedToTextSize (bool is_min_size_fixed_to_text_size);
    void SetIsMaxSizeFixedToTextSize (bool is_max_size_fixed_to_text_size);
    void SetIsSizeFixedToTextSize (bool is_size_fixed_to_text_size);

    virtual void PreDraw ();
    virtual void Draw (RenderContext const &render_context) const = 0;

protected:

    /// Should only be used by Label.
    void ReleaseFont () { m_font.Release(); }

    Color const &RenderTextColor () const { ASSERT1(!RenderTextColorNeedsUpdate()); return m_render_text_color; }
    Color const &RenderTextColor ()
    {
        if (RenderTextColorNeedsUpdate())
            UpdateRenderTextColor();
        ASSERT1(!RenderTextColorNeedsUpdate());
        return m_render_text_color;
        
    }
    Resource<Font> const &RenderFont () const { ASSERT1(!RenderFontNeedsUpdate()); return m_render_font; }
    Resource<Font> const &RenderFont ()
    {
        if (RenderFontNeedsUpdate())
            UpdateRenderFont();
        ASSERT1(!RenderFontNeedsUpdate());
        return m_render_font;
    }

    void SetRenderTextColor (Color const &render_text_color) { m_render_text_color = render_text_color; }
    virtual void SetRenderFont (Resource<Font> const &render_font);

    virtual void HandleChangedStyleSheet ();
    // subclasses should override this to specify how to set the render text color.
    virtual void UpdateRenderTextColor ();
    // Returns true iff the "render text color needs update" flag is set.  @see SetRenderTextColorNeedsUpdate.
    bool RenderTextColorNeedsUpdate () const { return m_render_text_color_needs_update; }
    // indicates that UpdateRenderTextColor should be called before Draw.
    void SetRenderTextColorNeedsUpdate (bool f = true) { m_render_text_color_needs_update = f; }
    // subclasses should override this to specify how to set the render font.
    virtual void UpdateRenderFont ();
    // Returns true iff the "render font needs update" flag is set.  @see SetRenderFontNeedsUpdate.
    bool RenderFontNeedsUpdate () const { return m_render_font_needs_update; }
    // indicates that UpdateRenderFont should be called before Draw.
    void SetRenderFontNeedsUpdate (bool f = true) { m_render_font_needs_update = f; }

    virtual void HandleChangedFrameMargins ();
    virtual void HandleChangedContentMargins ();

    // if you need to specify a custom text bounding box accessor,
    // do it with an override of this.
    virtual ScreenCoordRect TextRect () const;
    // if you want to clamp m_is_min_width_fixed_to_text_width and all
    // that to certain values, do it in an override of this.
    virtual void UpdateMinAndMaxSizesFromText ();

    // the text of the label
    std::string m_text;
    // indicates if the text width dictates the minimum width
    bool m_is_min_width_fixed_to_text_width;
    // indicates if the text width dictates the maximum width
    bool m_is_max_width_fixed_to_text_width;
    // indicates if the text height dictates the minimum height
    bool m_is_min_height_fixed_to_text_height;
    // indicates if the text height dictates the maximum height
    bool m_is_max_height_fixed_to_text_height;

private:

    void SetTextV (std::string text) { SetText(text); }
    void EnsureFontIsCached () const;

    /// If nonempty, gives the StyleSheet type of font to use for the basic font.
    /// @see StyleSheet::Font.  If empty, the font is specified manually in m_font.
    std::string m_font_style;
    /// When m_font_style is empty, this specifies the font path to use.
    std::string m_font_path;
    /// When m_font_style is empty, this specifies the font height ratio to use.
    Float m_font_height_ratio;
    /// Cached font for use in GetFont().
    mutable Resource<Font> m_font;
    
    // color of the text
    Color m_text_color;
    
    // the font which will be used for text drawing calculations
    Resource<Font> m_render_font;
    // indicates that UpdateRenderFont should be called before Draw.
    bool m_render_font_needs_update;
    // color to render the text in
    Color m_render_text_color;
    // indicates that UpdateRenderTextColor should be called before Draw.
    bool m_render_text_color_needs_update;

    // ///////////////////////////////////////////////////////////////////////
    // SignalReceivers
    // ///////////////////////////////////////////////////////////////////////

    SignalReceiver1<std::string const &> m_receiver_set_text;
    SignalReceiver1<std::string> m_receiver_set_text_v;

}; // end of class TextWidget

} // end of namespace Xrb

#endif // !defined(_XRB_TEXTWIDGET_HPP_)

