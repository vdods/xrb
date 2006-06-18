// ///////////////////////////////////////////////////////////////////////////
// xrb_label.cpp by Victor Dods, created 2004/09/12
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_label.h"

#include <stdarg.h>

#include "xrb_gl.h"
#include "xrb_render.h"
#include "xrb_screen.h"

namespace Xrb
{

Label::Label (
    std::string const &text,
    Widget *const parent,
    std::string const &name)
    :
    TextWidget(text, parent, name)
{
    DirtyTextFormatting();
    m_line_format_vector_source = NULL;
    m_alignment = Alignment2(CENTER, CENTER);
    m_word_wrap = false;
    m_is_picture_label = false;
    SetIsMinHeightFixedToTextHeight(true);
    SetIsMinWidthFixedToTextWidth(true);
    ASSERT1(!m_picture.GetIsValid())
    ASSERT1(!m_render_picture.GetIsValid())
}

Label::Label (
    Resource<GLTexture> const &picture,
    Widget *const parent,
    std::string const &name)
    :
    TextWidget("", parent, name),
    m_picture(picture)
{
    // this must be done before clearing the font
    m_is_picture_label = true;
    // clear the font (because the presence or absence of a font
    // is what dictates if this is a text or picture label).
    SetFont(Resource<Font>());
    m_picture_keeps_aspect_ratio = false;
    Label::UpdateRenderPicture();
}

void Label::SetText (std::string const &text)
{
    ASSERT1(!m_is_picture_label)
    DirtyTextFormatting();
    TextWidget::SetText(text);
}

void Label::SetAlignment (Alignment2 const &alignment)
{
    ASSERT1(!m_is_picture_label)
    m_alignment = alignment;
}

void Label::SetAlignment (Uint32 const component, Alignment const alignment)
{
    ASSERT1(!m_is_picture_label)
    ASSERT1(component <= 1)
    m_alignment[component] = alignment;
}

void Label::SetWordWrap (bool const word_wrap)
{
    ASSERT1(!m_is_picture_label)
    if (m_word_wrap != word_wrap)
    {
        m_word_wrap = word_wrap;
        DirtyTextFormatting();
        // this will set all m_is_min_width_fixed_to_text_width etc to false
        UpdateMinAndMaxSizesFromText();
    }
}

void Label::SetPicture (std::string const &picture_name)
{
    ASSERT1(m_is_picture_label)
    ASSERT1(!picture_name.empty())

    Resource<GLTexture> picture =
        Singletons::ResourceLibrary().
            LoadFilename<GLTexture>(GLTexture::Create, picture_name);
    ASSERT1(picture.GetIsValid())
    if (m_picture != picture)
    {
        m_picture = picture;
        UpdateRenderPicture();
    }
}

void Label::SetPicture (Resource<GLTexture> const &picture)
{
    ASSERT1(m_is_picture_label)
    if (m_picture != picture)
    {
        m_picture = picture;
        UpdateRenderPicture();
    }
}

void Label::Draw (RenderContext const &render_context) const
{
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
    if (!m_is_picture_label && m_word_wrap && GetSize()[Dim::X] != size[Dim::X])
        DirtyTextFormatting();
    return Widget::Resize(size);
}

void Label::HandleChangedFrameMargins ()
{
    if (!m_is_picture_label && m_word_wrap)
        DirtyTextFormatting();
    TextWidget::HandleChangedFrameMargins();
}

void Label::HandleChangedContentMargins ()
{
    if (!m_is_picture_label && m_word_wrap)
        DirtyTextFormatting();
    TextWidget::HandleChangedContentMargins();
}

void Label::DrawText (RenderContext const &render_context) const
{
    ASSERT1(!m_is_picture_label)
    ASSERT1(GetRenderFont().GetIsValid())

    ScreenCoordRect contents_rect(GetContentsRect());
    if (contents_rect.GetIsValid())
    {
        // give it a chance to update the formatted text if
        // m_text_formatting_update_required is set
        UpdateCachedFormattedText();
        ASSERT1(!m_text_formatting_update_required)
        // generate a render context for the string drawing function
        RenderContext string_render_context(render_context);
        // calculate the clip rect
        string_render_context.ApplyClipRect(contents_rect);
        // calculate the color mask
        string_render_context.ApplyColorMask(GetRenderTextColor());
        // set up the GL clip rect
        string_render_context.SetupGLClipRect();
        // draw the text
        ASSERT1(m_line_format_vector_source != NULL)
        GetRenderFont()->DrawLineFormattedText(
            string_render_context,
            GetContentsRect(),
            m_line_format_vector_source->c_str(),
            m_line_format_vector,
            m_alignment);
    }
}

void Label::DrawPicture (RenderContext const &render_context) const
{
    ASSERT1(m_is_picture_label)
    if (!GetRenderPicture().GetIsValid())
        return;

    // this is the rectangle which will be used to render the picture.
    ScreenCoordRect picture_rect;
    ScreenCoordRect contents_rect(GetContentsRect());

    if (m_picture_keeps_aspect_ratio)
    {
        ASSERT1(m_picture->GetWidth() > 0)
        ASSERT1(m_picture->GetHeight() > 0)
        // determine if the picture should be fit to the width
        // of the label or the height of the label.
        if (contents_rect.GetHeight() * m_picture->GetWidth() /
            m_picture->GetHeight()
            <=
            contents_rect.GetWidth())
        {
            // the picture must fit to the label's height
            ScreenCoordVector2 picture_size(
                contents_rect.GetHeight() * m_picture->GetWidth() / m_picture->GetHeight(),
                contents_rect.GetHeight());
            picture_rect = picture_size;
            picture_rect += ScreenCoordVector2((contents_rect.GetWidth() - picture_rect.GetWidth()) / 2, 0);
        }
        else
        {
            // the picture must fit to the label's width
            ScreenCoordVector2 picture_size(
                contents_rect.GetWidth(),
                contents_rect.GetWidth() * m_picture->GetHeight() / m_picture->GetWidth());
            picture_rect = picture_size;
            picture_rect += ScreenCoordVector2(0, (contents_rect.GetHeight() - picture_rect.GetHeight()) / 2);
        }
        picture_rect += contents_rect.GetBottomLeft();
    }
    else
    {
        picture_rect = contents_rect;
    }

    Render::DrawScreenRectTexture(render_context, *GetRenderPicture(), picture_rect);
}

void Label::SetRenderFont (Resource<Font> const &render_font)
{
    ASSERT1(!m_is_picture_label)
    if (m_word_wrap)
        DirtyTextFormatting();
    TextWidget::SetRenderFont(render_font);
}

void Label:: SetRenderPicture (Resource<GLTexture> const &render_picture)
{
    ASSERT1(m_is_picture_label)
    m_render_picture = render_picture;
}

void Label::UpdateRenderFont ()
{
    if (!m_is_picture_label)
        TextWidget::UpdateRenderFont();
}

void Label::UpdateRenderPicture ()
{
    SetRenderPicture(GetPicture());
}

void Label::UpdateMinAndMaxSizesFromText ()
{
    // no text-based min/max sizes if this is a picture label
    if (m_is_picture_label)
        return;

    // if word-wrapping is enabled, then we can't base the min/max size
    // of this widget off the text, because the size of the widget dictates
    // the formatting of the text.
    if (m_word_wrap)
    {
        m_is_min_width_fixed_to_text_width = false;
        m_is_max_width_fixed_to_text_width = false;
        m_is_min_height_fixed_to_text_height = false;
        m_is_max_height_fixed_to_text_height = false;
    }

    TextWidget::UpdateMinAndMaxSizesFromText();
}

void Label::UpdateCachedFormattedText () const
{
    ASSERT1(!m_is_picture_label)
    ASSERT1(GetRenderFont().GetIsValid())

    // if no update was required, early-out
    if (!m_text_formatting_update_required)
        return;

    // text formatting update no longer required
    m_text_formatting_update_required = false;

    // word-wrapped text munges m_text into m_cached_formatted_text
    if (m_word_wrap)
    {
        // generate the word-wrapped string
        GetRenderFont()->GenerateWordWrappedString(
            m_text,
            &m_cached_formatted_text,
            GetContentsRect().GetSize());
        m_line_format_vector_source = &m_cached_formatted_text;
    }
    // otherwise use m_text directly
    else
    {
        m_line_format_vector_source = &m_text;
    }

    // create the line format vector from whatever text was chosen above
    GetRenderFont()->GenerateLineFormatVector(m_line_format_vector_source->c_str(), &m_line_format_vector);
}

} // end of namespace Xrb
