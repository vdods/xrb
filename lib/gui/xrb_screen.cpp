// ///////////////////////////////////////////////////////////////////////////
// xrb_screen.cpp by Victor Dods, created 2004/06/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_screen.h"

#include "xrb_eventqueue.h"
#include "xrb_gl.h"
#include "xrb_gui_events.h"
#include "xrb_input_events.h"

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
    ASSERT1(GetOwnerEventQueue() != NULL)
    delete GetOwnerEventQueue();
    SetOwnerEventQueue(NULL);
}

Screen *Screen::Create (
    ScreenCoord const width,
    ScreenCoord const height,
    Uint32 const bit_depth,
    Uint32 const flags)
{
    // maybe change these to actual code-checks and error handling
    ASSERT1(width > 0)
    ASSERT1(height > 0)
    ASSERT1(bit_depth > 0)

    Screen *retval = NULL;

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Surface *surface = SDL_SetVideoMode(width, height, bit_depth, flags|SDL_OPENGL);
    if (surface == NULL)
    {
        fprintf(stderr, "Screen::Create(); could not set the requested video mode\n");
        return retval;
    }

    GL::Initialize();

    // it is important that all the SDL video init and GL init
    // happens before this constructor, because the Widget constructor
    // is called, which loads textures and makes GL calls.
    retval = new Screen();
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
    ASSERT1(GL::GetMatrixStackDepth(GL_COLOR) == 1)
    ASSERT1(GL::GetMatrixStackDepth(GL_MODELVIEW) == 1)
    ASSERT1(GL::GetMatrixStackDepth(GL_PROJECTION) == 1)
    ASSERT1(GL::GetMatrixStackDepth(GL_TEXTURE) == 1)
#endif // !defined(WIN32)

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // make sure the screen rect we're constructing the render context
    // with does not extend past the physical screen
    ScreenCoordRect screen_rect(GetScreenRect());
    ASSERT1(screen_rect.GetLeft() == 0)
    ASSERT1(screen_rect.GetBottom() == 0)
    if (screen_rect.GetWidth() > m_current_video_resolution[Dim::X])
        screen_rect.SetWidth(m_current_video_resolution[Dim::X]);
    if (screen_rect.GetHeight() > m_current_video_resolution[Dim::Y])
        screen_rect.SetHeight(m_current_video_resolution[Dim::Y]);

    // create the render context.  we must do it manually because the
    // top-level widget (Screen) has no parent to do it automatically.
    RenderContext render_context(screen_rect, Color(1.0, 1.0, 1.0, 1.0));
    // set the GL clip rect (must do it manually for the same reason
    // as the render context).
    render_context.SetupGLClipRect();

    // call draw on the ContainerWidget base class.
    ContainerWidget::Draw(render_context);

#if !defined(WIN32)
    ASSERT1(GL::GetMatrixStackDepth(GL_COLOR) == 1 && "You forgot to pop a GL_COLOR matrix somewhere")
    ASSERT1(GL::GetMatrixStackDepth(GL_MODELVIEW) == 1 && "You forgot to pop a GL_MODELVIEW matrix somewhere")
    ASSERT1(GL::GetMatrixStackDepth(GL_PROJECTION) == 1 && "You forgot to pop a GL_PROJECTION matrix somewhere")
    ASSERT1(GL::GetMatrixStackDepth(GL_TEXTURE) == 1 && "You forgot to pop a GL_TEXTURE matrix somewhere")
#endif // !defined(WIN32)

    // all drawing is complete for this frame, so flush it down
    // and then swap the backbuffer.
    glFlush();
    SDL_GL_SwapBuffers();

    // record this frame in the framerate calculator
    m_framerate_calculator.AddFrameTime(GetMostRecentFrameTime());
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
    ASSERT1(e != NULL)

    switch (e->GetEventType())
    {
        case Event::QUIT:
            RequestQuit();
            return true;

        case Event::KEYDOWN:
            /*
            switch (static_cast<EventKey const *const>(e)->GetKeyCode())
            {
//                 case Key::PRINT:
//                     fprintf(stderr,
//                             "Screen::HandleEvent(); Key::PRINT "
//                             "- capturing screenshot to screenshot.png\n");
//                     SDL::WritePNG(m_surface, "screenshot.png");
//                     return true;
                case Key::SCROLLLOCK:
                    // TODO: don't capture input if fullscreen is set
                    fprintf(stderr, "Screen::HandleEvent(); toggling input grab\n");
                    if (SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_ON) {
                        SDL_ShowCursor(SDL_ENABLE);
                        SDL_WM_GrabInput(SDL_GRAB_OFF);
                    } else {
                        SDL_ShowCursor(SDL_DISABLE);
                        SDL_WM_GrabInput(SDL_GRAB_ON);
                    }

                    return true;

                default:
                    break;
            }
            */
            break;

        default:
            break;
    }

    // special handling for the top-level parent widget (Screen)
    {
        if (e->GetIsMouseMotionEvent())
        {
            EventMouseMotion const *mouse_motion_event =
                DStaticCast<EventMouseMotion const *>(e);

            // generate a mouseover event from the mouse motion event
            EventMouseover mouseover_event(
                mouse_motion_event->GetPosition(),
                mouse_motion_event->GetTime());
            ProcessEvent(&mouseover_event);
        }

        if (e->GetEventType() == Event::MOUSEBUTTONDOWN)
        {
            // create a focus event
            EventFocus focus_event(
                DStaticCast<EventMouseButton const *>(e)->GetPosition(),
                e->GetTime());
            // send it to the event processor
            ProcessEvent(&focus_event);
        }

        // get the top of the modal widget stack
        Widget *modal_widget = NULL;
        for (ContainerWidget::WidgetListReverseIterator
                 it = m_modal_widget_stack.rbegin(),
                 it_end = m_modal_widget_stack.rend();
             it != it_end;
             ++it)
        {
            Widget *widget = *it;
            ASSERT1(widget != NULL)
            if (!widget->GetIsHidden())
            {
                modal_widget = widget;
                break;
            }
        }

        // if there's a non-hidden modal widget, send the event to it.
        if (modal_widget != NULL)
        {
            // if the modal widget has mouse grab, send all input events to it
            if (modal_widget->GetIsMouseGrabbed() && e->GetIsInputEvent())
                return modal_widget->ProcessEvent(e);

            // check if this is a mouse event and it doesn't fall inside the
            // top modal widget.  if so, throw the event out.
            if (e->GetIsMouseEvent())
                if (!modal_widget->GetScreenRect().GetIsPointInside(DStaticCast<EventMouse const *>(e)->GetPosition()))
                    return false;

            return modal_widget->ProcessEvent(e);
        }
    }

    // pass the event to the ContainerWidget base class
    return ContainerWidget::HandleEvent(e);
}

} // end of namespace Xrb
