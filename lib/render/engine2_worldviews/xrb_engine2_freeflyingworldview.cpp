// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_freeflyingworldview.cpp by Victor Dods, created 2005/06/18
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_freeflyingworldview.h"

#include "xrb_engine2_worldviewwidget.h"
#include "xrb_input_events.h"
#include "xrb_keybinds.h"

namespace Xrb
{

Engine2::FreeFlyingWorldView::FreeFlyingWorldView (
    Engine2::WorldViewWidget *const parent_world_view_widget)
    :
    WorldView(parent_world_view_widget)
{
    m_key_movement_speed_factor = 0.5;

    m_zoom_accumulator = 0.0;
    m_zoom_increment = Math::Pow(2.0f, 0.25f);
    m_zoom_speed = 10.0;

    m_rotation_accumulator = 0.0;
    m_rotation_increment = 15.0;
    m_rotation_speed = 120.0;
}

Engine2::FreeFlyingWorldView::~FreeFlyingWorldView ()
{
}

bool Engine2::FreeFlyingWorldView::ProcessKeyEvent (EventKey const *const e)
{
    if (e->GetIsKeyDownEvent())
    {
        switch (e->GetKeyCode())
        {
            case Key::KP_DIVIDE:
                if (m_rotation_accumulator < 0.0)
                    m_rotation_accumulator -=
                        m_rotation_increment *
                        (Sint32)(m_rotation_accumulator / m_rotation_increment);
                m_rotation_accumulator += m_rotation_increment;
                break;

            case Key::KP_MULTIPLY:
                if (m_rotation_accumulator > 0.0)
                    m_rotation_accumulator -=
                        m_rotation_increment *
                        (Sint32)(m_rotation_accumulator / m_rotation_increment);
                m_rotation_accumulator -= m_rotation_increment;
                break;

            case Key::KP_PLUS:
                if (m_zoom_accumulator < 0.0)
                    m_zoom_accumulator -= (Sint32)m_zoom_accumulator;
                m_zoom_accumulator += 1.0;
                break;

            case Key::KP_MINUS:
                if (m_zoom_accumulator > 0.0)
                    m_zoom_accumulator -= (Sint32)m_zoom_accumulator;
                m_zoom_accumulator -= 1.0;
                break;

            default:
                break;
        }
    }

    return true;
}

bool Engine2::FreeFlyingWorldView::ProcessMouseButtonEvent (EventMouseButton const *const e)
{
    return true;
}

bool Engine2::FreeFlyingWorldView::ProcessMouseWheelEvent (EventMouseWheel const *const e)
{
    // don't allow mouse wheel input while the widget is not focused
    if (!GetParentWorldViewWidget()->GetIsFocused())
        return false;

    if (e->GetIsEitherAltKeyPressed())
    {
        // when the alt key is held down, change the view's rotation

        if (e->GetButtonCode() == Key::MOUSEWHEELUP)
        {
            if (m_rotation_accumulator > 0.0)
                m_rotation_accumulator -=
                    m_rotation_increment *
                    (Sint32)(m_rotation_accumulator / m_rotation_increment);
            m_rotation_accumulator -= m_rotation_increment;
        }
        else if (e->GetButtonCode() == Key::MOUSEWHEELDOWN)
        {
            if (m_rotation_accumulator < 0.0)
                m_rotation_accumulator -=
                    m_rotation_increment *
                    (Sint32)(m_rotation_accumulator / m_rotation_increment);
            m_rotation_accumulator += m_rotation_increment;
        }
        else
            ASSERT0(false && "Invalid mouse wheel event (button code not recognized)")
    }
    else
    {
        // otherwise, change the view's zoom

        if (e->GetButtonCode() == Key::MOUSEWHEELUP)
        {
            if (m_zoom_accumulator < 0.0)
                m_zoom_accumulator -= (Sint32)m_zoom_accumulator;
            m_zoom_accumulator += 1.0;
        }
        else if (e->GetButtonCode() == Key::MOUSEWHEELDOWN)
        {
            if (m_zoom_accumulator > 0.0)
                m_zoom_accumulator -= (Sint32)m_zoom_accumulator;
            m_zoom_accumulator -= 1.0;
        }
        else
            ASSERT0(false && "Invalid mouse wheel event (button code not recognized)")
    }

    return true;
}

bool Engine2::FreeFlyingWorldView::ProcessMouseMotionEvent (EventMouseMotion const *const e)
{
    // get the mouse movement's delta, in world coordinates
    FloatVector2 position_delta(
        GetParallaxedScreenToWorld() * FloatVector2::ms_zero -
        GetParallaxedScreenToWorld() * e->GetDelta().StaticCast<Float>());

    // the right mouse is being held and neither shift key is pressed, drag the view
    if (e->GetIsRightMouseButtonPressed() &&
        !e->GetIsEitherShiftKeyPressed())
    {
        // translate the view by the transformed delta
        MoveView(position_delta);
    }

    return true;
}

void Engine2::FreeFlyingWorldView::ProcessFrameOverride ()
{
    // don't do anything if this view is hidden
    if (GetParentWorldViewWidget()->GetIsHidden())
        return;

    // handle view zooming (from accumulated mouse wheel events)
    {
        Float zoom_by_power;

        if (m_zoom_accumulator > 0)
            zoom_by_power = Min(m_zoom_accumulator,
                                m_zoom_speed*GetFrameDT());
        else if (m_zoom_accumulator < 0)
            zoom_by_power = Max(m_zoom_accumulator,
                                -m_zoom_speed*GetFrameDT());
        else
            zoom_by_power = 0.0;

        ZoomView(Math::Pow(m_zoom_increment, zoom_by_power));
        m_zoom_accumulator -= zoom_by_power;
    }

    // handle view rotation (from acculumated mouse motion events)
    {
        Float rotate_by_angle;

        if (m_rotation_accumulator > 0)
            rotate_by_angle = Min(m_rotation_accumulator,
                                  m_rotation_speed*GetFrameDT());
        else if (m_rotation_accumulator < 0)
            rotate_by_angle = Max(m_rotation_accumulator,
                                  -m_rotation_speed*GetFrameDT());
        else
            rotate_by_angle = 0.0;

        RotateView(rotate_by_angle);
        m_rotation_accumulator -= rotate_by_angle;
    }

    // handle view movement (from the arrow keys)
    if (GetParentWorldViewWidget()->GetIsFocused())
    {
        // movement basis vectors
        FloatVector2 origin(
            GetParallaxedWorldViewToWorld() * FloatVector2::ms_zero);
        FloatVector2 right(
            GetParallaxedWorldViewToWorld() * FloatVector2(1.0, 0.0) - origin);
        FloatVector2 up(
            GetParallaxedWorldViewToWorld() * FloatVector2(0.0, 1.0) - origin);

        Sint8 left_right_input =
            static_cast<Sint8>(Singletons::KeyBinds()->GetIsKeyPressed(Key::RIGHT)) -
            static_cast<Sint8>(Singletons::KeyBinds()->GetIsKeyPressed(Key::LEFT));
        Sint8 up_down_input =
            static_cast<Sint8>(Singletons::KeyBinds()->GetIsKeyPressed(Key::UP)) -
            static_cast<Sint8>(Singletons::KeyBinds()->GetIsKeyPressed(Key::DOWN));

        right *= (Float)left_right_input;
        up *= (Float)up_down_input;

        MoveView(GetFrameDT() * m_key_movement_speed_factor * (right + up));
    }
}

} // end of namespace Xrb

