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

    /** @brief Constructs a text label using the specified text.
      */
    TextWidget (std::string const &text, std::string const &name = "TextWidget");
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
    /// Returns the Font that this TextWidget is currently using.
    /// @note The current font can only be set via WidgetSkin.
    Resource<Font> const &GetFont () const;
    
    /** If true, when the text's width changes, the minimum width of this
      * widget will change, and the widget may be resized.
      * @brief Returns true iff the label's minimum width is fixed to the
      *        width of its text.
      */
    bool IsMinWidthFixedToTextWidth () const { return m_is_min_width_fixed_to_text_width; }
    /** If true, when the text's width changes, the maximum width of this
      * widget will change, and the widget may be resized.
      * @brief Returns true iff the label's maximum width is fixed to the
      *        width of its text.
      */
    bool IsMaxWidthFixedToTextWidth () const { return m_is_max_width_fixed_to_text_width; }
    /** If true, when the text's height changes, the minimum height of this
      * widget will change, and the widget may be resized.
      * @brief Returns true iff the label's minimum height is fixed to the
      *        height of its text.
      */
    bool IsMinHeightFixedToTextHeight () const { return m_is_min_height_fixed_to_text_height; }
    /** If true, when the text's height changes, the maximum height of this
      * widget will change, and the widget may be resized.
      * @brief Returns true iff the label's maximum height is fixed to the
      *        height of its text.
      */
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

    void SetIsMinWidthFixedToTextWidth (bool is_min_width_fixed_to_text_width);
    void SetIsMaxWidthFixedToTextWidth (bool is_max_width_fixed_to_text_width);
    void SetIsWidthFixedToTextWidth (bool is_width_fixed_to_text_width);
    void SetIsMinHeightFixedToTextHeight (bool is_min_height_fixed_to_text_height);
    void SetIsMaxHeightFixedToTextHeight (bool is_max_height_fixed_to_text_height);
    void SetIsHeightFixedToTextHeight (bool is_height_fixed_to_text_height);
    void SetIsMinSizeFixedToTextSize (bool is_min_size_fixed_to_text_size);
    void SetIsMaxSizeFixedToTextSize (bool is_max_size_fixed_to_text_size);
    void SetIsSizeFixedToTextSize (bool is_size_fixed_to_text_size);

    virtual void Draw (RenderContext const &render_context) const = 0;

protected:

    /// Should only be used by Label.
    void ReleaseFont () { m_font.Release(); }

    Color const &RenderTextColor () const { return m_render_text_color; }
    Resource<Font> const &RenderFont () const { return m_render_font; }

    void SetRenderTextColor (Color const &render_text_color) { m_render_text_color = render_text_color; }
    virtual void SetRenderFont (Resource<Font> const &render_font);

    // NOT part of WidgetSkinHandler
    virtual void HandleChangedFrameMargins ();
    virtual void HandleChangedContentMargins ();

    virtual void UpdateRenderTextColor ();
    virtual void UpdateRenderFont ();

    // YES part of WidgetSkinHandler
    virtual void HandleChangedWidgetSkin ();

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

    // the basic font for this label
    mutable Resource<Font> m_font;
    // color of the text
    Color m_text_color;
    
    // the font which will be used for text drawing calculations
    Resource<Font> m_render_font;
    // color to render the text in
    Color m_render_text_color;

    // ///////////////////////////////////////////////////////////////////////
    // SignalReceivers
    // ///////////////////////////////////////////////////////////////////////

    SignalReceiver1<std::string const &> m_receiver_set_text;
    SignalReceiver1<std::string> m_receiver_set_text_v;

}; // end of class TextWidget

} // end of namespace Xrb

#endif // !defined(_XRB_TEXTWIDGET_HPP_)

