// ///////////////////////////////////////////////////////////////////////////
// xrb_mapeditor2_object.hpp by Victor Dods, created 2005/07/03
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_MAPEDITOR2_OBJECT_HPP_)
#define _XRB_MAPEDITOR2_OBJECT_HPP_

#include "xrb.hpp"

#include "xrb_engine2_object.hpp"

namespace Xrb
{

namespace Engine2
{
    class EntityGuts;
} // end of namespace Engine2

namespace MapEditor2
{

    class Entity;
    class ObjectLayer;
    class VisibilityQuadTree;
    
    class Object : public virtual Engine2::Object
    {
    public:
    
        enum SelectionOperation
        {
            SO_EQUALS = 0, // set the selection to the new set
            SO_IOR,        // inclusive OR operation between the sets
            SO_MINUS,      // subtract the new set from the selection
            SO_XOR,        // exclusive OR operation between the sets
            SO_AND,        // AND operation between the sets
    
            SO_COUNT
        }; // end of enum MapEditor2::Object::SelectionOperation
    
        enum TransformationMode
        {
            TM_GLOBAL_ORIGIN_CURSOR = 0,    // using the origin cursor as the origin
            TM_SELECTION_SET_ORIGIN,        // using the selected set's origin
            TM_EACH_SELECTED_OBJECT_ORIGIN, // using each selected object's origin
    
            TM_COUNT
        }; // end of enum MapEditor2::Object::TransformationMode
    
        enum MetricMode
        {
            MM_TRANSFORMATION = 0, // objects' positions/scales/rotations
            MM_LINEAR_VELOCITY,    // entities' linear velocities
            MM_ANGULAR_VELOCITY,   // entities' angular velocities
            MM_POLYGONS,           // compounds' polygons
            MM_VERTICES,           // compounds' vertices
    
            MM_COUNT
        }; // end of enum MapEditor2::Object::MetricMode
    
        virtual ~Object ();

        // ///////////////////////////////////////////////////////////////////
        // public serialization functions
        // ///////////////////////////////////////////////////////////////////
        
        // creates a new Object from the serializer.  this is the main
        // multiplexing function which creates whatever type of map editor
        // object subclass is actually stored in the serializer.
        static Object *Create (Serializer &serializer);

        // ///////////////////////////////////////////////////////////////////
        // public overridable methods
        // ///////////////////////////////////////////////////////////////////
        
        // creates a cloned version of this object
        virtual Engine2::Object *CreateClone () const = 0;
        // creates an entity version clone of this object, or NULL if
        // it either already is an entity, or if not applicable.
        virtual Entity *CreateEntityClone () const = 0;
        // creates a non-entity version clone of this object, or NULL if
        // it either already is a non-entity, or if not applicable.
        virtual Object *CreateNonEntityClone () const = 0;
        // draws the bounding circle, angle limit lines and angle arc
        virtual void DrawMetrics (
            DrawData const &draw_data,
            Float alpha_mask,
            MetricMode metric_mode);
    
        // ///////////////////////////////////////////////////////////////////
        // public accessors and modifiers
        // ///////////////////////////////////////////////////////////////////
    
        ObjectLayer *GetMapEditorObjectLayer () const;
        VisibilityQuadTree *GetOwnerMapEditorQuadTree () const;
        // returns the object's selection status
        inline bool GetIsSelected () const
        {
            return m_is_selected;
        }
        // returns the object's saved EntityGuts
        inline Engine2::EntityGuts *GetSavedEntityGuts () const
        {
            return m_saved_entity_guts;
        }
    
        virtual Color const &GetUnselectedMetricsColor () const;
        virtual Color const &GetSelectedMetricsColor () const;
    
        // sets the selection status for this object
        void SetIsSelected (bool const is_selected);
        // sets this object's saved EntityGuts
        inline void SetSavedEntityGuts (Engine2::EntityGuts *saved_entity_guts)
        {
            ASSERT1(m_saved_entity_guts == NULL);
            m_saved_entity_guts = saved_entity_guts;
        }

        // ///////////////////////////////////////////////////////////////////
        // public procedures
        // ///////////////////////////////////////////////////////////////////
            
        // toggles this object's selection state
        void ToggleIsSelected ();
        // applies the current selection operation to this object
        void ApplyObjectSelectionOperation (
            SelectionOperation selection_operation,
            bool object_is_in_operand_set);
        // scales this object using the given transformation mode and origin,
        // as part of a transformation being applied to a set of objects.
        void ObjectSelectionSetScale (
            Float scale_factor,
            FloatVector2 transformation_origin,
            TransformationMode transformation_mode);
        // rotates this object using the given transformation mode and origin,
        // as part of a transformation being applied to a set of objects.
        void ObjectSelectionSetRotate (
            Float angle_delta,
            FloatVector2 transformation_origin,
            TransformationMode transformation_mode,
            FloatMatrix2 const &rotation_transformation);

        // moves an object which has changed position/size to the correct
        // quadnode, using its current quadnode as the starting point
        // for high efficiency.        
        void ReAddMapEditorObjectToQuadTree (Engine2::QuadTreeType quad_tree_type);
                
    protected:

        // protected constructor so you must use Create()
        Object ();
        
        // tells this object's owner to add it to the object selection set
        void AddToObjectSelectionSet ();
        // tells this object's owner to remove it from the object selection set
        void RemoveFromObjectSelectionSet ();

        // the selection status
        bool m_is_selected;
        // this value is used to store the m_guts pointer when
        // an entity is converted into an object.
        Engine2::EntityGuts *m_saved_entity_guts;
    
    private:
    
        // the color of the object metrics when not selected
        static Color ms_unselected_metrics_color;
        // the color of the object metrics when selected
        static Color ms_selected_metrics_color;
    }; // end of class MapEditor2::Object

} // end of namespace MapEditor2

} // end of namespace Xrb
    
#endif // !defined(_XRB_MAPEDITOR2_OBJECT_HPP_)

