// ///////////////////////////////////////////////////////////////////////////
// xrb_resourcelibrary.hpp by Victor Dods, created 2005/06/05
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_RESOURCELIBRARY_HPP_)
#define _XRB_RESOURCELIBRARY_HPP_

#include "xrb.hpp"

#include <map>
#include <string>

namespace Xrb
{

template <typename T> class Resource;

/** Data associated with a filename which is too large to store many times
  * should be managed by ResourceLibrary.  When a piece of data is loaded,
  * it is tracked by the ResourceLibrary, so that successive calls to load
  * the same filename are transparent and return the existing data.  When
  * all references to a loaded piece of data are deleted, that piece of data
  * is deleted from memory.
  *
  * Resourced data is controlled by the templatized @ref Xrb::Resource object.
  * It provides the mechanisms required for reference-counting so the library
  * can unload unreferenced data.
  *
  * This is done by loading it using the templatized method LoadFilename.
  * The template requires a static function which loads the data from the
  * given filename and returns a pointer to whatever the templatized type is.
  * See the various classes which have a Create function (e.g.
  * @ref Xrb::GLTexture::Create and @ref Xrb::Font::Create), as these are
  * appropriate functions for the task.
  *
  * There is a second version of the templatized LoadFilename method
  * which indicates that the load function also takes a Sint32 as an
  * additional parameter.  This value can be used to specify additional data
  * necessary to load the resource in addition to the filename (see
  * @ref Xrb::Font, where the additional parameter is used for the pixel
  * height for the rendered font data).
  *
  * When the same filename is loaded multiple times, each using a different
  * load parameter, each counts as a separate resource.  Think about loading
  * a Font with different pixel heights.  The font data is actually an OpenGL
  * texture, and so different pixel heights must be separate textures, even
  * though the font face is the same.  However, loading the same filename
  * <strong>and</strong> load parameter will use the same resourced data.
  *
  * It should be noted that the value @c SINT32_LOWER_BOUND ( aka
  * @c UNSPECIFIED_LOAD_PARAMETER ) is reserved and should not be used by the
  * given load function.
  *
  * ResourceLibrary is a singleton object provided by @ref Xrb::Singletons.
  * It can be accessed using @ref Xrb::Singletons::ResourceLibrary.
  *
  * @brief Controls loading, storage, and unloading of resources.
  */
class ResourceLibrary
{
public:

    enum
    {
        UNSPECIFIED_LOAD_PARAMETER = static_cast<Sint32>(SINT32_LOWER_BOUND)
    };

    /** @brief Constructor
      */
    ResourceLibrary ();
    /** Asserts if there are any remaining resources.  This should not happen
      * due to the scope-based life of Resource objects.  If it does happen,
      * it's likely that a dynamically-allocated object that contains a
      * Resource object was not deleted.
      * @brief Destructor.
      */
    ~ResourceLibrary ();

    /** The template argument T must be the type of data being loaded.  The
      * template argument LoadFunction must be a static (or global) function
      * which takes a filename, and returns a pointer to the newly allocated
      * object of type T.  ResourceLibrary depends on the return value of
      * LoadFunction being a newly allocated object, so that it may handle
      * deletion of the object itself.
      * @brief Loads a resource from the given filename, using the supplied
      *        LoadFunction.
      * @param T Template argument giving the type of the resource to load.
      * @param LoadFunction Template argument giving the static or global
      *                     function to use to load the resource data.
      * @param filename The filename of the data to load using LoadFunction.
      */
    template <typename T>
    Resource<T> LoadFilename (
        T *(*LoadFunction)(std::string const &),
        std::string const &filename);
    /** The template argument T must be the type of data being loaded.  The
      * template argument LoadFunction must be a static (or global) function
      * which takes a filename, and returns a pointer to the newly allocated
      * object of type T.  ResourceLibrary depends on the return value of
      * LoadFunction being a newly allocated object, so that it may handle
      * deletion of the object itself.
      * @brief Loads a resource from the given filename, using the supplied
      *        LoadFunction, with load_parameter as a single extra parameter
      *        to LoadFunction.
      * @param T Template argument giving the type of the resource to load.
      * @param LoadFunction Template argument giving the static or global
      *                     function to use to load the resource data, which
      *                     also takes a Sint32 as the additional load
      *                     parameter.
      * @param filename The filename of the data to load using LoadFunction.
      * @param load_parameter The extra parameter to pass to LoadFunction.
      */
    template <typename T>
    Resource<T> LoadFilename (
        T *(*LoadFunction)(std::string const &, Sint32),
        std::string const &filename,
        Sint32 load_parameter);

private:

