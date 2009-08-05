// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_objectpropertiespanel.cpp by Victor Dods, created 2005/02/22
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_mapeditor2_objectpropertiespanel.hpp"

#include "xrb_button.hpp"
#include "xrb_label.hpp"
#include "xrb_layout.hpp"
#include "xrb_lineedit.hpp"
#include "xrb_math.hpp"
#include "xrb_spacerwidget.hpp"
#include "xrb_util.hpp"
#include "xrb_valueedit.hpp"

namespace Xrb
{

MapEditor2::ObjectPropertiesPanel::ObjectPropertiesPanel (
    ContainerWidget *const parent,
    std::string const &name)
    :
    Widget(parent, name),
    m_sender_per_object_origin_x_assigned(this),
    m_sender_per_object_origin_y_assigned(this),
    m_sender_per_object_scale_assigned(this),
    m_sender_per_object_angle_assigned(this),
    m_sender_object_selection_set_origin_x_changed(this),
    m_sender_object_selection_set_origin_y_changed(this),
    m_sender_object_selection_set_scale_changed(this),
    m_sender_object_selection_set_angle_changed(this),
    m_receiver_set_object_selection_set_origin(
        &ObjectPropertiesPanel::SetObjectSelectionSetOrigin, this),
    m_receiver_set_object_selection_set_scale(
        &ObjectPropertiesPanel::SetObjectSelectionSetScale, this),
    m_receiver_set_object_selection_set_angle(
        &ObjectPropertiesPanel::SetObjectSelectionSetAngle, this),
    m_internal_receiver_set_per_object_origin_x(
        &ObjectPropertiesPanel::InternalSetPerObjectOriginX, this),
    m_internal_receiver_set_per_object_origin_y(
        &ObjectPropertiesPanel::InternalSetPerObjectOriginY, this),
    m_internal_receiver_set_per_object_scale(
        &ObjectPropertiesPanel::InternalSetPerObjectScale, this),
    m_internal_receiver_set_per_object_angle(
        &ObjectPropertiesPanel::InternalSetPerObjectAngle, this),
    m_internal_receiver_set_object_selection_set_origin_x(
        &ObjectPropertiesPanel::InternalSetObjectSelectionSetOriginX, this),
    m_internal_receiver_set_object_selection_set_origin_y(
        &ObjectPropertiesPanel::InternalSetObjectSelectionSetOriginY, this),
    m_internal_receiver_set_object_selection_set_scale(
        &ObjectPropertiesPanel::InternalSetObjectSelectionSetScale, this),
    m_internal_receiver_set_object_selection_set_angle(
        &ObjectPropertiesPanel::InternalSetObjectSelectionSetAngle, this),
    m_validator_scale(0.0f)
{
    Label *l;
    LineEdit *le;
    ValueEdit<Float> *vef;

    Layout *main_layout = new Layout(VERTICAL, this, "main layout");
    main_layout->SetIsUsingZeroedLayoutSpacingMargins(true);

    l = new Label("Object Properties", main_layout, "title label");
    l->SetIsHeightFixedToTextHeight(true);

    // set up this widget's layout
    Layout *control_layout = new Layout(ROW, 3, main_layout, "control layout");

    // create the controls in the layout

    //////////////////////////////////////////////////////////////////////////
    // top row of labels
    new SpacerWidget(control_layout);

    l = new Label("Per-Object", control_layout, "per-object column header");
    l->SetIsHeightFixedToTextHeight(true);

    l = new Label("Selection Set", control_layout, "object selection set column header");
    l->SetIsHeightFixedToTextHeight(true);

    // the actual controls

    //////////////////////////////////////////////////////////////////////////
    // origin X row
    new Label("Origin X", control_layout, "origin x label");

    le = m_per_object_origin_x_edit =
        new LineEdit(20, control_layout, "per-object origin x edit");
    le->GetCharacterFilter().SetFilterType(DECIMAL_NOTATION_CHARACTER_FILTER_TYPE);
    le->GetCharacterFilter().SetFilter(DECIMAL_NOTATION_CHARACTER_FILTER);

    vef = m_object_selection_set_origin_x_edit =
        new ValueEdit<Float>(
            DECIMAL_NOTATION_PRINTF_FORMAT,
            Util::TextToFloat,
            control_layout,
            "object selection set origin x edit");
    vef->SetValue(0.0);

    //////////////////////////////////////////////////////////////////////////
    // origin Y row
    new Label("Origin Y", control_layout, "origin y label");

    le = m_per_object_origin_y_edit =
        new LineEdit(20, control_layout, "per-object origin y edit");
    le->GetCharacterFilter().SetFilterType(DECIMAL_NOTATION_CHARACTER_FILTER_TYPE);
    le->GetCharacterFilter().SetFilter(DECIMAL_NOTATION_CHARACTER_FILTER);

    vef = m_object_selection_set_origin_y_edit =
        new ValueEdit<Float>(
            DECIMAL_NOTATION_PRINTF_FORMAT,
            Util::TextToFloat,
            control_layout,
            "object selection set origin y edit");
    vef->SetValue(0.0);

    //////////////////////////////////////////////////////////////////////////
    // scale row
    new Label("Scale", control_layout, "scale label");

    le = m_per_object_scale_edit =
        new LineEdit(20, control_layout, "per-object scale edit");
    le->GetCharacterFilter().SetFilterType(DECIMAL_NOTATION_CHARACTER_FILTER_TYPE);
    le->GetCharacterFilter().SetFilter(DECIMAL_NOTATION_CHARACTER_FILTER);

    vef = m_object_selection_set_scale_edit =
        new ValueEdit<Float>(
        DECIMAL_NOTATION_PRINTF_FORMAT,
        Util::TextToFloat,
        control_layout,
        "object selection set scale edit");
    vef->SetValidator(&m_validator_scale);
    vef->SetValue(0.0);

    //////////////////////////////////////////////////////////////////////////
    // angle row
    new Label("Angle", control_layout, "angle label");

    le = m_per_object_angle_edit =
        new LineEdit(20, control_layout, "per-object angle edit");
    le->GetCharacterFilter().SetFilterType(DECIMAL_NOTATION_CHARACTER_FILTER_TYPE);
    le->GetCharacterFilter().SetFilter(DECIMAL_NOTATION_CHARACTER_FILTER);

    vef = m_object_selection_set_angle_edit =
        new ValueEdit<Float>(
            DECIMAL_NOTATION_PRINTF_FORMAT,
            Util::TextToFloat,
            control_layout,
            "object selection set angle edit");
    vef->SetValue(0.0);

    //////////////////////////////////////////////////////////////////////////
    // make the main layout be the main widget
    SetMainWidget(main_layout);

    ConnectSignals();
}

void MapEditor2::ObjectPropertiesPanel::SetObjectSelectionSetOrigin (FloatVector2 const &origin)
{
    m_object_selection_set_origin_x_edit->SetValue(origin[Dim::X]);
    m_object_selection_set_origin_y_edit->SetValue(origin[Dim::Y]);
}

void MapEditor2::ObjectPropertiesPanel::SetObjectSelectionSetScale (Float const scale)
{
    m_object_selection_set_scale_edit->SetValue(scale);
}

void MapEditor2::ObjectPropertiesPanel::SetObjectSelectionSetAngle (Float const angle)
{
    m_object_selection_set_angle_edit->SetValue(Math::CanonicalAngle(angle));
}

void MapEditor2::ObjectPropertiesPanel::ConnectSignals ()
{
    // hook up the internal per-object signals
    SignalHandler::Connect1(
        m_per_object_origin_x_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_set_per_object_origin_x);
    SignalHandler::Connect1(
        m_per_object_origin_y_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_set_per_object_origin_y);
    SignalHandler::Connect1(
        m_per_object_scale_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_set_per_object_scale);
    SignalHandler::Connect1(
        m_per_object_angle_edit->SenderTextSetByEnterKey(),
        &m_internal_receiver_set_per_object_angle);

    // hook up the internal object selection set signals
    SignalHandler::Connect1(
        m_object_selection_set_origin_x_edit->SenderValueSetByEnterKey(),
        &m_internal_receiver_set_object_selection_set_origin_x);
    SignalHandler::Connect1(
        m_object_selection_set_origin_y_edit->SenderValueSetByEnterKey(),
        &m_internal_receiver_set_object_selection_set_origin_y);
    SignalHandler::Connect1(
        m_object_selection_set_scale_edit->SenderValueSetByEnterKey(),
        &m_internal_receiver_set_object_selection_set_scale);
    SignalHandler::Connect1(
        m_object_selection_set_angle_edit->SenderValueSetByEnterKey(),
        &m_internal_receiver_set_object_selection_set_angle);
}

void MapEditor2::ObjectPropertiesPanel::InternalSetPerObjectOriginX (
    std::string const &origin_x)
{
    // don't do anything if the string is empty
    if (origin_x.empty())
        return;
    m_sender_per_object_origin_x_assigned.Signal(Util::TextToFloat(origin_x.c_str()));
    m_per_object_origin_x_edit->SetText("");
}

void MapEditor2::ObjectPropertiesPanel::InternalSetPerObjectOriginY (
    std::string const &origin_y)
{
    // don't do anything if the string is empty
    if (origin_y.empty())
        return;
    m_sender_per_object_origin_y_assigned.Signal(Util::TextToFloat(origin_y.c_str()));
    m_per_object_origin_y_edit->SetText("");
}

void MapEditor2::ObjectPropertiesPanel::InternalSetPerObjectScale (
    std::string const &scale)
{
    // don't do anything if the string is empty
    if (scale.empty())
        return;
    m_sender_per_object_scale_assigned.Signal(
        m_validator_scale.Validate(
            Util::TextToFloat(scale.c_str())));
    m_per_object_scale_edit->SetText("");
}

void MapEditor2::ObjectPropertiesPanel::InternalSetPerObjectAngle (
    std::string const &angle)
{
    // don't do anything if the string is empty
    if (angle.empty())
        return;
    m_sender_per_object_angle_assigned.Signal(Util::TextToFloat(angle.c_str()));
    m_per_object_angle_edit->SetText("");
}

void MapEditor2::ObjectPropertiesPanel::InternalSetObjectSelectionSetOriginX (
    Float const origin_x)
{
    m_sender_object_selection_set_origin_x_changed.Signal(origin_x);
}

void MapEditor2::ObjectPropertiesPanel::InternalSetObjectSelectionSetOriginY (
    Float const origin_y)
{
    m_sender_object_selection_set_origin_y_changed.Signal(origin_y);
}

void MapEditor2::ObjectPropertiesPanel::InternalSetObjectSelectionSetScale (
    Float const scale)
{
    m_sender_object_selection_set_scale_changed.Signal(scale);
}

void MapEditor2::ObjectPropertiesPanel::InternalSetObjectSelectionSetAngle (
    Float const angle)
{
    m_sender_object_selection_set_angle_changed.Signal(angle);
}

} // end of namespace Xrb
