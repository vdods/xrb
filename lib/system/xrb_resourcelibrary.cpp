// ///////////////////////////////////////////////////////////////////////////
// xrb_resourcelibrary.cpp by Victor Dods, created 2005/06/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_resourcelibrary.hpp"

namespace Xrb
{

// ///////////////////////////////////////////////////////////////////////////
// Xrb::ResourceLibrary
// ///////////////////////////////////////////////////////////////////////////

ResourceLibrary::ResourceLibrary ()
{
}

ResourceLibrary::~ResourceLibrary ()
{
    if (!m_instance_map.empty())
    {
        for (InstanceMap::iterator it = m_instance_map.begin(),
                                   it_end = m_instance_map.end();
             it != it_end;
             ++it)
        {
            ResourceInstanceBase *resource_instance_base = it->second;
            ASSERT1(resource_instance_base != NULL);
            fprintf(stderr, "ResourceLibrary * UNFREED RESOURCE: ");
            resource_instance_base->Key().Print(stderr);
        }
    }
}

void ResourceLibrary::UnmapKey (
    ResourceLibrary::ResourceInstanceKey const &key)
{
    InstanceMap::iterator it = m_instance_map.find(key);
    ASSERT1(it != m_instance_map.end());
    ASSERT1(it->second != NULL);

    fprintf(stderr, "ResourceLibrary * unloading ");
    key.Print(stderr);
    fprintf(stderr, "\n");

    // delete the ResourceLoadParameters stored in the instance key.
    delete key.m_parameters;
    // erase the appropriate entry from the map.
    m_instance_map.erase(it);
}

// ///////////////////////////////////////////////////////////////////////////
// Xrb::ResourceLibrary::ResourceInstanceKey::LessThan
// ///////////////////////////////////////////////////////////////////////////

bool ResourceLibrary::ResourceInstanceKey::LessThan::operator () (
    ResourceInstanceKey const &left_operand,
    ResourceInstanceKey const &right_operand) const
{
    int compare_result;

    // sort by path first
    compare_result = left_operand.m_path.compare(right_operand.m_path);
    if (compare_result < 0)
        return true;
    if (compare_result > 0)
        return false;

    // sort by parameters pointer nullity next (NULL is preferred)
    if (left_operand.m_parameters == NULL && right_operand.m_parameters != NULL)
        return true;
    if (left_operand.m_parameters != NULL && right_operand.m_parameters == NULL)
        return false;
    if (left_operand.m_parameters == NULL && right_operand.m_parameters == NULL)
        return false;

    ASSERT1(left_operand.m_parameters != NULL);
    ASSERT1(right_operand.m_parameters != NULL);

    // sort by ResourceLoadParameters::Name() next.
    compare_result = left_operand.m_parameters->Name().compare(right_operand.m_parameters->Name());
    if (compare_result < 0)
        return true;
    if (compare_result > 0)
        return false;

    // the final comparison is ResourceLoadParameters-subclass-specific.
    return left_operand.m_parameters->IsLessThan(*right_operand.m_parameters);
}

} // end of namespace Xrb
