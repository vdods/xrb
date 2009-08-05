// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_mainwidget.cpp by Victor Dods, created 2005/02/04
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_mapeditor2_mainwidget.hpp"

#include "xrb_binaryfileserializer.hpp"
#include "xrb_engine2_worldviewwidget.hpp"
#include "xrb_event.hpp" // TEMP
#include "xrb_filedialog.hpp"
#include "xrb_input.hpp" // temp
#include "xrb_label.hpp"
#include "xrb_layout.hpp"
#include "xrb_lineedit.hpp" // TEMP
#include "xrb_mapeditor2_entitypropertiespanel.hpp"
#include "xrb_mapeditor2_objectpropertiespanel.hpp"
#include "xrb_mapeditor2_world.hpp"
#include "xrb_mapeditor2_worldview.hpp"
#include "xrb_resourcelibrary.hpp"
#include "xrb_screen.hpp"
#include "xrb_spacerwidget.hpp"
#include "xrb_toolbarbutton.hpp"
#include "xrb_valueedit.hpp"
#include "xrb_valuelabel.hpp"
#include "xrb_widgetbackground.hpp" // TEMP

namespace Xrb
{

MapEditor2::MainWidget::MainWidget (
    ContainerWidget *const parent,
    std::string const &name)
    :
    Widget(parent, name),
    m_receiver_save_world_to_file(
        &MapEditor2::MainWidget::SaveWorldToFile, this),
    m_receiver_open_world_from_file(
        &MapEditor2::MainWidget::OpenWorldFromFile, this)
{
    // ///////////////////////////////////////////////////////////////////////
    // set up the widget layouts
    // ///////////////////////////////////////////////////////////////////////

    m_main_layout = new Layout(VERTICAL, this, "main layout");
    m_main_layout->SetIsUsingZeroedLayoutSpacingMargins(true);

    m_toolbar_layout =
        new Layout(
            HORIZONTAL,
            m_main_layout,
            "tool bar layout");

    m_main_panel_layout =
        new Layout(
            HORIZONTAL,
            m_main_layout,
            "main panel layout");
    m_main_panel_layout->SetIsUsingZeroedLayoutSpacingMargins(true);

    m_status_bar_layout =
        new Layout(
            HORIZONTAL,
            m_main_layout,
            "status bar layout");

    // ///////////////////////////////////////////////////////////////////////
    // set up the tool bar
    // ///////////////////////////////////////////////////////////////////////

    // transformation mode toolbar button group
    RadioButton<Object::TransformationMode,
                Object::TM_COUNT> *radio_button_tm;

    // global origin cursor
    radio_button_tm =
        new ToolbarButton<Object::TransformationMode,
                          Object::TM_COUNT>(
            Singletons::ResourceLibrary().
                LoadFilename<GLTexture>(
                    GLTexture::Create,
                    "resources/ui/black_checkmark.png"),
//                     "resources/global_origin.png"),
            Object::TM_GLOBAL_ORIGIN_CURSOR,
            &m_transformation_mode_group,
            m_toolbar_layout,
            "TM: global origin cursor");
    radio_button_tm->FixSizeRatios(FloatVector2(0.04f, 0.04f));

    // selected set local origin
    radio_button_tm =
        new ToolbarButton<Object::TransformationMode,
                          Object::TM_COUNT>(
            Singletons::ResourceLibrary().
                LoadFilename<GLTexture>(
                    GLTexture::Create,
                    "resources/ui/black_checkmark.png"),
//                     "resources/object_selection_set_origin.png"),
            Object::TM_SELECTION_SET_ORIGIN,
            &m_transformation_mode_group,
            m_toolbar_layout,
            "TM: object selection set origin");
    radio_button_tm->FixSizeRatios(FloatVector2(0.04f, 0.04f));

    // each selected object origin
    radio_button_tm =
        new ToolbarButton<Object::TransformationMode,
                          Object::TM_COUNT>(
            Singletons::ResourceLibrary().
                LoadFilename<GLTexture>(
                    GLTexture::Create,
                    "resources/ui/black_checkmark.png"),
//                     "resources/each_selected_object_origin.png"),
            Object::TM_EACH_SELECTED_OBJECT_ORIGIN,
            &m_transformation_mode_group,
            m_toolbar_layout,
            "TM: each selected object origin");
    radio_button_tm->FixSizeRatios(FloatVector2(0.04f, 0.04f));

    // transformation mode label
    m_transformation_mode_label =
        new Label(
            "",
            m_toolbar_layout,
            "transformation mode label");

    // edit mode toolbar button group
    RadioButton<Object::MetricMode,
                Object::MM_COUNT> *radio_button_mm;

    // transformation metric editing mode
    radio_button_mm =
        new ToolbarButton<Object::MetricMode,
                          Object::MM_COUNT>(
            Singletons::ResourceLibrary().
                LoadFilename<GLTexture>(
                    GLTexture::Create,
                    "resources/ui/black_checkmark.png"),
            Object::MM_TRANSFORMATION,
            &m_metric_editing_mode_group,
            m_toolbar_layout,
            "MM: transformation");
    radio_button_mm->FixSizeRatios(FloatVector2(0.04f, 0.04f));

    // linear velocity metric editing mode
    radio_button_mm =
        new ToolbarButton<Object::MetricMode,
                          Object::MM_COUNT>(
            Singletons::ResourceLibrary().
                LoadFilename<GLTexture>(
                    GLTexture::Create,
                    "resources/ui/black_checkmark.png"),
            Object::MM_LINEAR_VELOCITY,
            &m_metric_editing_mode_group,
            m_toolbar_layout,
            "MM: linear velocity");
    radio_button_mm->FixSizeRatios(FloatVector2(0.04f, 0.04f));

    // angular velocity metric editing mode
    radio_button_mm =
        new ToolbarButton<Object::MetricMode,
                          Object::MM_COUNT>(
            Singletons::ResourceLibrary().
                LoadFilename<GLTexture>(
                    GLTexture::Create,
                    "resources/ui/black_checkmark.png"),
            Object::MM_ANGULAR_VELOCITY,
            &m_metric_editing_mode_group,
            m_toolbar_layout,
            "MM: angular velocity");
    radio_button_mm->FixSizeRatios(FloatVector2(0.04f, 0.04f));

    // polygon metric editing mode
    radio_button_mm =
        new ToolbarButton<Object::MetricMode,
                          Object::MM_COUNT>(
            Singletons::ResourceLibrary().
                LoadFilename<GLTexture>(
                    GLTexture::Create,
                    "resources/ui/black_checkmark.png"),
            Object::MM_POLYGONS,
            &m_metric_editing_mode_group,
            m_toolbar_layout,
            "MM: polygons");
    radio_button_mm->FixSizeRatios(FloatVector2(0.04f, 0.04f));

    // vertices metric editing mode
    radio_button_mm =
        new ToolbarButton<Object::MetricMode,
                          Object::MM_COUNT>(
            Singletons::ResourceLibrary().
                LoadFilename<GLTexture>(
                    GLTexture::Create,
                    "resources/ui/black_checkmark.png"),
            Object::MM_VERTICES,
            &m_metric_editing_mode_group,
            m_toolbar_layout,
            "MM: vertices");
    radio_button_mm->FixSizeRatios(FloatVector2(0.04f, 0.04f));

    // metric editing mode label
    m_metric_editing_mode_label =
        new Label(
            "",
            m_toolbar_layout,
            "metric editing mode label");

    new SpacerWidget(m_toolbar_layout);

    // ///////////////////////////////////////////////////////////////////////
    // set up the main panel
    // ///////////////////////////////////////////////////////////////////////

    m_world_view_widget = new Engine2::WorldViewWidget(NULL, m_main_panel_layout);
    m_world_view_widget->Focus();

    m_control_panel_layout =
        new Layout(
            VERTICAL,
            m_main_panel_layout,
            "Control panel layout");
    m_control_panel_layout->SetIsUsingZeroedLayoutSpacingMargins(true);
    m_control_panel_layout->SetSizePropertyRatio(SizeProperties::MAX, Dim::X, 0.45f);
    m_control_panel_layout->SetSizePropertyEnabled(SizeProperties::MAX, Dim::X, true);

    // create the object properties panel
    m_object_properties_panel = new ObjectPropertiesPanel(m_control_panel_layout);
    // hide the object properties panel (because there is not even an
    // object layer to begin with)
    m_object_properties_panel->SetIsHidden(true);
    // create the entity properties panel
    m_entity_properties_panel = new EntityPropertiesPanel(m_control_panel_layout);
    // hide the entity properties panel (because there is not even an
    // object layer to begin with)
    m_entity_properties_panel->SetIsHidden(true);
    // spacer for the rest of the layout
    new SpacerWidget(m_control_panel_layout);

    // ///////////////////////////////////////////////////////////////////////
    // set up the status bar
    // ///////////////////////////////////////////////////////////////////////

    m_message_label = new Label("", m_status_bar_layout, "message label");
    m_message_label->SetIsHeightFixedToTextHeight(true);

    Label *l;

    l = new Label("Grid:", m_status_bar_layout, "grid scale text label");
    l->SetIsWidthFixedToTextWidth(true);

    m_grid_scale_label =
        new Label("", m_status_bar_layout, "grid scale value label");
    m_grid_scale_label->SetIsWidthFixedToTextWidth(true);

    l = new Label("N-Gon:", m_status_bar_layout, "n-gon vertices text label");
    l->SetIsWidthFixedToTextWidth(true);

    m_polygon_tesselation_label =
        new Label("", m_status_bar_layout, "polygon tesselation value label");
    m_polygon_tesselation_label->SetIsWidthFixedToTextWidth(true);

    // ///////////////////////////////////////////////////////////////////////
    // make the main layout be the main widget
    // ///////////////////////////////////////////////////////////////////////

    SetMainWidget(m_main_layout);

    // temp
    WorldView *world_view = new WorldView(GetWorldViewWidget());
    World *world = World::CreateEmpty();
    world->AttachWorldView(world_view);
    SetMapEditorWorldView(world_view);

    m_world_view_widget->Focus();
}

MapEditor2::MainWidget::~MainWidget ()
{
    m_world_view_widget->SetWorldView(NULL);
}

MapEditor2::WorldView *MapEditor2::MainWidget::GetMapEditorWorldView () const
{
    return DStaticCast<WorldView *>(m_world_view_widget->GetWorldView());
}

void MapEditor2::MainWidget::SaveWorldToFile (std::string const &filename)
{
    ASSERT1(!filename.empty());

    // early out if there is no view (and consequently no world)
    if (!GetMapEditorWorldView())
        return;

    BinaryFileSerializer serializer;
    serializer.Open(filename.c_str(), "wb");
    GetMapEditorWorldView()->GetWorld()->Write(serializer);
    serializer.Close();
}

void MapEditor2::MainWidget::OpenWorldFromFile (std::string const &filename)
{
    ASSERT1(!filename.empty());

    BinaryFileSerializer serializer;
    serializer.Open(filename.c_str(), "rb");
    World *map_editor_world = World::Create(serializer);
    serializer.Close();

    WorldView *world_view = new WorldView(GetWorldViewWidget());
    map_editor_world->AttachWorldView(world_view);

    SetMapEditorWorldView(world_view);
}

void MapEditor2::MainWidget::SetMapEditorWorldView (MapEditor2::WorldView *const map_editor_world_view)
{
    GetWorldViewWidget()->SetWorldView(map_editor_world_view);

    if (GetMapEditorWorldView() != NULL)
    {
        m_toolbar_layout->Enable();
        m_status_bar_layout->Enable();

        // initialize the toolbar button groups for transformation
        // and metric editing modes
        m_transformation_mode_group.SetID(
            GetMapEditorWorldView()->GetTransformationMode());
        m_metric_editing_mode_group.SetID(
            GetMapEditorWorldView()->GetMetricEditingMode());

        // hook up the signals

        // WorldView message text label
        m_message_label->SetText("");
        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderStatusBarMessageTextChanged(),
            m_message_label->ReceiverSetText());

        // grid scale label
        m_grid_scale_label->SetText(GetMapEditorWorldView()->CurrentGridScaleText());
        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderGridScaleTextChanged(),
            m_grid_scale_label->ReceiverSetText());
        // polygon tesselation label
        m_polygon_tesselation_label->SetText(GetMapEditorWorldView()->GetPolygonTesselationText());
        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderPolygonTesselationTextChanged(),
            m_polygon_tesselation_label->ReceiverSetText());

        // transformation mode toolbar buttons and text display - init
        m_transformation_mode_label->SetText(
            GetMapEditorWorldView()->GetTransformationModeText());
        // signal connections
        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderTransformationModeChanged(),
            m_transformation_mode_group.ReceiverSetID());
        SignalHandler::Connect1(
            m_transformation_mode_group.SenderIDChanged(),
            GetMapEditorWorldView()->ReceiverSetTransformationMode());
        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderTransformationModeTextChanged(),
            m_transformation_mode_label->ReceiverSetText());

        // metric editing mode toolbar buttons and text display - init
        m_metric_editing_mode_label->SetText(
            GetMapEditorWorldView()->GetMetricEditingModeText());
        // signal connections
        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderMetricEditingModeChanged(),
            m_metric_editing_mode_group.ReceiverSetID());
        SignalHandler::Connect1(
            m_metric_editing_mode_group.SenderIDChanged(),
            GetMapEditorWorldView()->ReceiverSetMetricEditingMode());
        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderMetricEditingModeTextChanged(),
            m_metric_editing_mode_label->ReceiverSetText());

        // object properties panel hiding - init
        m_object_properties_panel->SetIsHidden(
            GetMapEditorWorldView()->AreNoObjectsSelected());
        // signal connections
        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderNoObjectsAreSelected(),
            m_object_properties_panel->ReceiverSetIsHidden());

        // object properties panel values - init
        m_object_properties_panel->SetObjectSelectionSetOrigin(
            GetMapEditorWorldView()->GetObjectSelectionSetOrigin());
        m_object_properties_panel->SetObjectSelectionSetScale(
            GetMapEditorWorldView()->GetObjectSelectionSetScale());
        m_object_properties_panel->SetObjectSelectionSetAngle(
            GetMapEditorWorldView()->GetObjectSelectionSetAngle());
        // signal connections
        SignalHandler::Connect1(
            m_object_properties_panel->SenderPerObjectOriginXAssigned(),
            GetMapEditorWorldView()->ReceiverSetPerObjectOriginX());
        SignalHandler::Connect1(
            m_object_properties_panel->SenderObjectSelectionSetOriginXChanged(),
            GetMapEditorWorldView()->ReceiverSetObjectSelectionSetOriginX());

        SignalHandler::Connect1(
            m_object_properties_panel->SenderPerObjectOriginYAssigned(),
            GetMapEditorWorldView()->ReceiverSetPerObjectOriginY());
        SignalHandler::Connect1(
            m_object_properties_panel->SenderObjectSelectionSetOriginYChanged(),
            GetMapEditorWorldView()->ReceiverSetObjectSelectionSetOriginY());

        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderObjectSelectionSetOriginChanged(),
            m_object_properties_panel->ReceiverSetObjectSelectionSetOrigin());

        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderObjectSelectionSetScaleChanged(),
            m_object_properties_panel->ReceiverSetObjectSelectionSetScale());
        SignalHandler::Connect1(
            m_object_properties_panel->SenderPerObjectScaleAssigned(),
            GetMapEditorWorldView()->ReceiverSetPerObjectScale());
        SignalHandler::Connect1(
            m_object_properties_panel->SenderObjectSelectionSetScaleChanged(),
            GetMapEditorWorldView()->ReceiverSetObjectSelectionSetScale());

        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderObjectSelectionSetAngleChanged(),
            m_object_properties_panel->ReceiverSetObjectSelectionSetAngle());
        SignalHandler::Connect1(
            m_object_properties_panel->SenderPerObjectAngleAssigned(),
            GetMapEditorWorldView()->ReceiverSetPerObjectAngle());
        SignalHandler::Connect1(
            m_object_properties_panel->SenderObjectSelectionSetAngleChanged(),
            GetMapEditorWorldView()->ReceiverSetObjectSelectionSetAngle());

        // entity properties panel hiding - init
        m_entity_properties_panel->SetIsHidden(
            GetMapEditorWorldView()->AreNoEntitiesSelected());
        // signal connections
        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderNoEntitiesAreSelected(),
            m_entity_properties_panel->ReceiverSetIsHidden());
        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderSelectedEntityCountChanged(),
            m_entity_properties_panel->ReceiverSetSelectedEntityCount());

        // entity properties panel value - init
        m_entity_properties_panel->SetObjectSelectionSetFirstMoment(
            GetMapEditorWorldView()->GetObjectSelectionSetFirstMoment());
        // signal connections
        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderObjectSelectionSetFirstMomentChanged(),
            m_entity_properties_panel->ReceiverSetObjectSelectionSetFirstMoment());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderObjectSelectionSetFirstMomentChanged(),
            GetMapEditorWorldView()->ReceiverSetObjectSelectionSetFirstMoment());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderPerEntityFirstMomentAssigned(),
            GetMapEditorWorldView()->ReceiverSetPerEntityFirstMoment());

        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderObjectSelectionSetVelocityChanged(),
            m_entity_properties_panel->ReceiverSetObjectSelectionSetVelocity());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderObjectSelectionSetVelocityXChanged(),
            GetMapEditorWorldView()->ReceiverSetObjectSelectionSetVelocityX());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderPerEntityVelocityXAssigned(),
            GetMapEditorWorldView()->ReceiverSetPerEntityVelocityX());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderObjectSelectionSetVelocityYChanged(),
            GetMapEditorWorldView()->ReceiverSetObjectSelectionSetVelocityY());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderPerEntityVelocityYAssigned(),
            GetMapEditorWorldView()->ReceiverSetPerEntityVelocityY());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderObjectSelectionSetSpeedChanged(),
            GetMapEditorWorldView()->ReceiverSetObjectSelectionSetSpeed());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderPerEntitySpeedAssigned(),
            GetMapEditorWorldView()->ReceiverSetPerEntitySpeed());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderObjectSelectionSetVelocityAngleChanged(),
            GetMapEditorWorldView()->ReceiverSetObjectSelectionSetVelocityAngle());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderPerEntityVelocityAngleAssigned(),
            GetMapEditorWorldView()->ReceiverSetPerEntityVelocityAngle());

        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderObjectSelectionSetSecondMomentChanged(),
            m_entity_properties_panel->ReceiverSetObjectSelectionSetSecondMoment());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderObjectSelectionSetSecondMomentChanged(),
            GetMapEditorWorldView()->ReceiverSetObjectSelectionSetSecondMoment());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderPerEntitySecondMomentAssigned(),
            GetMapEditorWorldView()->ReceiverSetPerEntitySecondMoment());

        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderObjectSelectionSetAngularVelocityChanged(),
            m_entity_properties_panel->ReceiverSetObjectSelectionSetAngularVelocity());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderObjectSelectionSetAngularVelocityChanged(),
            GetMapEditorWorldView()->ReceiverSetObjectSelectionSetAngularVelocity());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderPerEntityAngularVelocityAssigned(),
            GetMapEditorWorldView()->ReceiverSetPerEntityAngularVelocity());

        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderObjectSelectionSetElasticityChanged(),
            m_entity_properties_panel->ReceiverSetObjectSelectionSetElasticity());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderObjectSelectionSetElasticityChanged(),
            GetMapEditorWorldView()->ReceiverSetObjectSelectionSetElasticity());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderPerEntityElasticityAssigned(),
            GetMapEditorWorldView()->ReceiverSetPerEntityElasticity());

        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderObjectSelectionSetDensityChanged(),
            m_entity_properties_panel->ReceiverSetObjectSelectionSetDensity());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderObjectSelectionSetDensityChanged(),
            GetMapEditorWorldView()->ReceiverSetObjectSelectionSetDensity());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderPerEntityDensityAssigned(),
            GetMapEditorWorldView()->ReceiverSetPerEntityDensity());

        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderPerEntityAppliesGravityAssigned(),
            m_entity_properties_panel->ReceiverSetPerEntityAppliesGravity());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderPerEntityAppliesGravityAssigned(),
            GetMapEditorWorldView()->ReceiverSetPerEntityAppliesGravity());

        SignalHandler::Connect1(
            GetMapEditorWorldView()->SenderPerEntityReactsToGravityAssigned(),
            m_entity_properties_panel->ReceiverSetPerEntityReactsToGravity());
        SignalHandler::Connect1(
            m_entity_properties_panel->SenderPerEntityReactsToGravityAssigned(),
            GetMapEditorWorldView()->ReceiverSetPerEntityReactsToGravity());
    }
    else
    {
        m_toolbar_layout->Disable();
        m_status_bar_layout->Disable();
//         m_edit_delta_label->Disable();
//         m_major_grid_unit_size_text_label->Disable();
//         m_major_grid_unit_size_value_label->Disable();
//         m_minor_grid_unit_size_text_label->Disable();
//         m_minor_grid_unit_size_value_label->Disable();
    }
}

