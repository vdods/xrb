// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_entitypropertiespanel.h by Victor Dods, created 2005/03/08
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MAPEDITOR2_ENTITYPROPERTIESPANEL_H_)
#define _XRB_MAPEDITOR2_ENTITYPROPERTIESPANEL_H_

#include "xrb.h"

#include "xrb_widget.h"
#include "xrb_validator.h"

namespace Xrb
{

class LineEdit;
class Button;
class CheckBox;
template <typename ValueType> class ValueEdit;

namespace MapEditor2
{

    class EntityPropertiesPanel : public Widget
    {
    public:
    
        EntityPropertiesPanel (
            Widget *parent,
            std::string const &name = "MapEditor2::EntityPropertiesPanel");
        virtual ~EntityPropertiesPanel () { }
    
        void SetPerEntityAppliesGravity (bool applies_gravity);
        void SetPerEntityReactsToGravity (bool reacts_to_gravity);
    
        void SetObjectSelectionSetFirstMoment (Float first_moment);
        void SetObjectSelectionSetVelocity (FloatVector2 const &velocity);
        void SetObjectSelectionSetSecondMoment (Float second_moment);
        void SetObjectSelectionSetAngularVelocity (Float angular_velocity);
        void SetObjectSelectionSetElasticity (Float elasticity);
        void SetObjectSelectionSetDensity (Float density);
    
        inline SignalSender1<Float> const *SenderPerEntityFirstMomentAssigned () { return &m_sender_per_entity_first_moment_assigned; }
        inline SignalSender1<Float> const *SenderPerEntityVelocityXAssigned () { return &m_sender_per_entity_velocity_x_assigned; }
        inline SignalSender1<Float> const *SenderPerEntityVelocityYAssigned () { return &m_sender_per_entity_velocity_y_assigned; }
        inline SignalSender1<Float> const *SenderPerEntitySpeedAssigned () { return &m_sender_per_entity_speed_assigned; }
        inline SignalSender1<Float> const *SenderPerEntityVelocityAngleAssigned () { return &m_sender_per_entity_velocity_angle_assigned; }
        inline SignalSender1<Float> const *SenderPerEntitySecondMomentAssigned () { return &m_sender_per_entity_second_moment_assigned; }
        inline SignalSender1<Float> const *SenderPerEntityAngularVelocityAssigned () { return &m_sender_per_entity_angular_velocity_assigned; }
        inline SignalSender1<Float> const *SenderPerEntityElasticityAssigned () { return &m_sender_per_entity_elasticity_assigned; }
        inline SignalSender1<Float> const *SenderPerEntityDensityAssigned () { return &m_sender_per_entity_density_assigned; }
        inline SignalSender1<bool> const *SenderPerEntityAppliesGravityAssigned () { return &m_sender_per_entity_applies_gravity_assigned; }
        inline SignalSender1<bool> const *SenderPerEntityReactsToGravityAssigned () { return &m_sender_per_entity_reacts_to_gravity_assigned; }
    
