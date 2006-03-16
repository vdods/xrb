// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_compoundentity.h by Victor Dods, created 2005/10/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_COMPOUNDENTITY_H_)
#define _XRB_ENGINE2_COMPOUNDENTITY_H_

#include "xrb.h"

#include "xrb_engine2_entity.h"
#include "xrb_engine2_compound.h"

namespace Xrb
{

namespace Engine2
{
    
    class CompoundEntity : public virtual Entity, public virtual Compound
    {
    public:
    
        virtual ~CompoundEntity () { }
    
        // ///////////////////////////////////////////////////////////////////
        // public serialization functions
        // ///////////////////////////////////////////////////////////////////
        
//         // create an entity/compound from a prefab file
//         static CompoundEntity *Create (std::string const &prefab_filename);
        // create an instance of this class by reading from the given Serializer
        static CompoundEntity *Create (Serializer &serializer);
        // makes calls to WriteClassSpecific for this and all superclasses
        virtual void Write (Serializer &serializer) const;

        // ///////////////////////////////////////////////////////////////////
        // public overridable methods
        // ///////////////////////////////////////////////////////////////////
        
        // creates a clone of this compound entity
        virtual Object *CreateClone () const;
        // draw this sprite.
        virtual void Draw (
            DrawData const &draw_data,
            Float alpha_mask) const
        {
            Compound::Draw(draw_data, alpha_mask);
        }
    
        virtual void SetScaleFactors (FloatVector2 const &scale_factors) { Entity::SetScaleFactors(scale_factors); }
        virtual void SetScaleFactors (Float r, Float s) { Entity::SetScaleFactors(r, s); }
        virtual void SetScaleFactor (Float scale_factor) { Entity::SetScaleFactor(scale_factor); }
        virtual void Scale (FloatVector2 const &scale_factors) { Entity::Scale(scale_factors); }
        virtual void Scale (Float r, Float s) { Entity::Scale(r, s); }
        virtual void Scale (Float scale_factor) { Entity::Scale(scale_factor); }
        virtual void ResetScale () { Entity::ResetScale(); }

    protected:
    
        // protected constructor so that you must use Create()
        CompoundEntity ();
    
        // ///////////////////////////////////////////////////////////////////
        // protected overridable methods
        // ///////////////////////////////////////////////////////////////////
        
        // recalculates the radius when the contents of the compound change
        virtual void CalculateRadius () const { Compound::CalculateRadius(); }

        // ///////////////////////////////////////////////////////////////////
        
        // copies the properties of the given object to this object
        void CloneProperties (Object const *object);
    }; // end of class Engine2::CompoundEntity

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_COMPOUNDENTITY_H_)
