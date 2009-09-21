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
        fprintf(stderr, "ResourceLibrary * UNFREED RESOURCES:\n");
        PrintInventory(stderr, 1);
    }
}

void ResourceLibrary::PrintInventory (FILE *fptr, Uint32 tab_count) const
{
    ASSERT1(fptr != NULL);
    for (InstanceMap::const_iterator it = m_instance_map.begin(),
                                     it_end = m_instance_map.end();
         it != it_end;
         ++it)
    {
        ResourceLoadParameters const *load_parameters = it->first;
        for (Uint32 i = 0; i < tab_count; ++i)
            fprintf(fptr, "    %s: ", load_parameters->ResourceName().c_str());
        load_parameters->Print(fptr);
        fprintf(fptr, "\n");
    }
}

void ResourceLibrary::Unload (ResourceLoadParameters const &load_parameters)
{
    InstanceMap::iterator it = m_instance_map.find(&load_parameters);
    ASSERT1(it != m_instance_map.end());
    ASSERT1(it->second != NULL);

    fprintf(stderr, "ResourceLibrary * unloaded %s: ", load_parameters.ResourceName().c_str());
    load_parameters.Print(stderr);
    fprintf(stderr, "\n");

    // delete the stored ResourceLoadParameters.
    delete it->first;
    // erase the appropriate entry from the map.
    m_instance_map.erase(it);
}

} // end of namespace Xrb
