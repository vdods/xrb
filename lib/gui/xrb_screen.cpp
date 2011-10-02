// ///////////////////////////////////////////////////////////////////////////
// xrb_screen.cpp by Victor Dods, created 2004/06/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_screen.hpp"

#include <iomanip>
#include <sstream>

#include "xrb_eventqueue.hpp"
#include "xrb_gl.hpp"
#include "xrb_gui_events.hpp"
#include "xrb_input_events.hpp"
#include "xrb_pal.hpp"
#include "xrb_texture.hpp"
#include "xrb_widgetcontext.hpp"

namespace Xrb {

Screen::~Screen ()
{
    // because we created our own event queue in the constructor, we must
    // delete it ourselves here in the destructor.  BUT, before we do that,
    // we must delete all child widgets, because they will potentially access
    // the condemned owner EventQueue via ~EventHandler.
    DeleteAllChildren();

    // unset the event queue before deleting the context (because the context owns the event queue)
    SetOwnerEventQueue(NULL);
    
    // destroy the WidgetContext (this doesn't cause a problem with the
    // Add/RemoveWidget tracking, since this Screen widget is not tracked).
    // this must be done before the singletons and the video are shutdown.
    delete m_context;
    m_context = NULL;

    Singleton::ShutdownGl();
    Singleton::Pal().ShutdownVideo();
}

Screen *Screen::Create (ScreenCoord width, ScreenCoord height, Uint32 bit_depth, bool fullscreen, Sint32 angle)
{
    // maybe change these to actual code-checks and error handling
    ASSERT1(width > 0);
    ASSERT1(height > 0);
    ASSERT1(bit_depth > 0);
    ASSERT1(angle % 90 == 0 && "angle must be a multiple of 90");

    // normalize the angle to the range [0, 360)
    while (angle < 0)
        angle += 360;
    angle %= 360;

    if (Singleton::Pal().InitializeVideo(width, height, bit_depth, fullscreen) != Pal::SUCCESS)
        return NULL;

    Singleton::InitializeGl();

    // it is important that all the video init and GL init
    // happens before this constructor, because the Widget constructor
    // is called, which loads textures and makes GL calls.
    Screen *retval = new Screen(angle);
    retval->m_device_size.SetComponents(width, height);         // TODO: this initialization should be moved to a constructor
    retval->m_original_screen_size = retval->RotatedScreenSize(retval->m_device_size);
    // this resizing must happen before the style sheet is created.
    retval->MoveTo(ScreenCoordVector2::ms_zero);
    retval->ContainerWidget::Resize(retval->m_original_screen_size);
    retval->FixSize(retval->m_original_screen_size);
    retval->SetBackgroundStyle(StyleSheet::BackgroundType::TRANSPARENT);

    return retval;
}

void Screen::RequestQuit ()
{
    if (!m_is_quit_requested)
    {
        m_is_quit_requested = true;
        std::cerr << "Screen::RequestQuit();" << std::endl;
        m_sender_quit_requested.Signal();
    }
}

void Screen::RequestScreenshot (std::string const &screenshot_path)
{
    std::cerr << "Screen::RequestScreenshot();";
    if (!m_screenshot_path.empty())
        std::cerr << " canceling screenshot request \"" << m_screenshot_path << '"';
    if (!screenshot_path.empty())
    {
        m_screenshot_path = screenshot_path;
        std::cerr << ", setting screenshot request \"" << m_screenshot_path << '"';
    }
    std::cerr << std::endl;
}

void Screen::SetProjectionMatrix (ScreenCoordRect const &clip_rect)
{
    ASSERT1(clip_rect.IsValid());

    // set up the GL projection matrix here.
    glMatrixMode(GL_PROJECTION);
    // there is an extra copy of the matrix on the stack so don't
    // have to worry about fucking it up.
    glPopMatrix();
    glPushMatrix();
    glOrtho(
        clip_rect.Left(), clip_rect.Right(),
        clip_rect.Bottom(), clip_rect.Top(),
        -1.0, 1.0); // these values (-1, 1) are arbitrary
}

void Screen::SetViewport (ScreenCoordRect const &clip_rect) const
{
    SetProjectionMatrix(clip_rect);

    // set up the viewport which is the rectangle on screen which
    // will be rendered to.  this also properly sets up the clipping
    // planes.

    ASSERT1(clip_rect.IsValid());
    ScreenCoordRect rotated_clip_rect(RotatedScreenRect(clip_rect));
    glViewport(
        rotated_clip_rect.Left(),
        rotated_clip_rect.Bottom(),
        rotated_clip_rect.Width(),
        rotated_clip_rect.Height());
}

void Screen::Draw (Time render_time)
{
    // NOTE: this method encompasses all drawing.

#if !defined(WIN32)
    ASSERT1(Gl::Integer(GL_MODELVIEW_STACK_DEPTH)  == 1 && "mismatched push/pop for GL_MODELVIEW matrix stack");
    ASSERT1(Gl::Integer(GL_PROJECTION_STACK_DEPTH) == 1 && "mismatched push/pop for GL_PROJECTION matrix stack");
    ASSERT1(Gl::Integer(GL_TEXTURE_STACK_DEPTH)    == 1 && "mismatched push/pop for GL_TEXTURE matrix stack");
#endif // !defined(WIN32)

    // clear the color buffer to the Screen's color bias (because that's
    // what you'd get if you applied the bias to all-black).
    glClearColor(
        ColorBias()[Dim::R]*ColorBias()[Dim::A],
        ColorBias()[Dim::G]*ColorBias()[Dim::A],
        ColorBias()[Dim::B]*ColorBias()[Dim::A],
        1.0f); // alpha value is irrelevant, glColorMask has disabled writing to alpha channel
    glClear(GL_COLOR_BUFFER_BIT);
    // reset some matrices (NOT the texture matrix, because the only thing
    // that's allowed to modify the texture matrix is Gl::BindAtlas)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // set up the screen rotation by using the projection matrix
    glRotatef(m_angle, 0.0f, 0.0f, 1.0f);
    // this is a way to save a copy of the matrix so that SetViewport can
    // not have to worry about fucking it up.
    glPushMatrix();

    // make sure the screen rect we're constructing the render context
    // with does not extend past the physical screen
    ScreenCoordRect screen_rect(ScreenRect());
    ASSERT1(screen_rect.Left() == 0);
    ASSERT1(screen_rect.Bottom() == 0);
    if (screen_rect.Width() > m_original_screen_size[Dim::X])
        screen_rect.SetWidth(m_original_screen_size[Dim::X]);
    if (screen_rect.Height() > m_original_screen_size[Dim::Y])
        screen_rect.SetHeight(m_original_screen_size[Dim::Y]);

    // create the render context.  we must do it manually because the
    // top-level widget (Screen) has no parent to do it automatically.
    // the default color bias is transparent black, which is the identity
    // for the blending function composition operation.  the default
    // color mask is opaque white, which is the identity for the color
    // masking operation.
    RenderContext render_context(screen_rect, ColorBias(), ColorMask(), render_time);
    // set the GL clip rect (must do it manually for the same reason
    // as the render context).
    SetViewport(render_context.ClipRect());

    // call draw on the ContainerWidget base class.
    PreDraw();
    ContainerWidget::Draw(render_context);
    PostDraw();

    // create a render context for the child widgets
    RenderContext child_render_context(render_context);
    // if there are modal widgets, draw them from the bottom of the stack, up.
    for (WidgetList::const_iterator it = m_modal_child_widget_stack.begin(),
                                    it_end = m_modal_child_widget_stack.end();
         it != it_end;
         ++it)
    {
        ASSERT1(*it != NULL);
        Widget &modal_widget = **it;

        // skip hidden modal widgets
        if (modal_widget.IsHidden())
            continue;

        // calculate the drawing clip rect from this widget's clip rect
        // and the child widget's virtual rect.
        child_render_context.SetClipRect(render_context.ClippedRect(modal_widget.ScreenRect()));
        // don't even bother drawing a modal widget if this resulting
        // clip rect is invalid (0 area)
        if (child_render_context.ClipRect().IsValid())
        {
            // set the color bias and color mask
            child_render_context.ColorBias() = render_context.ColorBias();
            child_render_context.ApplyColorBias(modal_widget.ColorBias());
            child_render_context.ColorMask() = render_context.ColorMask();
            child_render_context.ApplyColorMask(modal_widget.ColorMask());

            ASSERT1(modal_widget.IsEnabled());
            // set up the clip rect for the child
            Context().GetScreen().SetViewport(child_render_context.ClipRect());
            // do the actual draw call
            modal_widget.Draw(child_render_context);
        }
    }
    
    // need to pop the matrix we saved above.
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

#if !defined(WIN32)
    ASSERT1(Gl::Integer(GL_MODELVIEW_STACK_DEPTH)  == 1 && "mismatched push/pop for GL_MODELVIEW matrix stack");
    ASSERT1(Gl::Integer(GL_PROJECTION_STACK_DEPTH) == 1 && "mismatched push/pop for GL_PROJECTION matrix stack");
    ASSERT1(Gl::Integer(GL_TEXTURE_STACK_DEPTH)    == 1 && "mismatched push/pop for GL_TEXTURE matrix stack");
#endif // !defined(WIN32)

    // all drawing is complete for this frame, so flush it down
    // and then swap the backbuffer.
    glFlush();
    Singleton::Pal().FinishFrame();

    // we're done drawing, now check for a requested screenshot
    if (!m_screenshot_path.empty())
    {
        Texture *screenshot = Texture::Create(m_device_size, Texture::UNINITIALIZED);
        ASSERT1(screenshot != NULL);
        std::cerr << "Screen::Draw(); saving screenshot \"" << m_screenshot_path << '"' << std::endl;
        glReadPixels(0, 0, m_device_size[Dim::X], m_device_size[Dim::Y], GL_RGBA, GL_UNSIGNED_BYTE, screenshot->Data());
        screenshot->Save(m_screenshot_path);
        delete screenshot;
        m_screenshot_path.clear();
    }
}

bool Screen::IsAttachedAsModalChildWidget (Widget const &widget) const
{
    return std::find(m_modal_child_widget_stack.begin(), m_modal_child_widget_stack.end(), &widget) != m_modal_child_widget_stack.end();
}

void Screen::AttachAsModalChildWidget (Widget &child)
{
    ASSERT1(!child.IsModal());
    ASSERT1(child.IsEnabled());
    ASSERT1(!child.IsScreen());

    // stick the modal widget on the modal widget stack
    m_modal_child_widget_stack.push_back(&child);
    // first add it as an ordinary child, specifying focus=true
    AttachChild(&child);
    // turn off mouseover on this top level widget and all subordinate widgets that have mouseover focus
    MouseoverOffWidgetLine();
}

void Screen::DetachAsModalChildWidget (Widget &child)
{
    ASSERT1(child.IsModal());
    ASSERT1(child.IsEnabled());

    // remove the child from this as a ContainerWidget
    DetachChild(&child);
    // remove the child from the modal child widget stack
    WidgetList::iterator it = std::find(m_modal_child_widget_stack.begin(), m_modal_child_widget_stack.end(), &child);
    ASSERT1(it != m_modal_child_widget_stack.end());
    m_modal_child_widget_stack.erase(it);
}

Screen::Screen (Sint32 angle)
    :
    ContainerWidget(*CreateAndInitializeWidgetContext(), "Screen"),
    m_angle(angle),
    m_sender_quit_requested(this),
    m_receiver_request_quit(&Screen::RequestQuit, this)
{
    ASSERT1(m_angle % 90 == 0);
    ASSERT1(m_angle >= 0 && m_angle < 360);

    m_remove_from_widget_context_upon_destruction = false;
    m_is_quit_requested = false;
    m_device_size = ScreenCoordVector2::ms_zero;
    m_original_screen_size = ScreenCoordVector2::ms_zero;
}

void Screen::HandleFrame ()
{
    ContainerWidget::HandleFrame();

    // nothing needs to be done for now
}

bool Screen::HandleEvent (Event const &e)
{
    // basically all non-input events make it through to this widget.  except for Event::QUIT,
    // these should be the same as in Widget::HandleEvent.
    switch (e.GetEventType())
    {
        case Event::DETACH_AND_DELETE_CHILD_WIDGET:
            return ProcessDetachAndDeleteChildWidgetEvent(dynamic_cast<EventDetachAndDeleteChildWidget const &>(e));
            
        case Event::QUIT:
            RequestQuit();
            return true;

        case Event::STATE_MACHINE_INPUT:
            return ProcessStateMachineInputEvent(dynamic_cast<EventStateMachineInput const &>(e));

        case Event::CUSTOM:
            return ProcessCustomEvent(dynamic_cast<EventCustom const &>(e));
            
        default:
            break;
    }

    // check for screenshot/dump-atlas keys
    if (e.GetEventType() == Event::KEYDOWN)
    {
        EventKeyDown const &key_down_event = dynamic_cast<EventKeyDown const &>(e);
        if (key_down_event.KeyCode() == Key::PRINT)
        {
            static Uint32 screenshot_count = 0; // temp hack for screenshot numbering TODO real numbering
            RequestScreenshot(FORMAT("screenshot" << std::setw(3) << std::setfill('0') << screenshot_count++ << ".png"));
        }
        else if (key_down_event.KeyCode() == Key::SCROLLLOCK)
            Singleton::Gl().DumpAtlases("atlas"); // TODO: real atlas filename
    }

    // special handling for the root widget (Screen)
    {
        if (e.IsMouseEvent())
        {
            EventMouse const &mouse_event = dynamic_cast<EventMouse const &>(e);

            // transform the mouse event screen position
            mouse_event.SetPosition(RotatedScreenPosition(mouse_event.Position()));
        }

        if (e.IsMouseMotionEvent())
        {
            EventMouseMotion const &mouse_motion_event = dynamic_cast<EventMouseMotion const &>(e);

            // transform the mouse motion event position delta
            mouse_motion_event.SetDelta(RotatedScreenVector(mouse_motion_event.Delta()));

            // generate a mouseover event from the mouse motion event
            EventMouseover mouseover_event(mouse_motion_event.Position(), mouse_motion_event.GetTime());
            ProcessEvent(mouseover_event);
        }

        if (e.GetEventType() == Event::MOUSEBUTTONDOWN)
        {
            // create a focus event
            EventFocus focus_event(dynamic_cast<EventMouseButton const &>(e).Position(), e.GetTime());
            // send it to the event processor
            ProcessEvent(focus_event);
        }

        // get the top of the modal widget stack
        Widget *modal_widget = NULL;
        for (ContainerWidget::WidgetList::reverse_iterator it = m_modal_child_widget_stack.rbegin(),
                                                           it_end = m_modal_child_widget_stack.rend();
             it != it_end;
             ++it)
        {
            ASSERT1(*it != NULL);
            Widget &widget = **it;
            if (!widget.IsHidden())
            {
                modal_widget = &widget;
                break;
            }
        }

        // if there's a non-hidden modal widget, send the event to it.
        if (modal_widget != NULL)
        {
            // if the modal widget has mouse grab, send all input events to it
            if (modal_widget->IsMouseGrabbed() && e.IsInputEvent())
                return modal_widget->ProcessEvent(e);

            // check if this is a mouse event and it doesn't fall inside the
            // top modal widget.  if so, throw the event out.
            if (e.IsMouseEvent())
                if (!modal_widget->ScreenRect().IsPointInside(dynamic_cast<EventMouse const &>(e).Position()))
                    return false;

            return modal_widget->ProcessEvent(e);
        }
    }

    // pass the event to the ContainerWidget base class
    return ContainerWidget::HandleEvent(e);
}

bool Screen::InternalProcessFocusEvent (EventFocus const &e)
{
    // if there are any modal widgets, then focus can only go the top unhidden modal widget.
    Widget *modal_widget = NULL;
    for (WidgetList::reverse_iterator it = m_modal_child_widget_stack.rbegin(),
                                      it_end = m_modal_child_widget_stack.rend();
         it != it_end;
         ++it)
    {
        Widget *widget = *it;
        ASSERT1(widget != NULL);
        if (!widget->IsHidden())
        {
            modal_widget = widget;
            break;
        }
    }

    if (modal_widget != NULL)
    {
        if (modal_widget->ScreenRect().IsPointInside(e.Position()))
            return modal_widget->InternalProcessFocusEvent(e);
        else
            return false;
    }

    // otherwise fall back to the superclass' method.
    return ContainerWidget::InternalProcessFocusEvent(e);
}

ScreenCoordVector2 Screen::RotatedScreenSize (ScreenCoordVector2 const &v) const
{
    ASSERT1(m_angle % 90 == 0);
    ASSERT1(m_angle >= 0 && m_angle < 360);
    switch (m_angle)
    {
        default:
        case 0:
        case 180: return v;
        case 90:
        case 270: return ScreenCoordVector2(v[Dim::Y], v[Dim::X]);
    }
}

ScreenCoordVector2 Screen::RotatedScreenPosition (ScreenCoordVector2 const &v) const
{
    ASSERT1(m_angle % 90 == 0);
    ASSERT1(m_angle >= 0 && m_angle < 360);
    switch (m_angle)
    {
        default:
        case 0:   return v;
        case 90:  return ScreenCoordVector2(v[Dim::Y], m_device_size[Dim::X]-v[Dim::X]);
        case 180: return m_device_size - v;
        case 270: return ScreenCoordVector2(m_device_size[Dim::Y]-v[Dim::Y], v[Dim::X]);
    }
}

ScreenCoordVector2 Screen::RotatedScreenVector (ScreenCoordVector2 const &v) const
{
    ASSERT1(m_angle % 90 == 0);
    ASSERT1(m_angle >= 0 && m_angle < 360);
    switch (m_angle)
    {
        default:
        case 0:   return v;
        case 90:  return ScreenCoordVector2(v[Dim::Y], -v[Dim::X]);
        case 180: return -v;
        case 270: return ScreenCoordVector2(-v[Dim::Y], v[Dim::X]);
    }
}

ScreenCoordRect Screen::RotatedScreenRect (ScreenCoordRect const &r) const
{
    ASSERT1(m_angle % 90 == 0);
    ASSERT1(m_angle >= 0 && m_angle < 360);
    switch (m_angle)
    {
        default:
        case 0:
            return r;

        case 90:
            return ScreenCoordRect(
                m_device_size[Dim::X] - r.Top(),
                r.Left(),
                m_device_size[Dim::X] - r.Top() + r.Height(),
                r.Left() + r.Width());

        case 180:
            return ScreenCoordRect(
                m_device_size[Dim::X] - r.Right(),
                m_device_size[Dim::Y] - r.Top(),
                m_device_size[Dim::X] - r.Right() + r.Width(),
                m_device_size[Dim::Y] - r.Top() + r.Height());

        case 270:
            return ScreenCoordRect(
                r.Bottom(),
                m_device_size[Dim::Y] - r.Right(),
                r.Bottom() + r.Height(),
                m_device_size[Dim::Y] - r.Right() + r.Width());
    }
}

WidgetContext *Screen::CreateAndInitializeWidgetContext ()
{
    // this method is called as a parameter to Widget's constructor,
    // so nothing in Screen is constructed yet, so be careful.
    ASSERT1(this != NULL); // why not check, just this once.
    m_context = new WidgetContext(*this);
    ASSERT1(m_context != NULL); // why not.
    return m_context;
}

} // end of namespace Xrb
