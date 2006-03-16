// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_spriteentity.h by Victor Dods, created 2005/07/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_SPRITEENTITY_H_)
#define _XRB_ENGINE2_SPRITEENTITY_H_

#include "xrb.h"

#include "xrb_engine2_entity.h"
#include "xrb_engine2_sprite.h"

namespace Xrb
{

namespace Engine2
{
    
    class SpriteEntity : public virtual Sprite, public virtual Entity
    {
    public:
    
        virtual ~SpriteEntity () { }

        // ///////////////////////////////////////////////////////////////////
        // public serialization functions
        // ///////////////////////////////////////////////////////////////////
            
        // create an entity/sprite from a PNG file
        static SpriteEntity *Create (std::string const &texture_filename);
        // create an instance of this class by reading from the given Serializer
        static SpriteEntity *Create (Serializer &serializer);
        // makes calls to WriteClassSpecific for this and all superclasses
        virtual void Write (Serializer &serializer) const;
        
        // ///////////////////////////////////////////////////////////////////
        // public overridable methods
        // ///////////////////////////////////////////////////////////////////
        
        // creates a clone of this entity
        virtual Object *CreateClone () const;
        // draw this sprite.
        virtual void Draw (
            DrawData const &draw_data,
            Float alpha_mask) const
        {
            Sprite::Draw(draw_data, alpha_mask);
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
        SpriteEntity (Resource<GLTexture> const &texture);
    
        // ///////////////////////////////////////////////////////////////////
        
        // copies the properties of the given object to this object
        void CloneProperties (Object const *object);
        // recalculates the radius (this should be called if/when the sprite's
        // transformation matrix changes
        virtual void CalculateRadius () const { Sprite::CalculateRadius(); }
    }; // end of class Engine2::SpriteEntity

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_SPRITEENTITY_H_)
