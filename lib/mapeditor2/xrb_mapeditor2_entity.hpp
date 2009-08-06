// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_entity.hpp by Victor Dods, created 2005/07/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MAPEDITOR2_ENTITY_HPP_)
#define _XRB_MAPEDITOR2_ENTITY_HPP_

#include "xrb.hpp"

#include "xrb_engine2_entity.hpp"
#include "xrb_mapeditor2_object.hpp"

namespace Xrb
{

namespace MapEditor2
{

    class Entity : public virtual Object, public virtual Engine2::Entity
    {
    public:
    
        virtual ~Entity ()
        {
            ASSERT1(m_saved_entity_guts == NULL);
        }

        // ///////////////////////////////////////////////////////////////////
        // public overridable methods
        // ///////////////////////////////////////////////////////////////////
        
        virtual void Write (Serializer &serializer) const { Engine2::Entity::Write(serializer); }

        virtual Engine2::Object *CreateClone () const = 0;
        // creates an entity version clone of this object, or NULL if
        // it either already is an entity, or if not applicable.
        virtual Entity *CreateEntityClone () const
        {
            return NULL;
        }
        // creates a non-entity version clone of this object, or NULL if
        // it either already is a non-entity, or if not applicable.
        virtual Object *CreateNonEntityClone () const
        {
            return NULL;
        }
        // draws the linear velocity vector, and angular velocity arc
        virtual void DrawMetrics (
            DrawData const &draw_data,
            Float alpha_mask,
            MetricMode metric_mode);
    
        virtual void SetScaleFactors (FloatVector2 const &scale_factors) { Engine2::Entity::SetScaleFactors(scale_factors); }
        virtual void SetScaleFactors (Float r, Float s) { Engine2::Entity::SetScaleFactors(r, s); }
        virtual void SetScaleFactor (Float scale_factor) { Engine2::Entity::SetScaleFactor(scale_factor); }
        virtual void Scale (FloatVector2 const &scale_factors) { Engine2::Entity::Scale(scale_factors); }
        virtual void Scale (Float r, Float s) { Engine2::Entity::Scale(r, s); }
        virtual void Scale (Float scale_factor) { Engine2::Entity::Scale(scale_factor); }
        virtual void ResetScale () { Engine2::Entity::ResetScale(); }

        // ///////////////////////////////////////////////////////////////////
        // public accessors
        // ///////////////////////////////////////////////////////////////////        
        
        virtual Color const &GetUnselectedMetricsColor () const;
        virtual Color const &SelectedMetricsColor () const;

        // ///////////////////////////////////////////////////////////////////
        // public procedures
        // ///////////////////////////////////////////////////////////////////
            
        // scales the tip of the linear velocity vector using the given
        // transformation mode and origin, as part of a transformation being
        // applied to a set of entities.
        void ObjectSelectionSetScaleVelocity (
            Float scale_factor,
            FloatVector2 transformation_origin,
            TransformationMode transformation_mode);
        // rotates the tip of the linear velocity vector using the given
        // transformation mode and origin, as part of a transformation being
        // applied to a set of entities.
        void ObjectSelectionSetRotateVelocity (
            Float angle_delta,
            FloatVector2 transformation_origin,
            TransformationMode transformation_mode,
            FloatMatrix2 const &rotation_transformation);
    
    protected:
    
        // protected constructor so that you must use Create()
        Entity ()
            :
            Engine2::Object(),
            Object(),
            Engine2::Entity()
        { }

    private:
    
        // the color of the object metrics when not selected
        static Color ms_unselected_metrics_color;
        // the color of the object metrics when selected
        static Color ms_selected_metrics_color;
    }; // end of class MapEditor2::Entity

} // end of namespace MapEditor2

} // end of namespace Xrb

#endif // !defined(_XRB_MAPEDITOR2_ENTITY_HPP_)


