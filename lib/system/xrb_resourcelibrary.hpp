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

#include "xrb_resourceloadparameters.hpp"

namespace Xrb
{

template <typename T> class Resource;

/** Data associated with a path which is too large to store many times
  * should be managed by ResourceLibrary.  When a piece of data is loaded,
  * it is tracked by the ResourceLibrary, so that successive calls to load
  * the same path are transparent and return the existing data.  When
  * all references to a loaded piece of data are deleted, that piece of data
  * is deleted from memory.
  *
  * Resourced data is controlled by the templatized @ref Xrb::Resource object.
  * It provides the mechanisms required for reference-counting so the library
  * can unload unreferenced data.
  *
  * This is done by loading it using the templatized method LoadPath.
  * The template requires a static function which loads the data from the
  * given path and returns a pointer to whatever the templatized type is.
  * See the various classes which have a Create function (e.g.
  * @ref Xrb::GlTexture::Create and @ref Xrb::Font::Create), as these are
  * appropriate functions for the task.
  *
  * When the same path is loaded multiple times, each using different
  * load parameters, each counts as a separate resource.  Think about loading
  * a Font with different pixel heights.  The font data is actually an OpenGL
  * texture, and so different pixel heights must be separate textures, even
  * though the font face is the same.  However, loading the same path
  * <strong>and</strong> load parameters will use the same resourced data.
  *
  * The load parameters is a pointer value, and NULL is the default value,
  * which is convenient if you don't care about anything past the path itself.
  *
  * ResourceLibrary is a singleton object provided by @ref Xrb::Singleton.
  * It can be accessed using @ref Xrb::Singleton::ResourceLibrary.
  *
  * @brief Controls loading, storage, and unloading of resources.
  */
class ResourceLibrary
{
public:

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
      * which takes a path and a pointer to a ResourceLoadParameters instance,
      * returning a pointer to the newly allocated object of type T.
      * ResourceLibrary depends on the return value of LoadFunction being a
      * newly allocated object, so that it may handle deletion of the object
      * itself.
      *
      * @brief Loads a resource from the given path, using the supplied
      *        LoadFunction.
      * @param T Template argument giving the type of the resource to load.
      * @param LoadFunction Template argument giving the static or global
      *                     function to use to load the resource data.
      *                     The load function should not store the parameters
      *                     pointer value. 
      * @param path The path of the data to load using LoadFunction.
      * @param load_parameters The (optional) load parameters (a pointer to
      *                        an instance of a subclass of ResourceLoadParameters).
      *                        This should be a newly allocated instance (if
      *                        not NULL), and ResourceLibrary will take care
      *                        of deleting it.
      */
    template <typename T>
    Resource<T> LoadPath (
        T *(*LoadFunction)(std::string const &, ResourceLoadParameters const *),
        std::string const &path,
        ResourceLoadParameters const *load_parameters = NULL);

    /** @brief Prints a list of all currently loaded resources.
      */
    void PrintInventory (FILE *fptr, Uint32 tab_count = 0) const;

private:

    // used as the key in the resource instance map
    struct ResourceInstanceKey
    {
        // the resource's path
        std::string m_path;
        // the associated ResourceLoadParameters instance
        ResourceLoadParameters const *m_load_parameters;

        ResourceInstanceKey (
            std::string const &path,
            ResourceLoadParameters const *load_parameters)
        {
            m_path = path;
            m_load_parameters = load_parameters;
        }

