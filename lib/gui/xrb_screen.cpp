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
    bool fullscreen)
{
    // maybe change these to actual code-checks and error handling
    ASSERT1(width > 0);
    ASSERT1(height > 0);
    ASSERT1(bit_depth > 0);

    if (Singleton::Pal().InitializeVideo(width, height, bit_depth, fullscreen) != Pal::SUCCESS)
        return NULL;

    Singleton::InitializeGl();

    // it is important that all the video init and GL init
    // happens before this constructor, because the Widget constructor
    // is called, which loads textures and makes GL calls.
    Screen *retval = new Screen();
    // this resizing must happen before the widget skin is created.
    retval->m_current_video_resolution.SetComponents(width, height);
    retval->MoveTo(ScreenCoordVector2::ms_zero);
    retval->ContainerWidget::Resize(retval->m_current_video_resolution);
    retval->FixSize(retval->m_current_video_resolution);
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

void Screen::Draw () const
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
        0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // reset all matrices
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    // make sure the screen rect we're constructing the render context
    // with does not extend past the physical screen
    ScreenCoordRect screen_rect(ScreenRect());
    ASSERT1(screen_rect.Left() == 0);
    ASSERT1(screen_rect.Bottom() == 0);
    if (screen_rect.Width() > m_current_video_resolution[Dim::X])
        screen_rect.SetWidth(m_current_video_resolution[Dim::X]);
    if (screen_rect.Height() > m_current_video_resolution[Dim::Y])
        screen_rect.SetHeight(m_current_video_resolution[Dim::Y]);

    // create the render context.  we must do it manually because the
    // top-level widget (Screen) has no parent to do it automatically.
    // the default color bias is transparent black, which is the identity
    // for the blending function composition operation.  the default 
    // color mask is opaque white, which is the identity for the color 
    // masking operation.
    RenderContext render_context(screen_rect, ColorBias(), ColorMask());
    // set the GL clip rect (must do it manually for the same reason
    // as the render context).
    render_context.SetupGLClipRect();

    // call draw on the ContainerWidget base class.
    ContainerWidget::Draw(render_context);

#if !defined(WIN32)
    ASSERT1(Gl::Integer(GL_MODELVIEW_STACK_DEPTH)  == 1 && "mismatched push/pop for GL_MODELVIEW matrix stack");
    ASSERT1(Gl::Integer(GL_PROJECTION_STACK_DEPTH) == 1 && "mismatched push/pop for GL_PROJECTION matrix stack");
    ASSERT1(Gl::Integer(GL_TEXTURE_STACK_DEPTH)    == 1 && "mismatched push/pop for GL_TEXTURE matrix stack");
#endif // !defined(WIN32)

    // all drawing is complete for this frame, so flush it down
    // and then swap the backbuffer.
    glFlush();
    Singleton::Pal().FinishFrame();
}

Screen::Screen ()
    :
    ContainerWidget(NULL, "Screen"),
    m_sender_quit_requested(this),
    m_receiver_request_quit(&Screen::RequestQuit, this)
{
    m_is_quit_requested = false;
    m_current_video_resolution = ScreenCoordVector2::ms_zero;

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

    // special handling for the top-level parent widget (Screen)
    {
        if (e->IsMouseMotionEvent())
        {
            EventMouseMotion const *mouse_motion_event =
                DStaticCast<EventMouseMotion const *>(e);

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

} // end of namespace Xrb
