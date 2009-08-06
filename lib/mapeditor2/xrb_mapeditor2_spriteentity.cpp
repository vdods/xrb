// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_spriteentity.cpp by Victor Dods, created 2004/07/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_mapeditor2_spriteentity.hpp"

namespace Xrb
{

Color MapEditor2::SpriteEntity::ms_unselected_metrics_color(0.0f, 0.8f, 0.0f, 1.0f);
Color MapEditor2::SpriteEntity::ms_selected_metrics_color(0.7f, 1.0f, 0.7f, 1.0f);

MapEditor2::SpriteEntity *MapEditor2::SpriteEntity::CreateSpriteEntityClone (
    Sprite const *const sprite)
{
    ASSERT1(sprite != NULL);
    SpriteEntity *retval = new SpriteEntity(sprite->GetTexture());
    retval->Engine2::Object::CloneProperties(sprite);
    retval->Object::CloneProperties(sprite);
    retval->Sprite::CloneProperties(sprite);
    return retval;
}


MapEditor2::SpriteEntity *MapEditor2::SpriteEntity::Create (
    std::string const &texture_filename)
{
    Resource<GLTexture> texture =
        Singletons::ResourceLibrary().LoadFilename<GLTexture>(
            GLTexture::Create,
            texture_filename);
    if (!texture.IsValid())
        return NULL;

    return new SpriteEntity(texture);
}

MapEditor2::SpriteEntity *MapEditor2::SpriteEntity::Create (
    Resource<GLTexture> const &texture)
{
    if (!texture.IsValid())
        return NULL;

    return new SpriteEntity(texture);
}

MapEditor2::SpriteEntity *MapEditor2::SpriteEntity::Create (Serializer &serializer)
{
    SpriteEntity *retval = new SpriteEntity(Resource<GLTexture>());

    retval->Engine2::Object::ReadClassSpecific(serializer);
    retval->Engine2::Sprite::ReadClassSpecific(serializer);
    retval->Engine2::Entity::ReadClassSpecific(serializer);

    return retval;
}

Engine2::Object *MapEditor2::SpriteEntity::CreateClone () const
{
    ASSERT1(m_texture.IsValid());

    SpriteEntity *retval = new SpriteEntity(m_texture);
    retval->Engine2::Object::CloneProperties(this);
    retval->Engine2::Sprite::CloneProperties(this);
    retval->Engine2::Entity::CloneProperties(this);
    retval->Engine2::SpriteEntity::CloneProperties(this);

    return static_cast<Engine2::Object *>(retval);
}

MapEditor2::Object *MapEditor2::SpriteEntity::CreateNonEntityClone () const
{
    return static_cast<Object *>(Sprite::CreateSpriteNonEntityClone(this));
}

Color const &MapEditor2::SpriteEntity::GetUnselectedMetricsColor () const
{
    return MapEditor2::SpriteEntity::ms_unselected_metrics_color;
}

Color const &MapEditor2::SpriteEntity::SelectedMetricsColor () const
{
    return MapEditor2::SpriteEntity::ms_selected_metrics_color;
}
 
void MapEditor2::SpriteEntity::CloneProperties (Object const *const object)
{
//     SpriteEntity const *sprite_entity =
//         dynamic_cast<SpriteEntity const *>(object);
//     ASSERT1(sprite_entity != NULL);
    ASSERT1(dynamic_cast<SpriteEntity const *>(object) != NULL);

    // nothing to do (yet)
}

} // end of namespace Xrb
