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

#include "xrb_eventqueue.hpp"
#include "xrb_gl.hpp"
#include "xrb_gui_events.hpp"
#include "xrb_input_events.hpp"
#include "xrb_pal.hpp"
#include "xrb_texture.hpp"

namespace Xrb
{

Screen::~Screen ()
{
    // because we created our own event queue in the constructor, we must
    // delete it ourselves here in the destructor.  BUT, before we do that,
    // we must delete all child widgets, because they will potentially access
    // the condemned owner EventQueue via ~EventHandler.
    DeleteAllChildren();
    // now delete the owner EventQueue.
    ASSERT1(OwnerEventQueue() != NULL);
    delete OwnerEventQueue();
    SetOwnerEventQueue(NULL);

    ReleaseAllWidgetSkinResources();

    Singleton::ShutdownGl();
    Singleton::Pal().ShutdownVideo();
}

Screen *Screen::Create (
    ScreenCoord width,
    ScreenCoord height,
    Uint32 bit_depth,
    bool fullscreen,
    Sint32 angle)
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
    retval->m_device_size.SetComponents(width, height);
    retval->m_original_screen_size = retval->RotatedScreenSize(retval->m_device_size);
    // this resizing must happen before the widget skin is created.
    retval->MoveTo(ScreenCoordVector2::ms_zero);
    retval->ContainerWidget::Resize(retval->m_original_screen_size);
    retval->FixSize(retval->m_original_screen_size);
    retval->m_widget_skin = new WidgetSkin(retval);
    retval->m_delete_widget_skin = true;
    retval->InitializeFromWidgetSkinProperties();
    retval->SetBackground(NULL);

    return retval;
}

void Screen::RequestQuit ()
{
    if (!m_is_quit_requested)
    {
        m_is_quit_requested = true;
        fprintf(stderr, "Screen::RequestQuit();\n");
        m_sender_quit_requested.Signal();
    }
}

void Screen::RequestScreenshot (std::string const &screenshot_path)
{
    fprintf(stderr, "Screen::RequestScreenshot();");
    if (!m_screenshot_path.empty())
        fprintf(stderr, " canceling screenshot request \"%s\"", m_screenshot_path.c_str());
    if (!screenshot_path.empty())
    {
        m_screenshot_path = screenshot_path;
        fprintf(stderr, ", setting screenshot request \"%s\"", m_screenshot_path.c_str());
    }
    fprintf(stderr, "\n");
}

void Screen::SetViewport (ScreenCoordRect const &clip_rect) const
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

    // set up the viewport which is the rectangle on screen which
    // will be rendered to.  this also properly sets up the clipping
    // planes.

    ScreenCoordRect rotated_clip_rect(RotatedScreenRect(clip_rect));
    glViewport(
        rotated_clip_rect.Left(),
        rotated_clip_rect.Bottom(),
        rotated_clip_rect.Width(),
        rotated_clip_rect.Height());
}

void Screen::Draw (Float real_time) const
{
    // NOTE: this method encompasses all drawing.

#if !defined(WIN32)
    ASSERT1(Gl::Integer(GL_MODELVIEW_STACK_DEPTH)  == 1 && "mismatched push/pop for GL_MODELVIEW matrix stack");
    ASSERT1(Gl::Integer(GL_PROJECTION_STACK_DEPTH) == 1 && "mismatched push/pop for GL_PROJECTION matrix stack");
    ASSERT1(Gl::Integer(GL_TEXTURE_STACK_DEPTH)    == 1 && "mismatched push/pop for GL_TEXTURE matrix stack");
#endif // !defined(WIN32)

    // clear the color buffer to the Screen's color bias (because that's
    // what you'd get if you applied the bias to all-black).
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClearColor(
        ColorBias()[Dim::R]*ColorBias()[Dim::A],
        ColorBias()[Dim::G]*ColorBias()[Dim::A],
        ColorBias()[Dim::B]*ColorBias()[Dim::A],
        1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // don't bother writing to the alpha channel
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
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
    RenderContext render_context(screen_rect, ColorBias(), ColorMask(), real_time);
    // set the GL clip rect (must do it manually for the same reason
    // as the render context).
    SetViewport(render_context.ClipRect());

    // call draw on the ContainerWidget base class.
    ContainerWidget::Draw(render_context);

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
        fprintf(stderr, "Screen::Draw(); saving screenshot \"%s\"\n", m_screenshot_path.c_str());
        glReadPixels(0, 0, m_device_size[Dim::X], m_device_size[Dim::Y], GL_RGBA, GL_UNSIGNED_BYTE, screenshot->Data());
        screenshot->Save(m_screenshot_path);
        delete screenshot;
        m_screenshot_path.clear();
    }
}

