// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_spriteentity.cpp by Victor Dods, created 2005/07/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_spriteentity.h"

#include "xrb_resourcelibrary.h"
#include "xrb_serializer.h"

namespace Xrb
{

Engine2::SpriteEntity *Engine2::SpriteEntity::Create (std::string const &texture_filename)
{
    Resource<GLTexture> texture =
        Singletons::ResourceLibrary()->LoadFilename<GLTexture>(
            GLTexture::Create,
            texture_filename);
    if (!texture.GetIsValid())
        return NULL;

    return new SpriteEntity(texture);
}

Engine2::SpriteEntity *Engine2::SpriteEntity::Create (Serializer &serializer)
{
    SpriteEntity *retval = new SpriteEntity(Resource<GLTexture>());

    // call ReadClassSpecific for this and all superclasses
    retval->Object::ReadClassSpecific(serializer);
    retval->Sprite::ReadClassSpecific(serializer);
    retval->Entity::ReadClassSpecific(serializer);

    return retval;
}

void Engine2::SpriteEntity::Write (Serializer &serializer) const
{
    WriteSubType(serializer);
    
    // call WriteClassSpecific for this and all superclasses
    Object::WriteClassSpecific(serializer);
    Sprite::WriteClassSpecific(serializer);
    Entity::WriteClassSpecific(serializer);
}

Engine2::Object *Engine2::SpriteEntity::CreateClone () const
{
    SpriteEntity *retval = new SpriteEntity(m_texture);

    // call CloneProperties for this and all superclasses
    retval->Object::CloneProperties(this);
    retval->Sprite::CloneProperties(this);
    retval->Entity::CloneProperties(this);
    retval->SpriteEntity::CloneProperties(this);

    return static_cast<Object *>(retval);
}

Engine2::SpriteEntity::SpriteEntity (Resource<GLTexture> const &texture)
    :
    Object(),
    Sprite(texture),
    Entity()
{
    m_sub_type = Object::ST_SPRITE_ENTITY;
}

void Engine2::SpriteEntity::CloneProperties (Engine2::Object const *const object)
{
//     SpriteEntity const *sprite_entity =
//         dynamic_cast<SpriteEntity const *>(object);
//     ASSERT1(sprite_entity != NULL)
    ASSERT1(dynamic_cast<SpriteEntity const *>(object) != NULL)

    // nothing to do (yet)
}

} // end of namespace Xrb
