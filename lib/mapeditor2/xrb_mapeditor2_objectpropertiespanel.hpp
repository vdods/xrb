// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_objectpropertiespanel.hpp by Victor Dods, created 2005/02/22
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MAPEDITOR2_OBJECTPROPERTIESPANEL_HPP_)
#define _XRB_MAPEDITOR2_OBJECTPROPERTIESPANEL_HPP_

#include "xrb.hpp"

#include "xrb_widget.hpp"
#include "xrb_validator.hpp"

namespace Xrb
{

class LineEdit;
template <typename ValueType> class ValueEdit;
class Button;

namespace MapEditor2
{

    class ObjectPropertiesPanel : public Widget
    {
    public:
    
        ObjectPropertiesPanel (
            ContainerWidget *parent,
            std::string const &name = "MapEditor2::ObjectPropertiesPanel");
        virtual ~ObjectPropertiesPanel () { }
    
        void SetObjectSelectionSetOrigin (FloatVector2 const &origin);
        void SetObjectSelectionSetScale (Float scale);
        void SetObjectSelectionSetAngle (Float angle);
    
        inline SignalSender1<Float> const *SenderPerObjectOriginXAssigned () { return &m_sender_per_object_origin_x_assigned; }
        inline SignalSender1<Float> const *SenderPerObjectOriginYAssigned () { return &m_sender_per_object_origin_y_assigned; }
        inline SignalSender1<Float> const *SenderPerObjectScaleAssigned () { return &m_sender_per_object_scale_assigned; }
        inline SignalSender1<Float> const *SenderPerObjectAngleAssigned () { return &m_sender_per_object_angle_assigned; }
    
        inline SignalSender1<Float> const *SenderObjectSelectionSetOriginXChanged () { return &m_sender_object_selection_set_origin_x_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetOriginYChanged () { return &m_sender_object_selection_set_origin_y_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetScaleChanged () { return &m_sender_object_selection_set_scale_changed; }
        inline SignalSender1<Float> const *SenderObjectSelectionSetAngleChanged () { return &m_sender_object_selection_set_angle_changed; }
    
        inline SignalReceiver1<FloatVector2 const &> const *ReceiverSetObjectSelectionSetOrigin () { return &m_receiver_set_object_selection_set_origin; }
        inline SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetScale () { return &m_receiver_set_object_selection_set_scale; }
        inline SignalReceiver1<Float> const *ReceiverSetObjectSelectionSetAngle () { return &m_receiver_set_object_selection_set_angle; }
    
    private:
    
        void InitializeSignalSenders ();
        void InitializeSignalReceivers ();
        void ConnectSignals ();
    
        void InternalSetPerObjectOriginX (std::string const &origin_x);
        void InternalSetPerObjectOriginY (std::string const &origin_y);
        void InternalSetPerObjectScale (std::string const &scale);
        void InternalSetPerObjectAngle (std::string const &angle);
    
        void InternalSetObjectSelectionSetOriginX (Float origin_x);
        void InternalSetObjectSelectionSetOriginY (Float origin_y);
        void InternalSetObjectSelectionSetScale (Float scale);
        void InternalSetObjectSelectionSetAngle (Float angle);
    
        LineEdit *m_per_object_origin_x_edit;
        LineEdit *m_per_object_origin_y_edit;
        LineEdit *m_per_object_scale_edit;
        LineEdit *m_per_object_angle_edit;
    
        ValueEdit<Float> *m_object_selection_set_origin_x_edit;
        ValueEdit<Float> *m_object_selection_set_origin_y_edit;
        ValueEdit<Float> *m_object_selection_set_scale_edit;
        ValueEdit<Float> *m_object_selection_set_angle_edit;
    
        //////////////////////////////////////////////////////////////////////////
        // SignalSenders
        SignalSender1<Float> m_sender_per_object_origin_x_assigned;
        SignalSender1<Float> m_sender_per_object_origin_y_assigned;
        SignalSender1<Float> m_sender_per_object_scale_assigned;
        SignalSender1<Float> m_sender_per_object_angle_assigned;
    
        SignalSender1<Float> m_sender_object_selection_set_origin_x_changed;
        SignalSender1<Float> m_sender_object_selection_set_origin_y_changed;
        SignalSender1<Float> m_sender_object_selection_set_scale_changed;
        SignalSender1<Float> m_sender_object_selection_set_angle_changed;
    
        //////////////////////////////////////////////////////////////////////////
        // SignalReceivers
        SignalReceiver1<FloatVector2 const &> m_receiver_set_object_selection_set_origin;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_scale;
        SignalReceiver1<Float> m_receiver_set_object_selection_set_angle;
    
        SignalReceiver1<std::string const &> m_internal_receiver_set_per_object_origin_x;
        SignalReceiver1<std::string const &> m_internal_receiver_set_per_object_origin_y;
        SignalReceiver1<std::string const &> m_internal_receiver_set_per_object_scale;
        SignalReceiver1<std::string const &> m_internal_receiver_set_per_object_angle;
    
        SignalReceiver1<Float> m_internal_receiver_set_object_selection_set_origin_x;
        SignalReceiver1<Float> m_internal_receiver_set_object_selection_set_origin_y;
        SignalReceiver1<Float> m_internal_receiver_set_object_selection_set_scale;
        SignalReceiver1<Float> m_internal_receiver_set_object_selection_set_angle;
    
        //////////////////////////////////////////////////////////////////////////
        // Validators
        GreaterOrEqualValidator<Float> m_validator_scale;
    }; // end of class MapEditor2::ObjectPropertiesPanel

} // end of namespace MapEditor2

} // end of namespace Xrb

#endif // !defined(_XRB_MAPEDITOR2_OBJECTPROPERTIESPANEL_HPP_)
