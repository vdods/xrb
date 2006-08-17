// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_entitypropertiespanel.cpp by Victor Dods, created 2005/03/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_mapeditor2_entitypropertiespanel.h"

#include "xrb_button.h"
#include "xrb_cellpaddingwidget.h"
#include "xrb_checkbox.h"
#include "xrb_label.h"
#include "xrb_layout.h"
#include "xrb_lineedit.h"
#include "xrb_math.h"
#include "xrb_spacerwidget.h"
#include "xrb_valueedit.h"
// #include "xrb_widgetbackground.h" // temp

namespace Xrb
{

MapEditor2::EntityPropertiesPanel::EntityPropertiesPanel (
    ContainerWidget *const parent,
    std::string const &name)
    :
    Widget(parent, name),
    m_sender_per_entity_first_moment_assigned(this),
    m_sender_per_entity_velocity_x_assigned(this),
    m_sender_per_entity_velocity_y_assigned(this),
    m_sender_per_entity_speed_assigned(this),
    m_sender_per_entity_velocity_angle_assigned(this),
    m_sender_per_entity_second_moment_assigned(this),
    m_sender_per_entity_angular_velocity_assigned(this),
    m_sender_per_entity_elasticity_assigned(this),
    m_sender_per_entity_density_assigned(this),
    m_sender_per_entity_applies_gravity_assigned(this),
    m_sender_per_entity_reacts_to_gravity_assigned(this),

    m_sender_object_selection_set_first_moment_changed(this),
    m_sender_object_selection_set_velocity_x_changed(this),
    m_sender_object_selection_set_velocity_y_changed(this),
    m_sender_object_selection_set_speed_changed(this),
    m_sender_object_selection_set_velocity_angle_changed(this),
    m_sender_object_selection_set_second_moment_changed(this),
    m_sender_object_selection_set_angular_velocity_changed(this),
    m_sender_object_selection_set_elasticity_changed(this),
    m_sender_object_selection_set_density_changed(this),

    m_receiver_set_selected_entity_count(
        &EntityPropertiesPanel::SetSelectedEntityCount, this),

    m_receiver_set_per_entity_applies_gravity(
        &EntityPropertiesPanel::SetPerEntityAppliesGravity, this),
    m_receiver_set_per_entity_reacts_to_gravity(
        &EntityPropertiesPanel::SetPerEntityReactsToGravity, this),

    m_receiver_set_object_selection_set_first_moment(
        &EntityPropertiesPanel::SetObjectSelectionSetFirstMoment, this),
    m_receiver_set_object_selection_set_velocity(
        &EntityPropertiesPanel::SetObjectSelectionSetVelocity, this),
    m_receiver_set_object_selection_set_second_moment(
        &EntityPropertiesPanel::SetObjectSelectionSetSecondMoment, this),
    m_receiver_set_object_selection_set_angular_velocity(
        &EntityPropertiesPanel::SetObjectSelectionSetAngularVelocity, this),
    m_receiver_set_object_selection_set_elasticity(
        &EntityPropertiesPanel::SetObjectSelectionSetElasticity, this),
    m_receiver_set_object_selection_set_density(
        &EntityPropertiesPanel::SetObjectSelectionSetDensity, this),

    m_internal_receiver_set_per_entity_first_moment(
        &EntityPropertiesPanel::InternalSetPerEntityFirstMoment, this),
    m_internal_receiver_set_per_entity_velocity_x(
        &EntityPropertiesPanel::InternalSetPerEntityVelocityX, this),
    m_internal_receiver_set_per_entity_velocity_y(
        &EntityPropertiesPanel::InternalSetPerEntityVelocityY, this),
    m_internal_receiver_set_per_entity_speed(
        &EntityPropertiesPanel::InternalSetPerEntitySpeed, this),
    m_internal_receiver_set_per_entity_velocity_angle(
        &EntityPropertiesPanel::InternalSetPerEntityVelocityAngle, this),
    m_internal_receiver_set_per_entity_second_moment(
        &EntityPropertiesPanel::InternalSetPerEntitySecondMoment, this),
    m_internal_receiver_set_per_entity_angular_velocity(
        &EntityPropertiesPanel::InternalSetPerEntityAngularVelocity, this),
    m_internal_receiver_set_per_entity_elasticity(
        &EntityPropertiesPanel::InternalSetPerEntityElasticity, this),
    m_internal_receiver_set_per_entity_density(
        &EntityPropertiesPanel::InternalSetPerEntityDensity, this),
    m_internal_receiver_set_per_entity_applies_gravity(
        &EntityPropertiesPanel::InternalSetPerEntityAppliesGravity, this),
    m_internal_receiver_set_per_entity_reacts_to_gravity(
        &EntityPropertiesPanel::InternalSetPerEntityReactsToGravity, this),

