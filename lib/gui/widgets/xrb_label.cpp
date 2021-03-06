// ///////////////////////////////////////////////////////////////////////////
// xrb_label.cpp by Victor Dods, created 2004/09/12
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_label.hpp"

#include <stdarg.h>

#include "xrb_gl.hpp"
#include "xrb_render.hpp"
#include "xrb_screen.hpp"
#include "xrb_widgetcontext.hpp"

namespace Xrb {

Label::Label (std::string const &text, WidgetContext &context, std::string const &name)
    :
    TextWidget(text, context, name)
{
    DirtyTextFormatting();
    m_line_format_vector_source = NULL;
    m_alignment = Alignment2(CENTER, CENTER);
    m_word_wrap = false;
    m_is_picture_label = false;
    m_render_picture_needs_update = false; // this is not a picture label
    SetIsMinSizeFixedToTextSize(true);
    ASSERT1(!m_picture.IsValid());
    ASSERT1(!m_render_picture.IsValid());
}

Label::Label (Resource<GlTexture> const &picture, WidgetContext &context, std::string const &name)
    :
    TextWidget("", context, name),
    m_picture(picture)
{
    // this must be done before clearing the font
    m_is_picture_label = true;
    // override these values (default is true in TextWidget constructor).
    SetRenderTextColorNeedsUpdate(false);
    SetRenderFontNeedsUpdate(false);
    // clear the font (because the presence or absence of a font
    // is what dictates if this is a text or picture label).
    ReleaseFont();
    m_render_picture_needs_update = true;
    m_picture_keeps_aspect_ratio = false;
}

void Label::SetText (std::string const &text)
{
    if (m_is_picture_label)
        return;

    DirtyTextFormatting();
    TextWidget::SetText(text);
}

void Label::SetAlignment (Alignment2 const &alignment)
{
    if (m_is_picture_label)
        return;

    m_alignment = alignment;
}

void Label::SetAlignment (Uint32 component, Alignment alignment)
{
    if (m_is_picture_label)
        return;

    ASSERT1(component <= 1);
    m_alignment[component] = alignment;
}

void Label::SetWordWrap (bool word_wrap)
{
    if (m_is_picture_label)
        return;

    ASSERT1(RenderFont().IsValid());

    if (m_word_wrap != word_wrap)
    {
        m_word_wrap = word_wrap;
        DirtyTextFormatting();
        UpdateMinAndMaxSizesFromText();
    }
}

void Label::SetPicture (std::string const &picture_name)
{
    if (!m_is_picture_label)
        return;

    ASSERT1(!picture_name.empty());

    Resource<GlTexture> picture = GlTexture::Load(picture_name);
    ASSERT1(picture.IsValid());
    if (m_picture != picture)
    {
        m_picture = picture;
        SetRenderPictureNeedsUpdate();
    }
}

void Label::SetPicture (Resource<GlTexture> const &picture)
{
    if (!m_is_picture_label)
        return;

    if (m_picture != picture)
    {
        m_picture = picture;
        SetRenderPictureNeedsUpdate();
    }
}

void Label::PreDraw ()
{
    TextWidget::PreDraw();
    if (RenderPictureNeedsUpdate())
        UpdateRenderPicture();
    ASSERT1(!RenderPictureNeedsUpdate());
}

void Label::Draw (RenderContext const &render_context) const
{
    ASSERT1(!RenderPictureNeedsUpdate());
    ASSERT1(!RenderTextColorNeedsUpdate());     // from TextWidget, since there's no Draw implementation.
    ASSERT1(!RenderFontNeedsUpdate());          // from TextWidget, since there's no Draw implementation.
    
    // this handles drawing of the background
    Widget::Draw(render_context);

    // if a picture is specified, draw the picture, otherwise it must
    // be a text label, so draw text.
    if (m_is_picture_label)
        DrawPicture(render_context);
    else
        DrawText(render_context);
}

ScreenCoordVector2 Label::Resize (ScreenCoordVector2 const &size)
{
    if (!m_is_picture_label && m_word_wrap && Size()[Dim::X] != size[Dim::X])
        DirtyTextFormatting();
    Widget::Resize(size);
    UpdateMinAndMaxSizesFromText();
    return Size();
}

void Label::HandleChangedFrameMargins ()
{
    DirtyTextFormatting();
    TextWidget::HandleChangedFrameMargins();
}

void Label::HandleChangedContentMargins ()
{
    DirtyTextFormatting();
    TextWidget::HandleChangedContentMargins();
}

void Label::DrawText (RenderContext const &render_context) const
{
    ASSERT1(!m_is_picture_label);
    ASSERT1(RenderFont().IsValid());

    ScreenCoordRect contents_rect(ContentsRect());
    if (contents_rect.IsValid())
    {
        // give it a chance to update the formatted text if
        // m_text_formatting_update_required is set
        UpdateCachedFormattedText();
        ASSERT1(!m_text_formatting_update_required);
        // generate a render context for the string drawing function
        RenderContext string_render_context(render_context);
        // calculate the clip rect
        string_render_context.ApplyClipRect(contents_rect);
        // calculate the color mask
        string_render_context.ApplyColorMask(RenderTextColor());
        // set up the GL clip rect
        Context().GetScreen().SetViewport(string_render_context.ClipRect());
        // draw the text
        ASSERT1(m_line_format_vector_source != NULL);
        RenderFont()->DrawLineFormattedText(
            string_render_context,
            ContentsRect(),
            m_line_format_vector_source->c_str(),
            m_line_format_vector,
            m_alignment);
    }
}

void Label::DrawPicture (RenderContext const &render_context) const
{
    ASSERT1(m_is_picture_label);
    if (!RenderPicture().IsValid())
        return;

    // this is the rectangle which will be used to render the picture.
    ScreenCoordRect picture_rect;
    ScreenCoordRect contents_rect(ContentsRect());

    if (m_picture_keeps_aspect_ratio)
    {
        ASSERT1(m_picture->Width() > 0);
        ASSERT1(m_picture->Height() > 0);
        // determine if the picture should be fit to the width
        // of the label or the height of the label.
        if (contents_rect.Height() * m_picture->Width() /
            m_picture->Height()
            <=
            contents_rect.Width())
        {
            // the picture must fit to the label's height
            ScreenCoordVector2 picture_size(
                contents_rect.Height() * m_picture->Width() / m_picture->Height(),
                contents_rect.Height());
            picture_rect = picture_size;
            picture_rect += ScreenCoordVector2((contents_rect.Width() - picture_rect.Width()) / 2, 0);
        }
        else
        {
            // the picture must fit to the label's width
            ScreenCoordVector2 picture_size(
                contents_rect.Width(),
                contents_rect.Width() * m_picture->Height() / m_picture->Width());
            picture_rect = picture_size;
            picture_rect += ScreenCoordVector2(0, (contents_rect.Height() - picture_rect.Height()) / 2);
        }
        picture_rect += contents_rect.BottomLeft();
    }
    else
    {
        picture_rect = contents_rect;
    }

    Render::DrawScreenRectTexture(render_context, **RenderPicture(), picture_rect);
}

void Label::SetRenderFont (Resource<Font> const &render_font)
{
    if (m_is_picture_label)
        return;

    TextWidget::SetRenderFont(render_font);

    DirtyTextFormatting();
    UpdateMinAndMaxSizesFromText();
}

void Label::SetRenderPicture (Resource<GlTexture> const &render_picture)
{
    if (!m_is_picture_label)
        return;

    m_render_picture = render_picture;
}

void Label::UpdateRenderFont ()
{
    if (m_is_picture_label)
        ASSERT1(!RenderFontNeedsUpdate());
    else
        TextWidget::UpdateRenderFont();
}

void Label::UpdateRenderPicture ()
{
    m_render_picture_needs_update = false;
    SetRenderPicture(Picture());
}

ScreenCoordRect Label::TextRect () const
{
    ASSERT1(!m_is_picture_label);
    ASSERT1(RenderFont().IsValid());

    UpdateCachedFormattedText();
    return RenderFont()->StringRect(m_line_format_vector);
}

void Label::UpdateMinAndMaxSizesFromText ()
{
    // no text-based min/max sizes if this is a picture label
    if (m_is_picture_label)
        return;

    if (RenderFontNeedsUpdate())
        UpdateRenderFont();
    ASSERT1(!RenderFontNeedsUpdate());
    ASSERT1(RenderFont().IsValid());

    // if word-wrapping is enabled, then we can't base the min/max width
    // of this widget off the text, because the width of the widget dictates
    // the formatting of the text.
    if (m_word_wrap)
    {
        m_is_min_width_fixed_to_text_width = false;
        m_is_max_width_fixed_to_text_width = false;
    }

    UpdateCachedFormattedText();
    TextWidget::UpdateMinAndMaxSizesFromText();
}

void Label::UpdateCachedFormattedText () const
{
    ASSERT1(!m_is_picture_label);
    ASSERT1(!RenderFontNeedsUpdate());
    ASSERT1(RenderFont().IsValid());

    // if no update was required, early-out
    if (!m_text_formatting_update_required)
        return;

    // text formatting update no longer required
    m_text_formatting_update_required = false;

    // word-wrapped text munges m_text into m_cached_formatted_text
    if (m_word_wrap)
    {
        // generate the word-wrapped string
        RenderFont()->GenerateWordWrappedString(
            m_text,
            &m_cached_formatted_text,
            ContentsRect().Size());
        m_line_format_vector_source = &m_cached_formatted_text;
    }
    // otherwise use m_text directly
    else
    {
        m_line_format_vector_source = &m_text;
    }

    // create the line format vector from whatever text was chosen above
    RenderFont()->GenerateLineFormatVector(m_line_format_vector_source->c_str(), &m_line_format_vector);
}

} // end of namespace Xrb
