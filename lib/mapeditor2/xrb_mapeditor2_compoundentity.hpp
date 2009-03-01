// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_compoundentity.hpp by Victor Dods, created 2004/10/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MAPEDITOR2_COMPOUNDENTITY_HPP_)
#define _XRB_MAPEDITOR2_COMPOUNDENTITY_HPP_

#include "xrb.hpp"

#include "xrb_mapeditor2_entity.hpp"
#include "xrb_mapeditor2_compound.hpp"
#include "xrb_engine2_compoundentity.hpp"

namespace Xrb
{

namespace MapEditor2
{

    class CompoundEntity :
        public Compound,
        public Entity,
        public Engine2::CompoundEntity
    {
    public:

        virtual ~CompoundEntity () { }

        // creates an entity version clone of the given compound
        static CompoundEntity *CreateCompoundEntityClone (
            Compound const *compound);

        // ///////////////////////////////////////////////////////////////////
        // public serialization functions
        // ///////////////////////////////////////////////////////////////////

//         // create an entity/sprite from a prefab file
//         static CompoundEntity *Create (std::string const &prefab_filename);
        // create an instance of this class by reading from the given Serializer
        static CompoundEntity *Create (Serializer &serializer);
        // necessary because of multiple virtual inheritance
        virtual void Write (Serializer &serializer) const
        {
            Engine2::CompoundEntity::Write(serializer);
        }

        // ///////////////////////////////////////////////////////////////////
        // public overridable methods
        // ///////////////////////////////////////////////////////////////////

        // creates a clone of this entity
        virtual Engine2::Object *CreateClone () const;
        // creates an entity version clone of this object, or NULL if
        // it either already is an entity, or if not applicable.
        virtual Entity *CreateEntityClone () const
        {
            return NULL;
        }
        // creates a non-entity version clone of this object, or NULL if
        // it either already is a non-entity, or if not applicable.
        virtual Object *CreateNonEntityClone () const;
        // necessary because of multiple virtual inheritance
        virtual void Draw (
            DrawData const &draw_data,
            Float alpha_mask) const
        {
            MapEditor2::Compound::Draw(draw_data, alpha_mask);
        }
        // draws the Entity and Compound metrics
        virtual void DrawMetrics (
            DrawData const &draw_data,
            Float alpha_mask,
            MetricMode metric_mode)
        {
            Compound::DrawMetrics(draw_data, alpha_mask, metric_mode);
            Entity::DrawMetrics(draw_data, alpha_mask, metric_mode);
        }

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
        virtual Color const &GetSelectedMetricsColor () const;

    protected:

        // protected constructor so that you must use Create()
        CompoundEntity ()
            :
            Engine2::Object(),
            Object(),
            Engine2::Compound(),
            Engine2::Entity(),
            Compound(),
            Entity(),
            Engine2::CompoundEntity()
        { }

        // ///////////////////////////////////////////////////////////////////
        // protected overridable methods
        // ///////////////////////////////////////////////////////////////////

        virtual void CalculateVisibleRadius () const
        {
            Compound::CalculateVisibleRadius();
        }

        // ///////////////////////////////////////////////////////////////////
        
        void CloneProperties (Object const *object);
            
    private:

        // the color of the object metrics when not selected
        static Color ms_unselected_metrics_color;
        // the color of the object metrics when selected
        static Color ms_selected_metrics_color;
    }; // end of class MapEditor2::CompoundEntity

} // end of namespace MapEditor2

} // end of namespace Xrb

#endif // !defined(_XRB_MAPEDITOR2_COMPOUNDENTITY_HPP_)
