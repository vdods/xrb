// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_compound.hpp by Victor Dods, created 2005/06/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_COMPOUND_HPP_)
#define _XRB_ENGINE2_COMPOUND_HPP_

#include "xrb.hpp"

#include "xrb_engine2_object.hpp"
#include "xrb_gltexture.hpp"
#include "xrb_resourcelibrary.hpp"

namespace Xrb {
namespace Engine2 {

struct Polygon;

class Compound : public Object
{
public:

    virtual ~Compound ();

    // ///////////////////////////////////////////////////////////////////
    // public serialization functions
    // ///////////////////////////////////////////////////////////////////

    static Compound *Create (Serializer &serializer);
    // makes calls to WriteClassSpecific for this and all superclasses
    virtual void Write (Serializer &serializer) const;

    // ///////////////////////////////////////////////////////////////////
    // public Object interface methods
    // ///////////////////////////////////////////////////////////////////

    // draws the compound
    virtual void Draw (
        DrawData const &draw_data,
        Float alpha_mask) const;

protected:

    // protected constructor so you must use Create()
    Compound ();

    // ///////////////////////////////////////////////////////////////////
    // protected serialization functions
    // ///////////////////////////////////////////////////////////////////

    // does the guts of serializing reading for this class (doesn't read
    // the object subtype)
    void ReadClassSpecific (Serializer &serializer);
    // does the guts of serializing writing for this class (doesn't write
    // the object subtype)
    void WriteClassSpecific (Serializer &serializer) const;

    // ///////////////////////////////////////////////////////////////////
    // protected Object interface methods
    // ///////////////////////////////////////////////////////////////////

    virtual void CalculateRadius (QuadTreeType quad_tree_type) const;

    // ///////////////////////////////////////////////////////////////////

    // copies the properties of the given object (compound) to this compound
    void CloneProperties (Object const *object);

    // move this geometry into CompoundGeometry (or some such)
    // so that it can be ResourceLibrary'd

    Uint32 m_vertex_count;
    FloatVector2 *m_vertex_array;
    Uint32 m_polygon_count;
    Polygon *m_polygon_array;
}; // end of class Engine2::Compound

} // end of Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_COMPOUND_HPP_)