    m_internal_receiver_set_object_selection_set_first_moment(
        &EntityPropertiesPanel::InternalSetObjectSelectionSetFirstMoment, this),
    m_internal_receiver_set_object_selection_set_velocity_x(
        &EntityPropertiesPanel::InternalSetObjectSelectionSetVelocityX, this),
    m_internal_receiver_set_object_selection_set_velocity_y(
        &EntityPropertiesPanel::InternalSetObjectSelectionSetVelocityY, this),
    m_internal_receiver_set_object_selection_set_speed(
        &EntityPropertiesPanel::InternalSetObjectSelectionSetSpeed, this),
    m_internal_receiver_set_object_selection_set_velocity_angle(
        &EntityPropertiesPanel::InternalSetObjectSelectionSetVelocityAngle, this),
    m_internal_receiver_set_object_selection_set_second_moment(
        &EntityPropertiesPanel::InternalSetObjectSelectionSetSecondMoment, this),
    m_internal_receiver_set_object_selection_set_angular_velocity(
        &EntityPropertiesPanel::InternalSetObjectSelectionSetAngularVelocity, this),
    m_internal_receiver_set_object_selection_set_elasticity(
        &EntityPropertiesPanel::InternalSetObjectSelectionSetElasticity, this),
    m_internal_receiver_set_object_selection_set_density(
        &EntityPropertiesPanel::InternalSetObjectSelectionSetDensity, this),
    m_internal_receiver_object_selection_set_applies_gravity_on(
        &EntityPropertiesPanel::InternalObjectSelectionSetAppliesGravityOn, this),
    m_internal_receiver_object_selection_set_applies_gravity_off(
        &EntityPropertiesPanel::InternalObjectSelectionSetAppliesGravityOff, this),
    m_internal_receiver_object_selection_set_reacts_to_gravity_on(
        &EntityPropertiesPanel::InternalObjectSelectionSetReactsToGravityOn, this),
    m_internal_receiver_object_selection_set_reacts_to_gravity_off(
        &EntityPropertiesPanel::InternalObjectSelectionSetReactsToGravityOff, this),

