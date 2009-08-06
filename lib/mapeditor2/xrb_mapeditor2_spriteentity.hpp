// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_spriteentity.hpp by Victor Dods, created 2004/07/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MAPEDITOR2_SPRITEENTITY_HPP_)
#define _XRB_MAPEDITOR2_SPRITEENTITY_HPP_

#include "xrb.hpp"

#include "xrb_mapeditor2_entity.hpp"
#include "xrb_mapeditor2_sprite.hpp"
#include "xrb_engine2_spriteentity.hpp"

namespace Xrb
{

namespace MapEditor2
{

    class SpriteEntity :
        public Sprite,
        public Entity,
        public Engine2::SpriteEntity
    {
    public:
    
        virtual ~SpriteEntity () { }

        // creates an entity version clone of the given sprite
        static SpriteEntity *CreateSpriteEntityClone (
            Sprite const *sprite);
            
        // ///////////////////////////////////////////////////////////////////
        // public serialization functions
        // ///////////////////////////////////////////////////////////////////
        
        // create a new sprite from the given texture filename        
        static SpriteEntity *Create (std::string const &texture_filename);
        // create a sprite entity from a resourced texture
        static SpriteEntity *Create (Resource<GLTexture> const &texture);
        // create an instance of this class by reading from the given Serializer
        static SpriteEntity *Create (Serializer &serializer);
        // writes this SpriteEntity to the serializer
        virtual void Write (Serializer &serializer) const
        {
            Engine2::SpriteEntity::Write(serializer);
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
        // this is only necessary because of multiple virtual inheritance
        virtual void Draw (
            DrawData const &draw_data,
            Float alpha_mask) const
        {
            Engine2::SpriteEntity::Draw(draw_data, alpha_mask);
        }
        // draws the Entity and Sprite metrics
        virtual void DrawMetrics (
            DrawData const &draw_data,
            Float alpha_mask,
            MetricMode metric_mode)
        {
            Sprite::DrawMetrics(draw_data, alpha_mask, metric_mode);
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
        // public accessors and modifiers
        // ///////////////////////////////////////////////////////////////////
    
        virtual Color const &GetUnselectedMetricsColor () const;
        virtual Color const &SelectedMetricsColor () const;
    
    protected:
    
        // protected constructor so that you must use Create
        SpriteEntity (Resource<GLTexture> const &texture)
            :
            Engine2::Object(),
            Object(),
            Engine2::Sprite(texture),
            Engine2::Entity(),
            Sprite(texture),
            Entity(),
            Engine2::SpriteEntity(texture)
        { }

        // ///////////////////////////////////////////////////////////////////
        // protected overridable methods
        // ///////////////////////////////////////////////////////////////////
        
        virtual void CalculateVisibleRadius () const
        {
            Engine2::Sprite::CalculateVisibleRadius();
        }

        // ///////////////////////////////////////////////////////////////////
            
        // clones the class-specific properties of the given object onto this.
        void CloneProperties (Object const *object);
    
    private:
    
        // the color of the object metrics when not selected
        static Color ms_unselected_metrics_color;
        // the color of the object metrics when selected
        static Color ms_selected_metrics_color;
    }; // end of class MapEditor2::SpriteEntity

} // end of namespace MapEditor2

} // end of namespace Xrb

#endif // !defined(_XRB_MAPEDITOR2_SPRITEENTITY_HPP_)
