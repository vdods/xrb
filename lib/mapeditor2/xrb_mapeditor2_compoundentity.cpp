// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_compoundentity.cpp by Victor Dods, created 2004/10/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_mapeditor2_compoundentity.h"

namespace Xrb
{

Color MapEditor2::CompoundEntity::ms_unselected_metrics_color(0.0f, 0.8f, 0.0f, 1.0f);
Color MapEditor2::CompoundEntity::ms_selected_metrics_color(0.7f, 1.0f, 0.7f, 1.0f);

MapEditor2::CompoundEntity *MapEditor2::CompoundEntity::CreateCompoundEntityClone (
    Compound const *const compound) 
{
    ASSERT1(compound != NULL)
    CompoundEntity *retval = new CompoundEntity();
    retval->Engine2::Object::CloneProperties(compound);
    retval->Object::CloneProperties(compound);
    retval->Compound::CloneProperties(compound);
    return retval;
}

/*
MapEditor2::CompoundEntity *MapEditor2::CompoundEntity::Create (
    std::string const &prefab_filename)
{
}
*/

MapEditor2::CompoundEntity *MapEditor2::CompoundEntity::Create (Serializer &serializer)
{
    CompoundEntity *retval = new CompoundEntity();

    retval->Engine2::Object::ReadClassSpecific(serializer);
    retval->MapEditor2::Compound::ReadClassSpecific(serializer);
    retval->Engine2::Entity::ReadClassSpecific(serializer);

    return retval;
}

Engine2::Object *MapEditor2::CompoundEntity::CreateClone () const
{
    CompoundEntity *retval = new CompoundEntity();
    
    retval->Engine2::Object::CloneProperties(this);
    retval->Engine2::Compound::CloneProperties(this);
    retval->Engine2::Entity::CloneProperties(this);
    retval->Engine2::CompoundEntity::CloneProperties(this);

    return static_cast<Engine2::Object *>(retval);
}

MapEditor2::Object *MapEditor2::CompoundEntity::CreateNonEntityClone () const
{
    return static_cast<Object *>(Compound::CreateCompoundNonEntityClone(this));
}

Color const &MapEditor2::CompoundEntity::GetUnselectedMetricsColor () const
{
    return MapEditor2::CompoundEntity::ms_unselected_metrics_color;
}

Color const &MapEditor2::CompoundEntity::GetSelectedMetricsColor () const
{
    return MapEditor2::CompoundEntity::ms_selected_metrics_color;
}

void MapEditor2::CompoundEntity::CloneProperties (Object const *const object)
{
    CompoundEntity const *compound_entity =
        dynamic_cast<CompoundEntity const *>(object);
    ASSERT1(compound_entity != NULL)

    // nothing to do (yet)
}

} // end of namespace Xrb
