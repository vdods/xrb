// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_mainwidget.hpp by Victor Dods, created 2005/02/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MAPEDITOR2_MAINWIDGET_HPP_)
#define _XRB_MAPEDITOR2_MAINWIDGET_HPP_

#include "xrb.hpp"

#include "xrb_mapeditor2_object.hpp"
#include "xrb_toolbarbutton.hpp"
#include "xrb_widget.hpp"

namespace Xrb
{

class Label;
class Layout;
template <typename ValueType> class ValueLabel;

namespace Engine2
{
    class WorldViewWidget;
} // end of namespace Engine2

namespace MapEditor2
{

    class EntityPropertiesPanel;
    class ObjectPropertiesPanel;
    class WorldView;
    
    class MainWidget : public Widget
    {
    public:
    
        MainWidget (
            ContainerWidget *parent,
            std::string const &name = "MapEditor2::MainWidget");
        virtual ~MainWidget ();
    
        inline Engine2::WorldViewWidget *GetWorldViewWidget () const
        {
            return m_world_view_widget;
        }
        WorldView *MapEditorWorldView () const;
    
        inline SignalReceiver1<std::string const &> const *ReceiverSaveWorldToFile ()
        {
            return &m_receiver_save_world_to_file;
        }
        inline SignalReceiver1<std::string const &> const *ReceiverOpenWorldFromFile ()
        {
            return &m_receiver_open_world_from_file;
        }
    
        void SaveWorldToFile (std::string const &path);
        void OpenWorldFromFile (std::string const &path);
    
    protected:
    
        void SetMapEditorWorldView (WorldView *map_editor_world_view);
    
        bool ProcessKeyEvent (EventKey const *e); // temp
    
    private:
    
        // this layout contains the upper and lower status bars and the main panel
        Layout *m_main_layout;
    
        // this is the toolbar layout
        Layout *m_toolbar_layout;
        // button group for the transformation mode toolbar buttons
        RadioButtonGroup<
            Object::TransformationMode,
            Object::TM_COUNT> m_transformation_mode_group;
        // label to print the text of the current transformation mode
        Label *m_transformation_mode_label;
        // button group for the metric mode toolbar buttons
        RadioButtonGroup<
            Object::MetricMode,
            Object::MM_COUNT> m_metric_editing_mode_group;
        // label to print the text of the current metric editing mode
        Label *m_metric_editing_mode_label;
    
        // this layout is the main panel--a ViewWidget and a control
        // panel which is for articulation of properties of map objects
        Layout *m_main_panel_layout;
        // the ViewWidget instance in this layout
        Engine2::WorldViewWidget *m_world_view_widget;
        // the control panel layout (object/entity properties, etc)
        Layout *m_control_panel_layout;
        // the object properties panel
        ObjectPropertiesPanel *m_object_properties_panel;
        // the entity properties panel
        EntityPropertiesPanel *m_entity_properties_panel;
    
        // this is the status bar layout
        Layout *m_status_bar_layout;
        // label to display messages such as edit deltas, and mode indicators.
        Label *m_message_label;
        // label to display the grid scale
        Label *m_grid_scale_label;
        // label to display the polygon tesselation
        Label *m_polygon_tesselation_label;
    
        SignalReceiver1<std::string const &> m_receiver_save_world_to_file;
        SignalReceiver1<std::string const &> m_receiver_open_world_from_file;
    }; // end of class MapEditor2::MainWidget

} // end of namespace MapEditor2

} // end of namespace Xrb

#endif // !defined(_XRB_MAPEDITOR2_MAINWIDGET_HPP_)