    // used as the key in the resource instance map
    struct ResourceInstanceKey
    {
        // the resource's filename
        std::string m_filename;
        // an associated signed integer value (e.g. font pixel height)
        Sint32 m_load_parameter;

        inline ResourceInstanceKey (
            std::string const &filename,
            Sint32 const load_parameter)
        {
            m_filename = filename;
            m_load_parameter = load_parameter;
        }

        /// @cond DOXYGEN_EXCLUDE_CODE
        // i don't want this to show up in the docs as a class, since
        // i have doxygen exclude all private declarations.
        struct LessThan
        {
            bool operator () (
                ResourceInstanceKey const &left_operand,
                ResourceInstanceKey const &right_operand) const;
        }; // end of struct LessThan
        /// @endcond
    }; // end of struct ResourceInstanceKey

    void UnmapKey (ResourceInstanceKey const &key);

    // only used for storing pointers to ResourceInstance<T> in
    // an array, for proper casting and type safety.
    class ResourceInstanceBase
    {
    public:

        inline std::string const &GetFilename () const
        {
            return m_key.m_filename;
        }
        inline Sint32 GetLoadParameter () const
        {
            return m_key.m_load_parameter;
        }

        inline void IncrementReferenceCount ()
        {
            ASSERT1(m_reference_count < UINT32_UPPER_BOUND);
            ++m_reference_count;
        }
        inline bool DecrementReferenceCount ()
        {
            ASSERT1(m_reference_count > 0);
            --m_reference_count;
            if (m_reference_count == 0)
            {
                m_library->UnmapKey(m_key);
//                 fprintf(stderr, "ResourceLibrary * unloading ");
//                 Print(stderr);
                DeleteData();
                return true;
            }
            else
                return false;
        }

        void Print (FILE *fptr) const
        {
            fprintf(fptr, "\"%s\", ", m_key.m_filename.c_str());
            if (m_key.m_load_parameter == UNSPECIFIED_LOAD_PARAMETER)
                fprintf(stderr, "no load parameter\n");
            else
                fprintf(stderr, "load parameter = %d\n", m_key.m_load_parameter);
        }
                
    protected:
            
        inline ResourceInstanceBase (
            ResourceLibrary *const library,
            ResourceLibrary::ResourceInstanceKey const &key)
            :
            m_key(key)
        {
            ASSERT1(!key.m_filename.empty());
            m_library = library;
            m_reference_count = 0;
        }
        inline virtual ~ResourceInstanceBase () { }
    
        virtual void DeleteData () = 0;
    
        ResourceInstanceKey m_key;
        ResourceLibrary *m_library;
        Uint32 m_reference_count;
    }; // end of class ResourceInstanceBase

    // actually holds the reference-counted resource data
    template <typename T>
    class ResourceInstance : public ResourceInstanceBase
    {
    public:

        ResourceInstance (
            ResourceLibrary *const library,
            ResourceLibrary::ResourceInstanceKey const &key,
            T *const data)
            :
            ResourceInstanceBase(library, key)
        {
            ASSERT1(data != NULL);
            m_data = data;
        }
        ~ResourceInstance ()
        {
            ASSERT1(m_data == NULL);
        }

        inline T *GetData () const
        {
            return m_data;
        }

    protected:

        virtual void DeleteData ()
        {
            DeleteAndNullify(m_data);
        }
        
    private:

        T *m_data;
    }; // end of class ResourceInstance<T>

    typedef std::map<
        ResourceInstanceKey,
        ResourceInstanceBase *,
        ResourceInstanceKey::LessThan> InstanceMap;
    typedef InstanceMap::iterator InstanceMapIterator;

    InstanceMap m_instance_map;

    // so ResourceInstance<T> can call UnmapFilename()
    template <typename T> friend class ResourceInstance;
    // so Resource<T> can use ResourceInstance<T>
    template <typename T> friend class Resource;
}; // end of class ResourceLibrary

/** Resource should be used as a normal, non-dynamically-allocated object,
  * and can be assigned by value when making copies of the resource (though
  * it should generally be passed by reference when used as a parameter in
  * functions).
  *
  * The assignment operator is overridden to provide intuitive and convenient
  * cloning of resourced data.  Assignment causes the reference count to
  * increase, so that the newly assigned resource will remain valid as long
  * as it is in scope.
  *
  * A Resource object which has not been assigned an actual resource will be
  * "invalid" and references to its data will cause an assert.  GetIsValid
  * queries the validity of a Resource object.  Resources can be made "valid"
  * simply by assigning to them the value of another Resource object or by
  * assigning to them the return value of a call to 
  * @ref Xrb::ResourceLibrary::LoadFilename.
  *
  * The Release method is provided to explicitly unload data.  Once this
  * is done, the object is "invalid".
  *
  * Equality and inequality testing operators are provided, as well as
  * accessors for the resource's filename and load parameter.
  *
  * Dereferencing is done in two ways.  The unary * operator returns a pointer
  * to the const resource data.  The -> operator is used intuitively as if
  * the resource object was the data pointer itself.  You can call methods
  * and reference members directly using the -> operator ( e.g.
  * @c font_resource->GetPixelHeight() ).  Dereferencing the Resource will
  * assert if it is invalid.
  *
  * @brief Reference-counting container for resourced data.
  */
template <typename T>
class Resource
{
public:

