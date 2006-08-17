// ///////////////////////////////////////////////////////////////////////////
// xrb_textwidget.h by Victor Dods, created 2006/06/10
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_TEXTWIDGET_H_)
#define _XRB_TEXTWIDGET_H_

#include "xrb.h"

#include "xrb_color.h"
#include "xrb_widget.h"
#include "xrb_font.h"
#include "xrb_resourcelibrary.h"

namespace Xrb
{

class TextWidget : public Widget
{
public:

    /** @brief Constructs a text label using the specified text.
      */
    TextWidget (
        std::string const &text,
        ContainerWidget *const parent,
        std::string const &name = "TextWidget");
    virtual ~TextWidget () { }

    // ///////////////////////////////////////////////////////////////////////
    // public accessors
    // ///////////////////////////////////////////////////////////////////////

    /** @brief Returns the string containing the text of this label.
      */
    inline std::string const &GetText () const { return m_text; }
    /** This color isn't necessarily what will be used to render the text.
      * The render text color will be used to render the text, and this color
      * may be used to set the render text color.
      * @brief Returns the normal color of the text.
      */
    inline Color const &GetTextColor () const { return m_text_color; }
    /** @brief Returns the resourced normal Font for this label.
      */
    inline Resource<Font> const &GetFont () const { return m_font; }
    /** If true, when the text's width changes, the minimum width of this
      * widget will change, and the widget may be resized.
      * @brief Returns true iff the label's minimum width is fixed to the
      *        width of its text.
      */
    inline bool GetIsMinWidthFixedToTextWidth () const { return m_is_min_width_fixed_to_text_width; }
    /** If true, when the text's width changes, the maximum width of this
      * widget will change, and the widget may be resized.
      * @brief Returns true iff the label's maximum width is fixed to the
      *        width of its text.
      */
    inline bool GetIsMaxWidthFixedToTextWidth () const { return m_is_max_width_fixed_to_text_width; }
    /** If true, when the text's height changes, the minimum height of this
      * widget will change, and the widget may be resized.
      * @brief Returns true iff the label's minimum height is fixed to the
      *        height of its text.
      */
    inline bool GetIsMinHeightFixedToTextHeight () const { return m_is_min_height_fixed_to_text_height; }
    /** If true, when the text's height changes, the maximum height of this
      * widget will change, and the widget may be resized.
      * @brief Returns true iff the label's maximum height is fixed to the
      *        height of its text.
      */
    inline bool GetIsMaxHeightFixedToTextHeight () const { return m_is_max_height_fixed_to_text_height; }

    // ///////////////////////////////////////////////////////////////////////
    // SignalReceiver accessors
    // ///////////////////////////////////////////////////////////////////////

    inline SignalReceiver1<std::string const &> const *ReceiverSetText () { return &m_receiver_set_text; }
    inline SignalReceiver1<std::string> const *ReceiverSetTextV () { return &m_receiver_set_text_v; }

    // ///////////////////////////////////////////////////////////////////////
    // public modifiers
    // ///////////////////////////////////////////////////////////////////////

    virtual void SetText (std::string const &text);
    void SetTextColor (Color const &color);
    void SetFont (Resource<Font> const &font);
    void SetFontFaceFilename (std::string const &font_face_filename);
    void SetFontHeightRatio (Float font_height_ratio);
    void SetFontHeight (ScreenCoord font_height);
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

    inline Color const &GetRenderTextColor () const { return m_render_text_color; }
    inline Resource<Font> const &GetRenderFont () const { return m_render_font; }

    inline void SetRenderTextColor (Color const &render_text_color) { m_render_text_color = render_text_color; }
    virtual void SetRenderFont (Resource<Font> const &render_font);

    // NOT part of WidgetSkinHandler
    virtual void HandleChangedFont ();
    virtual void HandleChangedFrameMargins ();
    virtual void HandleChangedContentMargins ();

    virtual void UpdateRenderTextColor ();
    virtual void UpdateRenderFont ();

    // if you need to specify a custom text bounding box accessor,
    // do it with an override of this.
    virtual ScreenCoordRect GetTextRect () const;
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
    Resource<Font> m_font;
    // the font which will be used for text drawing calculations
    Resource<Font> m_render_font;
    // color of the text
    Color m_text_color;
    // color to render the text in
    Color m_render_text_color;

    // ///////////////////////////////////////////////////////////////////////
    // SignalReceivers
    // ///////////////////////////////////////////////////////////////////////

    SignalReceiver1<std::string const &> m_receiver_set_text;
    SignalReceiver1<std::string> m_receiver_set_text_v;

}; // end of class TextWidget

} // end of namespace Xrb

#endif // !defined(_XRB_TEXTWIDGET_H_)