// temp (put this into the menu commands later)
bool MapEditor2::MainWidget::ProcessKeyEvent (EventKey const *const e)
{
    if (e->IsKeyDownEvent())
    {
        /*
        if (e->GetKeyCode() == Key::F1)
        {
            m_control_panel_layout->ToggleIsHidden();
            return true;
        }
        */

        if (e->GetKeyCode() == Key::N && e->IsEitherControlKeyPressed())
        {
            fprintf(stderr, "MapEditor2::MainWidget::ProcessKeyEvent(); add in file-saving checking stuff\n");

            WorldView *world_view = new WorldView(GetWorldViewWidget());
            World *world = World::CreateEmpty();
            world->AttachWorldView(world_view);

            SetMapEditorWorldView(world_view);
            return true;
        }

        if (e->GetKeyCode() == Key::O && e->IsEitherControlKeyPressed())
        {
            fprintf(stderr, "MapEditor2::MainWidget::ProcessKeyEvent(); add in file-saving checking stuff\n");

            FileDialog *dialog =
                new FileDialog(
                    "Open world from file",
                    FilePanel::OP_OPEN,
                    this);
            dialog->ResizeByRatios(FloatVector2(0.8f, 0.8f));
            dialog->CenterOnWidget(dialog->GetTopLevelParent());
            SignalHandler::Connect1(
                dialog->SenderSubmitFilename(),
                &m_receiver_open_world_from_file);
            return true;
        }

        if (e->GetKeyCode() == Key::S && e->IsEitherControlKeyPressed())
        {
            // early out if there is no view (and consequently no world)
            if (!GetMapEditorWorldView())
                return false;

            FileDialog *dialog =
                new FileDialog(
                    "Save world to file",
                    FilePanel::OP_SAVE,
                    this);
            dialog->ResizeByRatios(FloatVector2(0.8f, 0.8f));
            dialog->CenterOnWidget(dialog->GetTopLevelParent());
            SignalHandler::Connect1(
                dialog->SenderSubmitFilename(),
                &m_receiver_save_world_to_file);
            return true;
        }

        if (e->GetKeyCode() == Key::W && e->IsEitherControlKeyPressed())
        {
            fprintf(stderr, "MapEditor2::MainWidget::ProcessKeyEvent(); add in file-saving checking stuff\n");

            SetMapEditorWorldView(NULL);
            return true;
        }
    }

    return false;
}

} // end of namespace Xrb