    /** Constructs an invalid resource (not referencing anything).
      * @brief Default constructor
      */
    inline Resource ()
    {
        m_instance = NULL;
    }
    /** You shouldn't use this constructor directly.  It is used when loading
      * already-loaded resources.
      * @brief Constructs a Resource using the ResourceLibrary-tracked
      *        instance of the data.
      */
    inline Resource (ResourceLibrary::ResourceInstance<T> *const instance)
    {
        ASSERT1(instance != NULL);
        m_instance = instance;
        m_instance->IncrementReferenceCount();
    }
    /** Resources constructed using the copy constructor cause the reference
      * count to increment.
      * @brief Copy constructor.
      */
    inline Resource (Resource<T> const &source)
    {
        m_instance = source.m_instance;
        if (m_instance != NULL)
            m_instance->IncrementReferenceCount();
    }
    /** If the Resource is valid, the reference count is decremented, which
      * causes the data to be unloaded if the count reaches zero.
      * @brief Destructor.
      */
    inline ~Resource ()
    {
        if (m_instance != NULL)
        {
            if (m_instance->DecrementReferenceCount())
                DeleteAndNullify(m_instance);
        }
    }

    /** Resources assigned to using the assignment operator cause the
      * reference count to increment.
      * @brief Assignment operator.
      */
    void operator = (Resource<T> const &source)
    {
        if (m_instance != NULL)
            if (m_instance->DecrementReferenceCount())
                Delete(m_instance);
        m_instance = source.m_instance;
        if (m_instance != NULL)
            m_instance->IncrementReferenceCount();
    }
    /** Returns true iff the Resources reference the same data or they are
      * both invalid.
      * @brief Equality test operator.
      */
    inline bool operator == (Resource<T> const &operand) const
    {
        return m_instance == operand.m_instance;
    }
    /** Returns true iff the Resources do not reference the same data or if
      * exactly one of them is invalid.
      * both invalid.
      * @brief Inequality test operator.
      */
    inline bool operator != (Resource<T> const &operand) const
    {
        return m_instance != operand.m_instance;
    }
    /** Returns a pointer to the resourced data.
      * @brief Pointer dereferencing.
      */
    inline T const *operator * () const
    {
        ASSERT1(m_instance != NULL);
        return m_instance->GetData();
    }
    /** Provides -> dereferencing syntax for the resourced data, e.g.
      * @c font_resource->GetPixelHeight() .
      * @brief Pointer dereferencing.
      */
    inline T const *operator -> () const
    {
        ASSERT1(m_instance != NULL);
        return m_instance->GetData();
    }

    /** An invalid Resource can't be dereferenced or accessed for its
      * filename or load parameter.
      * @brief Returns true iff this Resource is referencing data.
      */
    inline bool GetIsValid () const
    {
        return m_instance != NULL;
    }
    /** Asserts if this Resource is invalid.
      * @brief Returns the filename that the resourced data was loaded from.
      */
    inline std::string const &GetFilename () const
    {
        ASSERT1(m_instance != NULL);
        return m_instance->GetFilename();
    }
    /** If this resource was loaded without a load parameter, then
      * @ref Xrb::ResourceLibrary::UNSPECIFIED_LOAD_PARAMETER will be
      * returned. 
      *
      * Asserts if this Resource is invalid.
      *
      * @brief Returns the load parameter that was used when loading the
      *        resourced data.
      */
    inline Sint32 GetLoadParameter () const
    {
        ASSERT1(m_instance != NULL);
        return m_instance->GetLoadParameter();
    }

    /** @brief Explicitly causes the reference count to decrement, causing
      *        this Resource to become invalid.
      */
    void Release ()
    {
        if (m_instance != NULL)
        {
            if (m_instance->DecrementReferenceCount())
                DeleteAndNullify(m_instance);
        }
    }

private:

    ResourceLibrary::ResourceInstance<T> *m_instance;
}; // end of class Resource<T>

#include "xrb_resourcelibrary.tcpp"

} // end of namespace Xrb

#endif // !defined(_XRB_RESOURCELIBRARY_HPP_)

