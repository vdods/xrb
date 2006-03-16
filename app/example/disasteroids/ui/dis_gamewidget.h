// ///////////////////////////////////////////////////////////////////////////
// dis_gamewidget.h by Victor Dods, created 2005/12/17
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_GAMEWIDGET_H_)
#define _DIS_GAMEWIDGET_H_

#include "xrb_widgetstack.h"

#include "dis_enums.h"

using namespace Xrb;

namespace Xrb
{

class Label;
class Layout;
class SpacerWidget;
template <typename T> class ValueLabel;

namespace Engine2
{
    class World;
    class WorldViewWidget;
} // end of namespace Engine2

} // end of namespace Xrb

namespace Dis
{

class InventoryPanel;
class PlayerShip;
class ProgressBar;
class WorldView;

class GameWidget : public WidgetStack
{
public:

    GameWidget (Engine2::World *world, Widget *parent);
    virtual ~GameWidget () { }

    SignalSender0 const *SenderEndGame ();
    SignalSender0 const *SenderQuitGame ();
    
    inline WorldView *GetWorldView () { return m_world_view; }

    void SetPlayerShip (PlayerShip *player_ship);
    
    void SetWorldFrameTime (Uint32 world_frame_time);
    void SetGUIFrameTime (Uint32 gui_frame_time);
    void SetRenderFrameTime (Uint32 render_frame_time);
    void SetFramerate (Float framerate);

    void SetMineralInventory (Uint8 mineral_type, Float mineral_inventory);

    void ActivateInventoryPanel ();
    void DeactivateInventoryPanel ();
    
private:

    void SetTimeAlive (Float time_alive);

    void UpdateTimeAliveLabel ();

    WorldView *m_world_view;
    Engine2::WorldViewWidget *m_world_view_widget;

    Float m_saved_game_timescale;
    InventoryPanel *m_inventory_panel;
    
    // debug info layout and value labels
    Layout *m_debug_info_layout;
    ValueLabel<Uint32> *m_world_frame_time_label;
    ValueLabel<Uint32> *m_gui_frame_time_label;
    ValueLabel<Uint32> *m_render_frame_time_label;
    ValueLabel<Float> *m_framerate_label;

    ValueLabel<Uint32> *m_mineral_inventory_label[MINERAL_COUNT];

    Label *m_time_alive_label;
    Float m_time_alive;
    ValueLabel<Uint32> *m_score_label;
    ProgressBar *m_stoke_o_meter;

    SpacerWidget *m_view_spacer_widget;
    
    ProgressBar *m_armor_status;
    ProgressBar *m_shield_status;
    ProgressBar *m_power_status;
    ProgressBar *m_weapon_status;

    SignalReceiver1<PlayerShip *> m_receiver_set_player_ship;
    SignalReceiver1<Float> m_internal_receiver_set_time_alive;
    
    SignalReceiver0 m_receiver_activate_inventory_panel;
    SignalReceiver0 m_receiver_deactivate_inventory_panel;

    SignalReceiver2<Uint8, Float> m_receiver_set_mineral_inventory;
}; // end of class GameWidget

} // end of namespace Dis

#endif // !defined(_DIS_GAMEWIDGET_H_)

