// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_sprite.cpp by Victor Dods, created 2005/07/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_mapeditor2_sprite.hpp"

#include "xrb_mapeditor2_spriteentity.hpp"
#include "xrb_serializer.hpp"

namespace Xrb
{

MapEditor2::Sprite *MapEditor2::Sprite::CreateSpriteNonEntityClone (
    SpriteEntity const *const sprite_entity)
{
    ASSERT1(sprite_entity != NULL);
    Sprite *retval = new Sprite(sprite_entity->GetTexture());
    retval->Engine2::Object::CloneProperties(sprite_entity);
    retval->Object::CloneProperties(sprite_entity);
    retval->Sprite::CloneProperties(sprite_entity);
    return retval;
}


MapEditor2::Sprite *MapEditor2::Sprite::Create (std::string const &texture_path)
{
    Resource<GLTexture> texture =
        Singleton::ResourceLibrary().LoadPath<GLTexture>(
            GLTexture::Create,
            texture_path);
    if (!texture.IsValid())
        return NULL;

    return new Sprite(texture);
}

MapEditor2::Sprite *MapEditor2::Sprite::Create (Resource<GLTexture> const &texture)
{
    if (!texture.IsValid())
        return NULL;

    return new Sprite(texture);
}

MapEditor2::Sprite *MapEditor2::Sprite::Create (Serializer &serializer)
{
    Sprite *retval = new Sprite(Resource<GLTexture>());

    // call ReadClassSpecific for this and all superclasses
    retval->Engine2::Object::ReadClassSpecific(serializer);
    retval->Engine2::Sprite::ReadClassSpecific(serializer);

    return retval;
}

Engine2::Object *MapEditor2::Sprite::CreateClone () const
{
    ASSERT1(m_texture.IsValid());

    Sprite *retval = new Sprite(m_texture);
    retval->Engine2::Object::CloneProperties(this);
    retval->Engine2::Sprite::CloneProperties(this);

    return static_cast<Engine2::Object *>(retval);
}

MapEditor2::Entity *MapEditor2::Sprite::CreateEntityClone () const
{
    return static_cast<Entity *>(SpriteEntity::CreateSpriteEntityClone(this));
}

} // end of namespace Xrb
