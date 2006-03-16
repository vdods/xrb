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

#include "xrb_gl.h"
#include "xrb_input_events.h"

namespace Xrb
{

Screen::~Screen ()
{
}

Screen *Screen::Create (
    EventQueue *const owner_event_queue,
    ScreenCoord const width,
    ScreenCoord const height,
    Uint32 const bit_depth,
    Uint32 const flags)
{
    // maybe change these to actual code-checks and error handling
    ASSERT1(owner_event_queue != NULL)
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
    retval = new Screen(owner_event_queue);
    // this resizing must happen before the widget skin is created.
    retval->m_current_video_resolution.SetComponents(width, height);
    retval->MoveTo(ScreenCoordVector2::ms_zero);
    retval->Widget::Resize(retval->m_current_video_resolution);
    retval->FixSize(retval->m_current_video_resolution);
    retval->m_widget_skin = new WidgetSkin(retval);
    retval->m_delete_widget_skin = true;
    retval->InitializeFromWidgetSkinProperties();
    retval->SetBackground(NULL);

    return retval;
}

void Screen::Draw () const
{
    // this function encompasses all drawing.

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // create the render context.  we must do it manually because the
    // top-level widget (Screen) has no parent to do it automatically.
    RenderContext render_context(GetScreenRect(), Color(1.0, 1.0, 1.0, 1.0));
    // set the GL clip rect (must do it manually for the same reason
    // as the render context).
    render_context.SetupGLClipRect();

    // call draw on the Widget base class.
    Widget::Draw(render_context);

    // all drawing is complete for this frame, so flush it down
    // and then swap the backbuffer.
    glFlush();
    SDL_GL_SwapBuffers();

    // record this frame in the framerate calculator
    m_framerate_calculator.AddFrameTime(GetMostRecentFrameTime());
}

Screen::Screen (EventQueue *const owner_event_queue)
    :
    Widget(NULL, "Screen"),
    m_sender_quit_requested(this)
{
    m_is_quit_requested = false;
    m_current_video_resolution = ScreenCoordVector2::ms_zero;

    // when adding a Widget to its parent, this is done by the parent,
    // but since Screen has no parent Widget, we must do it manually.
    SetOwnerEventQueue(owner_event_queue);
}

void Screen::ProcessFrameOverride ()
{
    Widget::ProcessFrameOverride();

    // nothing needs to be done for now
}

bool Screen::ProcessEventOverride (Event const *const e)
{
    ASSERT1(e != NULL)

    switch (e->GetType())
    {
        case Event::QUIT:
            fprintf(stderr,
                    "Screen::ProcessEventOverride(); quit "
                    "event received: quitting\n");
            m_is_quit_requested = true;
            m_sender_quit_requested.Signal();
            return true;

        case Event::KEYDOWN:
            switch (static_cast<EventKey const *const>(e)->GetKeyCode())
            {
//                 case Key::PRINT:
//                     fprintf(stderr,
//                             "Screen::ProcessEventOverride(); Key::PRINT "
//                             "- capturing screenshot to screenshot.png\n");
//                     SDL::WritePNG(m_surface, "screenshot.png");
//                     return true;
/*
                case Key::RETURN:
                    fprintf(stderr, "Screen::ProcessEventOverride(); toggling input grab\n");
                    if (SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_ON) {
                        SDL_ShowCursor(SDL_ENABLE);
                        SDL_WM_GrabInput(SDL_GRAB_OFF);
                    } else {
                        SDL_ShowCursor(SDL_DISABLE);
                        SDL_WM_GrabInput(SDL_GRAB_ON);
                    }

                    return true;
*/
/*
                case Key::F5:
                    SetWidgetSkinTextureFilename(
                        WidgetSkin::CHECK_BOX_CHECK_TEXTURE,
                        "resources/ui/radiobutton_dot.png");
                    break;

                case Key::F6:
                    SetWidgetSkinTextureFilename(
                        WidgetSkin::CHECK_BOX_CHECK_TEXTURE,
                        "resources/ui/black_checkmark.png");
                    break;
*/
                default:
                    break;
            }
            break;

        default:
            break;
    }

    // pass the event to the Widget base class
    return Widget::ProcessEventOverride(e);
}

} // end of namespace Xrb
