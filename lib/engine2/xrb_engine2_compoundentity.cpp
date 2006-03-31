// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_compoundentity.cpp by Victor Dods, created 2005/10/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_engine2_compoundentity.h"

#include "xrb_serializer.h"

namespace Xrb
{

/*
Engine2::CompoundEntity *Engine2::CompoundEntity::Create (std::string const &prefab_filename)
{
}
*/

Engine2::CompoundEntity *Engine2::CompoundEntity::Create (Serializer &serializer)
{
    CompoundEntity *retval = new CompoundEntity();

    // call ReadClassSpecific for this and all superclasses
    retval->Object::ReadClassSpecific(serializer);
    retval->Compound::ReadClassSpecific(serializer);
    retval->Entity::ReadClassSpecific(serializer);

    return retval;
}

Engine2::Object *Engine2::CompoundEntity::CreateClone () const
{
    CompoundEntity *retval = new CompoundEntity();
    
    retval->Object::CloneProperties(this);
    retval->Compound::CloneProperties(this);
    retval->Entity::CloneProperties(this);
    retval->CompoundEntity::CloneProperties(this);

    return static_cast<Object *>(retval);
}

void Engine2::CompoundEntity::Write (Serializer &serializer) const
{
    WriteSubType(serializer);
    
    // call WriteClassSpecific for this and all superclasses
    Object::WriteClassSpecific(serializer);
    Compound::WriteClassSpecific(serializer);
    Entity::WriteClassSpecific(serializer);
}

Engine2::CompoundEntity::CompoundEntity ()
    :
    Object(),
    Entity(),
    Compound()
{
    m_sub_type = Object::ST_COMPOUND_ENTITY;
}

// copies the properties of the given object to this object
void Engine2::CompoundEntity::CloneProperties (Engine2::Object const *const object)
{
//     CompoundEntity const *compound_entity =
//         dynamic_cast<CompoundEntity const *>(object);
//     ASSERT1(compound_entity != NULL)
    ASSERT1(dynamic_cast<CompoundEntity const *>(object) != NULL)

    // nothing to do (yet)
}

} // end of namespace Xrb