        inline SignalSender1<Float> const *SenderObjectSelectionSetFirstMomentChanged () { return &m_sender_object_selection_set_first_moment_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetVelocityXChanged () { return &m_sender_object_selection_set_velocity_x_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetVelocityYChanged () { return &m_sender_object_selection_set_velocity_y_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetSpeedChanged () { return &m_sender_object_selection_set_speed_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetVelocityAngleChanged () { return &m_sender_object_selection_set_velocity_angle_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetSecondMomentChanged () { return &m_sender_object_selection_set_second_moment_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetAngularVelocityChanged () { return &m_sender_object_selection_set_angular_velocity_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetElasticityChanged () { return &m_sender_object_selection_set_elasticity_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetDensityChanged () { return &m_sender_object_selection_set_density_changed; }
    
        SignalReceiver1<Uint32> const *ReceiverSetSelectedEntityCount () { return &m_receiver_set_selected_entity_count; }
    
        SignalReceiver1<bool> const *ReceiverSetPerEntityAppliesGravity () { return &m_receiver_set_per_entity_applies_gravity; }
        SignalReceiver1<bool> const *ReceiverSetPerEntityReactsToGravity () { return &m_receiver_set_per_entity_reacts_to_gravity; }
    
        SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetFirstMoment () { return &m_receiver_set_object_selection_set_first_moment; }
        SignalReceiver1<FloatVector2 const &> const *ReceiverSetObjectSelectionSetVelocity () { return &m_receiver_set_object_selection_set_velocity; }
        SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetSecondMoment () { return &m_receiver_set_object_selection_set_second_moment; }
        SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetAngularVelocity () { return &m_receiver_set_object_selection_set_angular_velocity; }
        SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetElasticity () { return &m_receiver_set_object_selection_set_elasticity; }
        SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetDensity () { return &m_receiver_set_object_selection_set_density; }
    
    private:
    
        void SetSelectedEntityCount (Uint32 selected_entity_count);
    
        void ConnectSignals ();
    
        void InternalSetPerEntityFirstMoment (std::string const &first_moment);
        void InternalSetPerEntityVelocityX (std::string const &velocity_x);
        void InternalSetPerEntityVelocityY (std::string const &velocity_y);
        void InternalSetPerEntitySpeed (std::string const &speed);
        void InternalSetPerEntityVelocityAngle (std::string const &velocity_angle);
        void InternalSetPerEntitySecondMoment (std::string const &second_moment);
        void InternalSetPerEntityAngularVelocity (std::string const &angular_velocity);
        void InternalSetPerEntityElasticity (std::string const &elasticity);
        void InternalSetPerEntityDensity (std::string const &density);
        void InternalSetPerEntityAppliesGravity (bool applies_gravity);
        void InternalSetPerEntityReactsToGravity (bool reacts_to_gravity);
    
        void InternalSetObjectSelectionSetFirstMoment (Float first_moment);
        void InternalSetObjectSelectionSetVelocityX (Float velocity_x);
        void InternalSetObjectSelectionSetVelocityY (Float velocity_y);
        void InternalSetObjectSelectionSetSpeed (Float speed);
        void InternalSetObjectSelectionSetVelocityAngle (Float velocity_angle);
        void InternalSetObjectSelectionSetSecondMoment (Float second_moment);
        void InternalSetObjectSelectionSetAngularVelocity (Float angular_velocity);
        void InternalSetObjectSelectionSetElasticity (Float elasticity);
        void InternalSetObjectSelectionSetDensity (Float density);
        void InternalObjectSelectionSetAppliesGravityOn ();
        void InternalObjectSelectionSetAppliesGravityOff ();
        void InternalObjectSelectionSetReactsToGravityOn ();
        void InternalObjectSelectionSetReactsToGravityOff ();
    
        Uint32 m_selected_entity_count;
    
        LineEdit *m_per_entity_first_moment_edit;
        LineEdit *m_per_entity_velocity_x_edit;
        LineEdit *m_per_entity_velocity_y_edit;
        LineEdit *m_per_entity_speed_edit;
        LineEdit *m_per_entity_velocity_angle_edit;
        LineEdit *m_per_entity_second_moment_edit;
        LineEdit *m_per_entity_angular_velocity_edit;
        LineEdit *m_per_entity_elasticity_edit;
        LineEdit *m_per_entity_density_edit;
    
        ValueEdit<Float> *m_object_selection_set_first_moment_edit;
        ValueEdit<Float> *m_object_selection_set_velocity_x_edit;
        ValueEdit<Float> *m_object_selection_set_velocity_y_edit;
        ValueEdit<Float> *m_object_selection_set_speed_edit;
        ValueEdit<Float> *m_object_selection_set_velocity_angle_edit;
        ValueEdit<Float> *m_object_selection_set_second_moment_edit;
        ValueEdit<Float> *m_object_selection_set_angular_velocity_edit;
        ValueEdit<Float> *m_object_selection_set_elasticity_edit;
        ValueEdit<Float> *m_object_selection_set_density_edit;
    
        CheckBox *m_per_entity_applies_gravity_checkbox;
        Button *m_object_selection_set_turn_on_applies_gravity_button;
        Button *m_object_selection_set_turn_off_applies_gravity_button;
    
        CheckBox *m_per_entity_reacts_to_gravity_checkbox;
        Button *m_object_selection_set_turn_on_reacts_to_gravity_button;
        Button *m_object_selection_set_turn_off_reacts_to_gravity_button;
    
        //////////////////////////////////////////////////////////////////////////
        // SignalSenders
        SignalSender1<Float> m_sender_per_entity_first_moment_assigned;
        SignalSender1<Float> m_sender_per_entity_velocity_x_assigned;
        SignalSender1<Float> m_sender_per_entity_velocity_y_assigned;
        SignalSender1<Float> m_sender_per_entity_speed_assigned;
        SignalSender1<Float> m_sender_per_entity_velocity_angle_assigned;
        SignalSender1<Float> m_sender_per_entity_second_moment_assigned;
        SignalSender1<Float> m_sender_per_entity_angular_velocity_assigned;
        SignalSender1<Float> m_sender_per_entity_elasticity_assigned;
        SignalSender1<Float> m_sender_per_entity_density_assigned;
        SignalSender1<bool> m_sender_per_entity_applies_gravity_assigned;
        SignalSender1<bool> m_sender_per_entity_reacts_to_gravity_assigned;
    
        SignalSender1<Float> m_sender_object_selection_set_first_moment_changed;
        SignalSender1<Float> m_sender_object_selection_set_velocity_x_changed;
        SignalSender1<Float> m_sender_object_selection_set_velocity_y_changed;
        SignalSender1<Float> m_sender_object_selection_set_speed_changed;
        SignalSender1<Float> m_sender_object_selection_set_velocity_angle_changed;
        SignalSender1<Float> m_sender_object_selection_set_second_moment_changed;
        SignalSender1<Float> m_sender_object_selection_set_angular_velocity_changed;
        SignalSender1<Float> m_sender_object_selection_set_elasticity_changed;
        SignalSender1<Float> m_sender_object_selection_set_density_changed;
    
        //////////////////////////////////////////////////////////////////////////
        // SignalReceivers
        SignalReceiver1<Uint32> m_receiver_set_selected_entity_count;
    
        SignalReceiver1<bool> m_receiver_set_per_entity_applies_gravity;
        SignalReceiver1<bool> m_receiver_set_per_entity_reacts_to_gravity;
    
        SignalReceiver1<Float> m_receiver_set_object_selection_set_first_moment;
        SignalReceiver1<FloatVector2 const &> m_receiver_set_object_selection_set_velocity;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_second_moment;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_angular_velocity;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_elasticity;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_density;
    
        SignalReceiver1<std::string const &> m_internal_receiver_set_per_entity_first_moment;
        SignalReceiver1<std::string const &> m_internal_receiver_set_per_entity_velocity_x;
        SignalReceiver1<std::string const &> m_internal_receiver_set_per_entity_velocity_y;
        SignalReceiver1<std::string const &> m_internal_receiver_set_per_entity_speed;
        SignalReceiver1<std::string const &> m_internal_receiver_set_per_entity_velocity_angle;
        SignalReceiver1<std::string const &> m_internal_receiver_set_per_entity_second_moment;
        SignalReceiver1<std::string const &> m_internal_receiver_set_per_entity_angular_velocity;
        SignalReceiver1<std::string const &> m_internal_receiver_set_per_entity_elasticity;
        SignalReceiver1<std::string const &> m_internal_receiver_set_per_entity_density;
        SignalReceiver1<bool> m_internal_receiver_set_per_entity_applies_gravity;
        SignalReceiver1<bool> m_internal_receiver_set_per_entity_reacts_to_gravity;
    
        SignalReceiver1<Float> m_internal_receiver_set_object_selection_set_first_moment;
        SignalReceiver1<Float> m_internal_receiver_set_object_selection_set_velocity_x;
        SignalReceiver1<Float> m_internal_receiver_set_object_selection_set_velocity_y;
        SignalReceiver1<Float> m_internal_receiver_set_object_selection_set_speed;
        SignalReceiver1<Float> m_internal_receiver_set_object_selection_set_velocity_angle;
        SignalReceiver1<Float> m_internal_receiver_set_object_selection_set_second_moment;
        SignalReceiver1<Float> m_internal_receiver_set_object_selection_set_angular_velocity;
        SignalReceiver1<Float> m_internal_receiver_set_object_selection_set_elasticity;
        SignalReceiver1<Float> m_internal_receiver_set_object_selection_set_density;
        SignalReceiver0 m_internal_receiver_object_selection_set_applies_gravity_on;
        SignalReceiver0 m_internal_receiver_object_selection_set_applies_gravity_off;
        SignalReceiver0 m_internal_receiver_object_selection_set_reacts_to_gravity_on;
        SignalReceiver0 m_internal_receiver_object_selection_set_reacts_to_gravity_off;
    
        //////////////////////////////////////////////////////////////////////////
        // Validators
        GreaterThanValidator<Float> m_validator_greater_than_zero;
        RangeValidator<Float> m_validator_elasticity;
    }; // end of class MapEditor2::EntityPropertiesPanel

} // end of namespace MapEditor2

} // end of namespace Xrb

#endif // !defined(_XRB_MAPEDITOR2_ENTITYPROPERTIESPANEL_H_)