        void Print (FILE *fptr) const
        {
            fprintf(fptr, "\"%s\", ", m_path.c_str());
            if (m_load_parameters == NULL)
                fprintf(stderr, "no load parameters");
            else
            {
                fprintf(stderr, "load parameters: ");
                m_load_parameters->Print(fptr);
            }
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

        ResourceInstanceKey const &Key () const { return m_key; }

        void IncrementReferenceCount ()
        {
            ASSERT1(m_reference_count < UINT32_UPPER_BOUND);
            ++m_reference_count;
        }
        bool DecrementReferenceCount ()
        {
            ASSERT1(m_reference_count > 0);
            --m_reference_count;
            if (m_reference_count == 0)
            {
                m_library->UnmapKey(m_key);
                DeleteData();
                return true;
            }
            else
                return false;
        }

    protected:

        ResourceInstanceBase (
            ResourceLibrary *const library,
            ResourceLibrary::ResourceInstanceKey const &key)
            :
            m_key(key)
        {
            ASSERT1(!key.m_path.empty());
            m_library = library;
            m_reference_count = 0;
        }
        virtual ~ResourceInstanceBase () { }

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

        T *Data () const
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

    InstanceMap m_instance_map;

    // so ResourceInstance<T> can call UnmapPath()
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
  * "invalid" and references to its data will cause an assert.  IsValid
  * queries the validity of a Resource object.  Resources can be made "valid"
  * simply by assigning to them the value of another Resource object or by
  * assigning to them the return value of a call to 
  * @ref Xrb::ResourceLibrary::LoadPath.
  *
  * The Release method is provided to explicitly unload data.  Once this
  * is done, the object is "invalid".
  *
  * Equality and inequality testing operators are provided, as well as
  * accessors for the resource's path and load parameter.
  *
  * Dereferencing is done in two ways.  The unary * operator returns a pointer
  * to the const resource data.  The -> operator is used intuitively as if
  * the resource object was the data pointer itself.  You can call methods
  * and reference members directly using the -> operator ( e.g.
  * @c font_resource->PixelHeight() ).  Dereferencing the Resource will
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
    Resource ()
    {
        m_instance = NULL;
    }
    /** You shouldn't use this constructor directly.  It is used when loading
      * already-loaded resources.
      * @brief Constructs a Resource using the ResourceLibrary-tracked
      *        instance of the data.
      */
    Resource (ResourceLibrary::ResourceInstance<T> *const instance)
    {
        ASSERT1(instance != NULL);
        m_instance = instance;
        m_instance->IncrementReferenceCount();
    }
    /** Resources constructed using the copy constructor cause the reference
      * count to increment.
      * @brief Copy constructor.
      */
    Resource (Resource<T> const &source)
    {
        m_instance = source.m_instance;
        if (m_instance != NULL)
            m_instance->IncrementReferenceCount();
    }
    /** If the Resource is valid, the reference count is decremented, which
      * causes the data to be unloaded if the count reaches zero.
      * @brief Destructor.
      */
    ~Resource ()
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
    bool operator == (Resource<T> const &operand) const
    {
        return m_instance == operand.m_instance;
    }
    /** Returns true iff the Resources do not reference the same data or if
      * exactly one of them is invalid.
      * both invalid.
      * @brief Inequality test operator.
      */
    bool operator != (Resource<T> const &operand) const
    {
        return m_instance != operand.m_instance;
    }
    /** Returns a pointer to the resourced data.
      * @brief Pointer dereferencing.
      */
    T const *operator * () const
    {
        ASSERT1(m_instance != NULL);
        return m_instance->Data();
    }
    /** Provides -> dereferencing syntax for the resourced data, e.g.
      * @c font_resource->PixelHeight() .
      * @brief Pointer dereferencing.
      */
    T const *operator -> () const
    {
        ASSERT1(m_instance != NULL);
        return m_instance->Data();
    }

    /** An invalid Resource can't be dereferenced or accessed for its
      * path or load parameter.
      * @brief Returns true iff this Resource is referencing data.
      */
    bool IsValid () const
    {
        return m_instance != NULL;
    }
    /** Asserts if this Resource is invalid.
      * @brief Returns the path that the resourced data was loaded from.
      */
    std::string const &Path () const
    {
        ASSERT1(m_instance != NULL);
        return m_instance->Key().m_path;
    }
    /** If this resource was loaded without a load parameter, then
      * NULL will be returned.
      *
      * Asserts if this Resource is invalid.
      *
      * @brief Returns the load parameters used when loading the
      *        resourced data.
      */
    ResourceLoadParameters const *LoadParameters () const
    {
        ASSERT1(m_instance != NULL);
        return m_instance->Key().m_load_parameters;
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

template <typename T>
Resource<T> ResourceLibrary::LoadPath (
    T *(*LoadFunction)(std::string const &, ResourceLoadParameters const *),
    std::string const &path,
    ResourceLoadParameters const *load_parameters)
{
    Resource<T> invalid_resource;

    if (path.empty())
        return invalid_resource;

    ResourceInstanceKey const key(path, load_parameters);

    // check if the file is already loaded
    InstanceMap::iterator it = m_instance_map.find(key);
    // if it is loaded then return a Resource for it.
    if (it != m_instance_map.end())
    {
        ASSERT1(it->second != NULL);
        // make sure the existing data is of the right type
        ASSERT1(
            dynamic_cast<ResourceInstance<T> *>(it->second) != NULL &&
            "You probably are trying to load a currently loaded resource "
            "using a different type or method, which is a big no-no");
        // since a matching load_parameters is already stored in the
        // instance key, delete the one passed in.
        delete load_parameters;
        // return the loaded resource.
        return Resource<T>(dynamic_cast<ResourceInstance<T> *>(it->second));
    }
    // otherwise load up the given path, stick it in
    // the map and return a Resource for it.
    else
    {
        // attempt to load the path
        T *data = LoadFunction(path, load_parameters);

        // if successful, stick the data into a ResourceInstance to
        // be kept by this ResourceLibrary, otherwise return null.
        if (data != NULL)
        {
            fprintf(stderr, "ResourceLibrary * loading ");
            key.Print(stderr);
            fprintf(stderr, "\n");

            ResourceInstance<T> *instance =
                new ResourceInstance<T>(this, key, data);
            m_instance_map[key] = instance;
            // load_parameters is now stored in the instance map (via the key),
            // so don't delete it (it will be deleted in UnmapKey).
            return Resource<T>(instance);
        }
        else
        {
            fprintf(stderr, "ResourceLibrary * FAILURE while loading ");
            key.Print(stderr);
            fprintf(stderr, "\n");
            // load_parameters went unused, so delete it.
            delete load_parameters;
            // do some error handling
            return invalid_resource;
        }
    }
}

} // end of namespace Xrb

#endif // !defined(_XRB_RESOURCELIBRARY_HPP_)