Screen::Screen (Sint32 angle)
    :
    ContainerWidget(NULL, "Screen"),
    m_angle(angle),
    m_sender_quit_requested(this),
    m_receiver_request_quit(&Screen::RequestQuit, this)
{
    ASSERT1(m_angle % 90 == 0);
    ASSERT1(m_angle >= 0 && m_angle < 360);

    m_is_quit_requested = false;
    m_device_size = ScreenCoordVector2::ms_zero;
    m_original_screen_size = ScreenCoordVector2::ms_zero;

    // Screen creates its own EventQueue (which is the
    // master EventQueue for all the widgets it commands).
    SetOwnerEventQueue(new EventQueue());
}

void Screen::HandleFrame ()
{
    ContainerWidget::HandleFrame();

    // nothing needs to be done for now
}

bool Screen::HandleEvent (Event const *const e)
{
    ASSERT1(e != NULL);

    switch (e->GetEventType())
    {
        case Event::QUIT:
            RequestQuit();
            return true;

        default:
            break;
    }

    // check for screenshot/dump-atlas keys
    if (e->GetEventType() == Event::KEYDOWN)
    {
        EventKeyDown const *key_down_event = DStaticCast<EventKeyDown const *>(e);
        if (key_down_event->KeyCode() == Key::PRINT)
            RequestScreenshot("screenshot.png"); // TODO: real screenshot filename
        else if (key_down_event->KeyCode() == Key::SCROLLLOCK)
            Singleton::Gl().DumpAtlases("atlas"); // TODO: real atlas filename
    }

    // special handling for the top-level parent widget (Screen)
    {
        if (e->IsMouseEvent())
        {
            EventMouse const *mouse_event = DStaticCast<EventMouse const *>(e);

            // transform the mouse event screen position
            mouse_event->SetPosition(RotatedScreenPosition(mouse_event->Position()));
        }

        if (e->IsMouseMotionEvent())
        {
            EventMouseMotion const *mouse_motion_event =
                DStaticCast<EventMouseMotion const *>(e);

            // transform the mouse motion event position delta
            mouse_motion_event->SetDelta(RotatedScreenVector(mouse_motion_event->Delta()));

            // generate a mouseover event from the mouse motion event
            EventMouseover mouseover_event(
                mouse_motion_event->Position(),
                mouse_motion_event->Time());
            ProcessEvent(&mouseover_event);
        }

        if (e->GetEventType() == Event::MOUSEBUTTONDOWN)
        {
            // create a focus event
            EventFocus focus_event(
                DStaticCast<EventMouseButton const *>(e)->Position(),
                e->Time());
            // send it to the event processor
            ProcessEvent(&focus_event);
        }

        // get the top of the modal widget stack
        Widget *modal_widget = NULL;
        for (ContainerWidget::WidgetList::reverse_iterator
                 it = m_modal_widget_stack.rbegin(),
                 it_end = m_modal_widget_stack.rend();
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

        // if there's a non-hidden modal widget, send the event to it.
        if (modal_widget != NULL)
        {
            // if the modal widget has mouse grab, send all input events to it
            if (modal_widget->IsMouseGrabbed() && e->IsInputEvent())
                return modal_widget->ProcessEvent(e);

            // check if this is a mouse event and it doesn't fall inside the
            // top modal widget.  if so, throw the event out.
            if (e->IsMouseEvent())
                if (!modal_widget->ScreenRect().IsPointInside(DStaticCast<EventMouse const *>(e)->Position()))
                    return false;

            return modal_widget->ProcessEvent(e);
        }
    }

    // pass the event to the ContainerWidget base class
    return ContainerWidget::HandleEvent(e);
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
        case 90:  return ScreenCoordVector2(-v[Dim::Y], v[Dim::X]);
        case 180: return -v;
        case 270: return ScreenCoordVector2(v[Dim::Y], -v[Dim::X]);
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

} // end of namespace Xrb
