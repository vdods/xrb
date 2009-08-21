// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_sprite.hpp by Victor Dods, created 2005/07/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MAPEDITOR2_SPRITE_HPP_)
#define _XRB_MAPEDITOR2_SPRITE_HPP_

#include "xrb.hpp"

#include "xrb_mapeditor2_object.hpp"
#include "xrb_engine2_sprite.hpp"

namespace Xrb
{

namespace MapEditor2
{

    class SpriteEntity;

    class Sprite : public virtual Object, public virtual Engine2::Sprite
    {
    public:

        virtual ~Sprite () { }

        // creates a non-entity version clone of the given sprite entity
        static Sprite *CreateSpriteNonEntityClone (
            SpriteEntity const *sprite_entity);

        // ///////////////////////////////////////////////////////////////////
        // public serialization functions
        // ///////////////////////////////////////////////////////////////////

        // create a new sprite from the given texture path        
        static Sprite *Create (std::string const &texture_path);
        // create a new sprite from a resourced texture
        static Sprite *Create (Resource<GLTexture> const &texture);
        // create an instance of this class by reading from the given Serializer
        static Sprite *Create (Serializer &serializer);

        // ///////////////////////////////////////////////////////////////////
        // public overridable methods
        // ///////////////////////////////////////////////////////////////////

        virtual void Write (Serializer &serializer) const { Engine2::Sprite::Write(serializer); }

        // creates a clone of this sprite
        virtual Engine2::Object *CreateClone () const;
        // creates an entity version clone of this object, or NULL if
        // it either already is an entity, or if not applicable.
        virtual Entity *CreateEntityClone () const;
        // creates a non-entity version clone of this object, or NULL if
        // it either already is a non-entity, or if not applicable.
        virtual Object *CreateNonEntityClone () const
        {
            return NULL;
        }

        virtual void Draw (
            Object::DrawData const &draw_data,
            Float alpha_mask) const
        {
            Engine2::Sprite::Draw(draw_data, alpha_mask);
        }
        // draws the bounding circle or square and calls Object::DrawMetrics
        virtual void DrawMetrics (
            DrawData const &draw_data,
            Float alpha_mask,
            MetricMode metric_mode)
        {
            Object::DrawMetrics(draw_data, alpha_mask, metric_mode);
        }

        // ///////////////////////////////////////////////////////////////////

    protected:

        // protected constructor so you must use Create()    
        Sprite (Resource<GLTexture> const &texture)
            :
            Engine2::Object(),
            Object(),
            Engine2::Sprite(texture)
        { }

        virtual void CalculateVisibleRadius () const { Engine2::Sprite::CalculateVisibleRadius(); }
    }; // end of class MapEditor2::Sprite

} // end of namespace MapEditor2

} // end of namespace Xrb

#endif // !defined(_XRB_MAPEDITOR2_SPRITE_HPP_)
