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
            resource_instance_base->Print(stderr);
        }
    }
    ASSERT1(m_instance_map.empty() && "There are unfreed resources");
}

void ResourceLibrary::UnmapKey (
    ResourceLibrary::ResourceInstanceKey const &key)
{
    InstanceMap::iterator it = m_instance_map.find(key);
    ASSERT1(it != m_instance_map.end());
    ASSERT1(it->second != NULL);
    m_instance_map.erase(it);
}

// ///////////////////////////////////////////////////////////////////////////
// Xrb::ResourceLibrary::ResourceInstanceKey::LessThan
// ///////////////////////////////////////////////////////////////////////////

bool ResourceLibrary::ResourceInstanceKey::LessThan::operator () (
    ResourceInstanceKey const &left_operand,
    ResourceInstanceKey const &right_operand) const
{
    return
        left_operand.m_load_parameter < right_operand.m_load_parameter
        ||
        (left_operand.m_load_parameter == right_operand.m_load_parameter &&
         left_operand.m_filename.compare(right_operand.m_filename) == -1);
}

} // end of namespace Xrb