    m_validator_greater_than_zero(0.0f, 1.0f/65536.0f),
    m_validator_elasticity(0.0f, 1.0f)
{
    Label *l;
    LineEdit *le;
    ValueEdit<Float> *vef;
    Button *button;
    CheckBox *cb;
    CellPaddingWidget *cpw;

    Layout *main_layout = new Layout(VERTICAL, this, "main layout");
    main_layout->SetIsUsingZeroedFrameMargins(true);
    main_layout->SetIsUsingZeroedLayoutSpacingMargins(true);

    l = new Label("Entity Properties", main_layout, "title label");
    l->SetIsHeightFixedToTextHeight(true);

    // set up this widget's layout
    Layout *control_layout = new Layout(ROW, 3, main_layout, "control layout");

    // create the controls in the layout

    //////////////////////////////////////////////////////////////////////////
    // top row of labels
    new SpacerWidget(control_layout);

    l = new Label("Per-Entity", control_layout, "per-entity column header");
    l->SetIsHeightFixedToTextHeight(true);

    l = new Label("Selection Set", control_layout, "object selection set column header");
    l->SetIsHeightFixedToTextHeight(true);

    // the actual controls

    //////////////////////////////////////////////////////////////////////////
    // first moment row
    new Label("First Moment", control_layout, "first moment label");

    le = m_per_entity_first_moment_edit =
        new LineEdit(20, control_layout, "per-entity first moment edit");
    le->GetCharacterFilter().SetFilterType(DECIMAL_NOTATION_CHARACTER_FILTER_TYPE);
    le->GetCharacterFilter().SetFilter(DECIMAL_NOTATION_CHARACTER_FILTER);

    vef = m_object_selection_set_first_moment_edit =
        new ValueEdit<Float>(
            DECIMAL_NOTATION_PRINTF_FORMAT,
            Util::TextToFloat,
            control_layout,
            "object selection set first moment edit");
    vef->SetValidator(&m_validator_greater_than_zero);
    vef->SetValue(0.0);

    //////////////////////////////////////////////////////////////////////////
    // velocity x row
    new Label("Velocity X", control_layout, "first moment label");

    le = m_per_entity_velocity_x_edit =
        new LineEdit(20, control_layout, "per-entity velocity x edit");
    le->GetCharacterFilter().SetFilterType(DECIMAL_NOTATION_CHARACTER_FILTER_TYPE);
    le->GetCharacterFilter().SetFilter(DECIMAL_NOTATION_CHARACTER_FILTER);

    vef = m_object_selection_set_velocity_x_edit =
        new ValueEdit<Float>(
            DECIMAL_NOTATION_PRINTF_FORMAT,
            Util::TextToFloat,
            control_layout,
            "object selection set velocity x edit");
    vef->SetValue(0.0);

    //////////////////////////////////////////////////////////////////////////
    // velocity y row
    new Label("Velocity Y", control_layout, "velocity y label");

    le = m_per_entity_velocity_y_edit =
        new LineEdit(20, control_layout, "per-entity velocity y edit");
    le->GetCharacterFilter().SetFilterType(DECIMAL_NOTATION_CHARACTER_FILTER_TYPE);
    le->GetCharacterFilter().SetFilter(DECIMAL_NOTATION_CHARACTER_FILTER);

    vef = m_object_selection_set_velocity_y_edit =
        new ValueEdit<Float>(
            DECIMAL_NOTATION_PRINTF_FORMAT,
            Util::TextToFloat,
            control_layout,
            "object selection set velocity y edit");
    vef->SetValue(0.0);

    //////////////////////////////////////////////////////////////////////////
    // speed row
    new Label("Speed", control_layout, "speed label");

    le = m_per_entity_speed_edit =
        new LineEdit(20, control_layout, "per-entity speed edit");
    le->GetCharacterFilter().SetFilterType(DECIMAL_NOTATION_CHARACTER_FILTER_TYPE);
    le->GetCharacterFilter().SetFilter(DECIMAL_NOTATION_CHARACTER_FILTER);

    vef = m_object_selection_set_speed_edit =
        new ValueEdit<Float>(
            DECIMAL_NOTATION_PRINTF_FORMAT,
            Util::TextToFloat,
            control_layout,
            "object selection set speed edit");
    vef->SetValue(0.0);

    //////////////////////////////////////////////////////////////////////////
    // velocity angle row
    new Label("Velocity Angle", control_layout, "velocity angle label");

    le = m_per_entity_velocity_angle_edit =
        new LineEdit(20, control_layout, "per-entity velocity angle edit");
    le->GetCharacterFilter().SetFilterType(DECIMAL_NOTATION_CHARACTER_FILTER_TYPE);
    le->GetCharacterFilter().SetFilter(DECIMAL_NOTATION_CHARACTER_FILTER);

    vef = m_object_selection_set_velocity_angle_edit =
        new ValueEdit<Float>(
            DECIMAL_NOTATION_PRINTF_FORMAT,
            Util::TextToFloat,
            control_layout,
            "object selection set velocity angle edit");
    vef->SetValue(0.0);

    //////////////////////////////////////////////////////////////////////////
    // second moment row
    new Label("Second Moment", control_layout, "second moment label");

    le = m_per_entity_second_moment_edit =
        new LineEdit(20, control_layout, "per-entity second moment edit");
    le->GetCharacterFilter().SetFilterType(DECIMAL_NOTATION_CHARACTER_FILTER_TYPE);
    le->GetCharacterFilter().SetFilter(DECIMAL_NOTATION_CHARACTER_FILTER);

    vef = m_object_selection_set_second_moment_edit =
        new ValueEdit<Float>(
            DECIMAL_NOTATION_PRINTF_FORMAT,
            Util::TextToFloat,
            control_layout,
            "object selection set second moment edit");
    vef->SetValidator(&m_validator_greater_than_zero);
    vef->SetValue(0.0);

    //////////////////////////////////////////////////////////////////////////
    // angular velocity row
    new Label("Angular Velocity", control_layout, "angular_velocity label");

    le = m_per_entity_angular_velocity_edit =
        new LineEdit(20, control_layout, "per-entity angular_velocity edit");
    le->GetCharacterFilter().SetFilterType(DECIMAL_NOTATION_CHARACTER_FILTER_TYPE);
    le->GetCharacterFilter().SetFilter(DECIMAL_NOTATION_CHARACTER_FILTER);

    vef = m_object_selection_set_angular_velocity_edit =
        new ValueEdit<Float>(
            DECIMAL_NOTATION_PRINTF_FORMAT,
            Util::TextToFloat,
            control_layout,
            "object selection set angular velocity edit");
    vef->SetValue(0.0);

    //////////////////////////////////////////////////////////////////////////
    // elasticity row
    new Label("Elasticity", control_layout, "elasticity label");

    le = m_per_entity_elasticity_edit =
        new LineEdit(20, control_layout, "per-entity elasticity edit");
    le->GetCharacterFilter().SetFilterType(DECIMAL_NOTATION_CHARACTER_FILTER_TYPE);
    le->GetCharacterFilter().SetFilter(DECIMAL_NOTATION_CHARACTER_FILTER);

    vef = m_object_selection_set_elasticity_edit =
        new ValueEdit<Float>(
            DECIMAL_NOTATION_PRINTF_FORMAT,
            Util::TextToFloat,
            control_layout,
            "object selection set elasticity edit");
    vef->SetValidator(&m_validator_elasticity);
    vef->SetValue(0.0);

    //////////////////////////////////////////////////////////////////////////
    // density row
    new Label("Density", control_layout, "density label");

    le = m_per_entity_density_edit =
        new LineEdit(20, control_layout, "per-entity density edit");
    le->GetCharacterFilter().SetFilterType(DECIMAL_NOTATION_CHARACTER_FILTER_TYPE);
    le->GetCharacterFilter().SetFilter(DECIMAL_NOTATION_CHARACTER_FILTER);

    vef = m_object_selection_set_density_edit =
        new ValueEdit<Float>(
            DECIMAL_NOTATION_PRINTF_FORMAT,
            Util::TextToFloat,
            control_layout,
            "object selection set density edit");
    vef->SetValidator(&m_validator_greater_than_zero);
    vef->SetValue(0.0);

    //////////////////////////////////////////////////////////////////////////
    // applies gravity row
    l = new Label("Applies Gravity", control_layout, "applies gravity label");

    cpw = new CellPaddingWidget(
        control_layout,
        "applies gravity checkbox cell padding widget");
    cpw->SetAlignmentComponent(0, LEFT);

    cb = m_per_entity_applies_gravity_checkbox =
        new CheckBox(cpw, "applies gravity checkbox");
    cb->FixSizeRatios(FloatVector2(0.04f, 0.04f));

    Layout *applies_gravity_button_layout =
        new Layout(
            HORIZONTAL,
            control_layout,
            "applies gravity object selection set button layout");
    applies_gravity_button_layout->SetIsUsingZeroedFrameMargins(true);

    button = m_object_selection_set_turn_on_applies_gravity_button =
        new Button(
            "On",
            applies_gravity_button_layout,
            "object selection set turn on applies gravity");

    button = m_object_selection_set_turn_off_applies_gravity_button =
        new Button(
            "Off",
            applies_gravity_button_layout,
            "object selection set turn off applies gravity");

    //////////////////////////////////////////////////////////////////////////
    // reacts to gravity row
    l = new Label("Reacts To Gravity", control_layout, "reacts to gravity label");

    cpw = new CellPaddingWidget(
        control_layout,
        "reacts to gravity checkbox cell padding widget");
    cpw->SetAlignmentComponent(0, LEFT);

    cb = m_per_entity_reacts_to_gravity_checkbox =
        new CheckBox(cpw, "reacts to gravity checkbox");
    cb->FixSizeRatios(FloatVector2(0.04f, 0.04f));

    Layout *reacts_to_gravity_button_layout =
        new Layout(
            HORIZONTAL,
            control_layout,
            "reacts to gravity object selection set button layout");
    reacts_to_gravity_button_layout->SetIsUsingZeroedFrameMargins(true);

    button = m_object_selection_set_turn_on_reacts_to_gravity_button =
        new Button(
            "On",
            reacts_to_gravity_button_layout,
            "object selection set turn on reacts to gravity");

    button = m_object_selection_set_turn_off_reacts_to_gravity_button =
        new Button(
            "Off",
            reacts_to_gravity_button_layout,
            "object selection set turn off reacts to gravity");

    //////////////////////////////////////////////////////////////////////////
    // make the main layout be the main widget
    SetMainWidget(main_layout);

    ConnectSignals();
}

void MapEditor2::EntityPropertiesPanel::SetPerEntityAppliesGravity (
    bool const applies_gravity)
{
    m_per_entity_applies_gravity_checkbox->SetIsChecked(applies_gravity);
}

void MapEditor2::EntityPropertiesPanel::SetPerEntityReactsToGravity (
    bool const reacts_to_gravity)
{
    m_per_entity_reacts_to_gravity_checkbox->SetIsChecked(reacts_to_gravity);
}

void MapEditor2::EntityPropertiesPanel::SetObjectSelectionSetFirstMoment (
    Float const first_moment)
{
    m_object_selection_set_first_moment_edit->SetValue(first_moment);
}

void MapEditor2::EntityPropertiesPanel::SetObjectSelectionSetVelocity (
    FloatVector2 const &velocity)
{
    m_object_selection_set_velocity_x_edit->SetValue(velocity[Dim::X]);
    m_object_selection_set_velocity_y_edit->SetValue(velocity[Dim::Y]);
    m_object_selection_set_speed_edit->SetValue(velocity.GetLength());
    m_object_selection_set_velocity_angle_edit->SetValue(
        Math::Atan2(velocity[Dim::Y], velocity[Dim::X]));
}

void MapEditor2::EntityPropertiesPanel::SetObjectSelectionSetSecondMoment (
    Float const second_moment)
{
    m_object_selection_set_second_moment_edit->SetValue(second_moment);
}

void MapEditor2::EntityPropertiesPanel::SetObjectSelectionSetAngularVelocity (
    Float const angular_velocity)
{
    m_object_selection_set_angular_velocity_edit->SetValue(angular_velocity);
}

void MapEditor2::EntityPropertiesPanel::SetObjectSelectionSetElasticity (
    Float const elasticity)
{
    m_object_selection_set_elasticity_edit->SetValue(elasticity);
}

void MapEditor2::EntityPropertiesPanel::SetObjectSelectionSetDensity (
    Float const density)
{
    m_object_selection_set_density_edit->SetValue(density);
}

void MapEditor2::EntityPropertiesPanel::SetSelectedEntityCount (
    Uint32 const selected_entity_count)
{
    if (m_selected_entity_count != selected_entity_count)
    {
        m_selected_entity_count = selected_entity_count;

        // dis/enable the appropriate widgets
        bool enable_controls = m_selected_entity_count == 1;
        m_object_selection_set_velocity_x_edit->SetIsEnabled(enable_controls);
        m_object_selection_set_velocity_y_edit->SetIsEnabled(enable_controls);
        m_object_selection_set_speed_edit->SetIsEnabled(enable_controls);
        m_object_selection_set_velocity_angle_edit->SetIsEnabled(enable_controls);
        m_object_selection_set_second_moment_edit->SetIsEnabled(enable_controls);
        m_object_selection_set_angular_velocity_edit->SetIsEnabled(enable_controls);
        m_object_selection_set_elasticity_edit->SetIsEnabled(enable_controls);
        m_object_selection_set_density_edit->SetIsEnabled(enable_controls);
        m_per_entity_applies_gravity_checkbox->SetIsEnabled(enable_controls);
        m_per_entity_reacts_to_gravity_checkbox->SetIsEnabled(enable_controls);
    }
}

void MapEditor2::EntityPropertiesPanel::ConnectSignals ()
{
    // hook up the internal per-entity signals
    SignalHandler::Connect1(
        m_per_entity_first_moment_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_set_per_entity_first_moment);
    SignalHandler::Connect1(
        m_per_entity_velocity_x_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_set_per_entity_velocity_x);
    SignalHandler::Connect1(
        m_per_entity_velocity_y_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_set_per_entity_velocity_y);
    SignalHandler::Connect1(
        m_per_entity_speed_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_set_per_entity_speed);
    SignalHandler::Connect1(
        m_per_entity_velocity_angle_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_set_per_entity_velocity_angle);
    SignalHandler::Connect1(
        m_per_entity_second_moment_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_set_per_entity_second_moment);
    SignalHandler::Connect1(
        m_per_entity_angular_velocity_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_set_per_entity_angular_velocity);
    SignalHandler::Connect1(
        m_per_entity_elasticity_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_set_per_entity_elasticity);
    SignalHandler::Connect1(
        m_per_entity_density_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_set_per_entity_density);
    SignalHandler::Connect1(
        m_per_entity_applies_gravity_checkbox->SenderCheckedStateChanged(),
        &m_internal_receiver_set_per_entity_applies_gravity);
    SignalHandler::Connect1(
        m_per_entity_reacts_to_gravity_checkbox->SenderCheckedStateChanged(),
        &m_internal_receiver_set_per_entity_reacts_to_gravity);

    // hook up the internal object selection set signals
    SignalHandler::Connect1(
        m_object_selection_set_first_moment_edit->SenderValueSetByEnterKey(),
        &m_internal_receiver_set_object_selection_set_first_moment);
    SignalHandler::Connect1(
        m_object_selection_set_velocity_x_edit->SenderValueSetByEnterKey(),
        &m_internal_receiver_set_object_selection_set_velocity_x);
    SignalHandler::Connect1(
        m_object_selection_set_velocity_y_edit->SenderValueSetByEnterKey(),
        &m_internal_receiver_set_object_selection_set_velocity_y);
    SignalHandler::Connect1(
        m_object_selection_set_speed_edit->SenderValueSetByEnterKey(),
        &m_internal_receiver_set_object_selection_set_speed);
    SignalHandler::Connect1(
        m_object_selection_set_velocity_angle_edit->SenderValueSetByEnterKey(),
        &m_internal_receiver_set_object_selection_set_velocity_angle);
    SignalHandler::Connect1(
        m_object_selection_set_second_moment_edit->SenderValueSetByEnterKey(),
        &m_internal_receiver_set_object_selection_set_second_moment);
    SignalHandler::Connect1(
        m_object_selection_set_angular_velocity_edit->SenderValueSetByEnterKey(),
        &m_internal_receiver_set_object_selection_set_angular_velocity);
    SignalHandler::Connect1(
        m_object_selection_set_elasticity_edit->SenderValueSetByEnterKey(),
        &m_internal_receiver_set_object_selection_set_elasticity);
    SignalHandler::Connect1(
        m_object_selection_set_density_edit->SenderValueSetByEnterKey(),
        &m_internal_receiver_set_object_selection_set_density);

    SignalHandler::Connect0(
        m_object_selection_set_turn_on_applies_gravity_button->SenderReleased(),
        &m_internal_receiver_object_selection_set_applies_gravity_on);
    SignalHandler::Connect0(
        m_object_selection_set_turn_off_applies_gravity_button->SenderReleased(),
        &m_internal_receiver_object_selection_set_applies_gravity_off);
    SignalHandler::Connect0(
        m_object_selection_set_turn_on_reacts_to_gravity_button->SenderReleased(),
        &m_internal_receiver_object_selection_set_reacts_to_gravity_on);
    SignalHandler::Connect0(
        m_object_selection_set_turn_off_reacts_to_gravity_button->SenderReleased(),
        &m_internal_receiver_object_selection_set_reacts_to_gravity_off);
}

void MapEditor2::EntityPropertiesPanel::InternalSetPerEntityFirstMoment (
    std::string const &first_moment)
{
    // don't do anything if the string is empty
    if (first_moment.empty())
        return;
    m_sender_per_entity_first_moment_assigned.Signal(
        m_validator_greater_than_zero.Validate(
            Util::TextToFloat(first_moment.c_str())));
    m_per_entity_first_moment_edit->SetText("");
}

void MapEditor2::EntityPropertiesPanel::InternalSetPerEntityVelocityX (
    std::string const &velocity_x)
{
    // don't do anything if the string is empty
    if (velocity_x.empty())
        return;
    m_sender_per_entity_velocity_x_assigned.Signal(Util::TextToFloat(velocity_x.c_str()));
    m_per_entity_velocity_x_edit->SetText("");
}

void MapEditor2::EntityPropertiesPanel::InternalSetPerEntityVelocityY (
    std::string const &velocity_y)
{
    // don't do anything if the string is empty
    if (velocity_y.empty())
        return;
    m_sender_per_entity_velocity_y_assigned.Signal(Util::TextToFloat(velocity_y.c_str()));
    m_per_entity_velocity_y_edit->SetText("");
}

void MapEditor2::EntityPropertiesPanel::InternalSetPerEntitySpeed (
    std::string const &speed)
{
    // don't do anything if the string is empty
    if (speed.empty())
        return;
    m_sender_per_entity_speed_assigned.Signal(Util::TextToFloat(speed.c_str()));
    m_per_entity_speed_edit->SetText("");
}

void MapEditor2::EntityPropertiesPanel::InternalSetPerEntityVelocityAngle (
    std::string const &velocity_angle)
{
    // don't do anything if the string is empty
    if (velocity_angle.empty())
        return;
    m_sender_per_entity_velocity_angle_assigned.Signal(Util::TextToFloat(velocity_angle.c_str()));
    m_per_entity_velocity_angle_edit->SetText("");
}

void MapEditor2::EntityPropertiesPanel::InternalSetPerEntitySecondMoment (
    std::string const &second_moment)
{
    // don't do anything if the string is empty
    if (second_moment.empty())
        return;
    m_sender_per_entity_second_moment_assigned.Signal(
        m_validator_greater_than_zero.Validate(
            Util::TextToFloat(second_moment.c_str())));
    m_per_entity_second_moment_edit->SetText("");
}

void MapEditor2::EntityPropertiesPanel::InternalSetPerEntityAngularVelocity (
    std::string const &angular_velocity)
{
    // don't do anything if the string is empty
    if (angular_velocity.empty())
        return;
    m_sender_per_entity_angular_velocity_assigned.Signal(Util::TextToFloat(angular_velocity.c_str()));
    m_per_entity_angular_velocity_edit->SetText("");
}

void MapEditor2::EntityPropertiesPanel::InternalSetPerEntityElasticity (
    std::string const &elasticity)
{
    // don't do anything if the string is empty
    if (elasticity.empty())
        return;
    m_sender_per_entity_elasticity_assigned.Signal(
        m_validator_elasticity.Validate(
            Util::TextToFloat(elasticity.c_str())));
    m_per_entity_elasticity_edit->SetText("");
}

void MapEditor2::EntityPropertiesPanel::InternalSetPerEntityDensity (
    std::string const &density)
{
    // don't do anything if the string is empty
    if (density.empty())
        return;
    m_sender_per_entity_density_assigned.Signal(
        m_validator_greater_than_zero.Validate(
            Util::TextToFloat(density.c_str())));
    m_per_entity_density_edit->SetText("");
}

void MapEditor2::EntityPropertiesPanel::InternalSetPerEntityAppliesGravity (
    bool const applies_gravity)
{
    m_sender_per_entity_applies_gravity_assigned.Signal(applies_gravity);
}

void MapEditor2::EntityPropertiesPanel::InternalSetPerEntityReactsToGravity (
    bool const reacts_to_gravity)
{
    m_sender_per_entity_reacts_to_gravity_assigned.Signal(reacts_to_gravity);
}

void MapEditor2::EntityPropertiesPanel::InternalSetObjectSelectionSetFirstMoment (
    Float const first_moment)
{
    m_sender_object_selection_set_first_moment_changed.Signal(first_moment);
}

void MapEditor2::EntityPropertiesPanel::InternalSetObjectSelectionSetVelocityX (
    Float const velocity_x)
{
    m_sender_object_selection_set_velocity_x_changed.Signal(velocity_x);
}

void MapEditor2::EntityPropertiesPanel::InternalSetObjectSelectionSetVelocityY (
    Float const velocity_y)
{
    m_sender_object_selection_set_velocity_y_changed.Signal(velocity_y);
}

void MapEditor2::EntityPropertiesPanel::InternalSetObjectSelectionSetSpeed (
    Float const speed)
{
    m_sender_object_selection_set_speed_changed.Signal(speed);
}

void MapEditor2::EntityPropertiesPanel::InternalSetObjectSelectionSetVelocityAngle (
    Float const velocity_angle)
{
    m_sender_object_selection_set_velocity_angle_changed.Signal(velocity_angle);
}

void MapEditor2::EntityPropertiesPanel::InternalSetObjectSelectionSetSecondMoment (
    Float const second_moment)
{
    m_sender_object_selection_set_second_moment_changed.Signal(second_moment);
}

void MapEditor2::EntityPropertiesPanel::InternalSetObjectSelectionSetAngularVelocity (
    Float const angular_velocity)
{
    m_sender_object_selection_set_angular_velocity_changed.Signal(angular_velocity);
}

void MapEditor2::EntityPropertiesPanel::InternalSetObjectSelectionSetElasticity (
    Float const elasticity)
{
    m_sender_object_selection_set_elasticity_changed.Signal(elasticity);
}

void MapEditor2::EntityPropertiesPanel::InternalSetObjectSelectionSetDensity (
    Float const density)
{
    m_sender_object_selection_set_density_changed.Signal(density);
}

void MapEditor2::EntityPropertiesPanel::InternalObjectSelectionSetAppliesGravityOn ()
{
    m_sender_per_entity_applies_gravity_assigned.Signal(true);
    m_per_entity_applies_gravity_checkbox->SetIsChecked(true);
}

void MapEditor2::EntityPropertiesPanel::InternalObjectSelectionSetAppliesGravityOff ()
{
    m_sender_per_entity_applies_gravity_assigned.Signal(false);
    m_per_entity_applies_gravity_checkbox->SetIsChecked(false);
}

void MapEditor2::EntityPropertiesPanel::InternalObjectSelectionSetReactsToGravityOn ()
{
    m_sender_per_entity_reacts_to_gravity_assigned.Signal(true);
    m_per_entity_reacts_to_gravity_checkbox->SetIsChecked(true);
}

void MapEditor2::EntityPropertiesPanel::InternalObjectSelectionSetReactsToGravityOff ()
{
    m_sender_per_entity_reacts_to_gravity_assigned.Signal(false);
    m_per_entity_reacts_to_gravity_checkbox->SetIsChecked(false);
}

} // end of namespace Xrb
