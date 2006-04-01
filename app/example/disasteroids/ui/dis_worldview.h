// ///////////////////////////////////////////////////////////////////////////
// dis_worldview.h by Victor Dods, created 2005/11/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_WORLDVIEW_H_)
#define _DIS_WORLDVIEW_H_

#include "xrb_engine2_worldview.h"
#include "xrb_signalhandler.h"

using namespace Xrb;

namespace Dis
{

class InventoryPanel;
class PlayerShip;

class WorldView : public Engine2::WorldView, public SignalHandler
{
public:

    WorldView (Engine2::WorldViewWidget *parent_world_view_widget);
    virtual ~WorldView ();

    inline SignalSender1<PlayerShip *> const *SenderPlayerShipChanged () { return &m_sender_player_ship_changed; }
    inline SignalSender1<bool> const *SenderIsDebugInfoEnabledChanged () { return &m_sender_is_debug_info_enabled_changed; }
    inline SignalSender0 const *SenderActivateInventoryPanel () { return &m_sender_activate_inventory_panel; }
    inline SignalSender0 const *SenderDeactivateInventoryPanel () { return &m_sender_deactivate_inventory_panel; }
    inline SignalSender0 const *SenderShowGameOverLabel () { return &m_sender_show_game_over_label; }
    inline SignalSender0 const *SenderHideGameOverLabel () { return &m_sender_hide_game_over_label; }
    inline SignalSender0 const *SenderEndGame () { return &m_sender_end_game; }

    inline SignalReceiver0 const *ReceiverEnableInventoryPanel () { return &m_receiver_enable_inventory_panel; }
    inline SignalReceiver0 const *ReceiverDisableInventoryPanel () { return &m_receiver_disable_inventory_panel; }
    inline SignalReceiver0 const *ReceiverShowGameOverLabel () { return &m_receiver_show_game_over_label; }
    inline SignalReceiver0 const *ReceiverHideGameOverLabel () { return &m_receiver_hide_game_over_label; }
    inline SignalReceiver0 const *ReceiverEndGame () { return &m_receiver_end_game; }

    inline PlayerShip *GetPlayerShip () { return m_player_ship; }
    inline bool GetIsDebugInfoEnabled () const { return m_is_debug_info_enabled; }
    
    void SetPlayerShip (PlayerShip *player_ship);
    void SetIsDebugInfoEnabled (bool is_debug_info_enabled);

    virtual bool ProcessKeyEvent (EventKey const *e);
    virtual bool ProcessMouseButtonEvent (EventMouseButton const *e);
    virtual bool ProcessMouseWheelEvent (EventMouseWheel const *e);
    virtual bool ProcessMouseMotionEvent (EventMouseMotion const *e);
    
protected:

    virtual void ProcessFrameOverride ();

private:

    void EnableInventoryPanel ();
    void DisableInventoryPanel ();    
    void ShowGameOverLabel ();
    void HideGameOverLabel ();
    void EndGame ();

    // ///////////////////////////////////////////////////////////////////////
    // the player's ship
    
    PlayerShip *m_player_ship;

    // ///////////////////////////////////////////////////////////////////////
    // view movement vars

    // speed coefficient for using the arrow keys to move the view
    Float m_key_movement_speed_factor;
    // used to control how closely the view follows the ship
    Float m_dragging_factor;
    // the view center velocity (used to track the ship smoothly)
    FloatVector2 m_view_velocity;
    // used in interpolating the view center during recovery
    FloatVector2 m_calculated_view_center;
    // indicates if the view positioning is recovering from the
    // ship accelerating suddenly (e.g. hitting an asteroid)
    bool m_is_view_recovering;
    // indicates the current recover parameter in the range of [0, 1]
    // (used for interpolation from bad view position to recovered
    // view position)
    Float m_recover_parameter;

    // ///////////////////////////////////////////////////////////////////////
    // view zooming vars

    // accumulates weighted zoom in/out requests.  zooming in increases
    // this value by m_zoom_increment, zooming out decreases it by the same.
    Float m_zoom_accumulator;
    // the quanta for m_zoom_accumulator.
    Float m_zoom_increment;
    // the rate at which to zoom.
    Float m_zoom_speed;

    // ///////////////////////////////////////////////////////////////////////
    // view rotation vars

    // accumulates weighted rotate requests.  rotating counterclockwise
    // increases this value by m_rotation_increment, while rotating clockwise
    // decreases it by the same.
    Float m_rotation_accumulator;
    // the quanta for m_rotation_accumulator.
    Float m_rotation_increment;
    // the rate at which to rotate.
    Float m_rotation_speed;

    // ///////////////////////////////////////////////////////////////////////
    // other shit

    bool m_use_dvorak;
    bool m_is_debug_info_enabled;
    bool m_disable_inventory_panel;
    
    // ///////////////////////////////////////////////////////////////////////
    // SignalSenders

    SignalSender1<PlayerShip *> m_sender_player_ship_changed;
    SignalSender1<bool> m_sender_is_debug_info_enabled_changed;
    SignalSender0 m_sender_activate_inventory_panel;
    SignalSender0 m_sender_deactivate_inventory_panel;
    SignalSender0 m_sender_show_game_over_label;
    SignalSender0 m_sender_hide_game_over_label;
    SignalSender0 m_sender_end_game;

    // ///////////////////////////////////////////////////////////////////////
    // SignalReceivers

    SignalReceiver0 m_receiver_enable_inventory_panel;
    SignalReceiver0 m_receiver_disable_inventory_panel;
    SignalReceiver0 m_receiver_show_game_over_label;
    SignalReceiver0 m_receiver_hide_game_over_label;
    SignalReceiver0 m_receiver_end_game;
}; // end of class WorldView

} // end of namespace Dis

#endif // !defined(_DIS_WORLDVIEW_H_)
