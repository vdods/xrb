// ///////////////////////////////////////////////////////////////////////////
// xrb_filesystem.hpp by Victor Dods, created 2011/09/22
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_FILESYSTEM_HPP_)
#define _XRB_FILESYSTEM_HPP_

#include "xrb.hpp"

#include <vector>

#include "xrb_exception.hpp"

namespace Xrb {

/// @brief Provides a directory-based search path for locating files in the layered fashion
/// which is common to moddable games -- the later-added search paths are higher in search priority.
/// @details A path returned by the @c OsPath method will be referred to as an "OS path" (i.e.
/// platform-specific), whereas a path that is passed to the @c OsPath method will be referred
/// to as a "FS path" (filesystem path) (i.e. referring to a file in this FileSystem object).
/// This terminology will be used when specifying which type of path is used in a particular
/// function or method.
class FileSystem
{
public:

    enum { WRITABLE = true, READ_ONLY = false };
    
    /// Returns true iff the search path is empty.
    bool IsEmpty () const { return m_path_stack.empty(); }
    /// If the path argument begins with "fs://" (case sensitive), this method returns the corresponding OS path, throwing an exception
    /// if the path search gave no results, or there was an error.  If the writable flag is set, then the search path
    /// will be restricted to directories that were set is_writable in AddDirectory.  If the path argument does not
    /// begin with "fs://" then the supplied path is returned unchanged.
    std::string OsPath (std::string const &path, bool writable) const;

    /// Add a directory (OS path) to the top of the path stack (i.e. at the highest priority), throwing an exception
    /// in the case of error.  This method doesn't check if the directory is actually writable from an OS standpoint,
    /// the is_writable flag indicates if the directory allows files within to be written.
    void AddDirectory (std::string os_path, bool is_writable, std::string const &set_by);

    /// Returns a string containing the search path directories, from highest search priority to lowest,
    /// each delimited by the given delimiter.  No delimiter follows the last entry.
    std::string AsString (std::string const &delimiter = ", ") const { return AsString_Private(delimiter, NON_VERBOSE); }
    /// Same as AsString, except each entry is followed by its "set by" string.
    std::string AsVerboseString (std::string const &delimiter = "\n") const { return AsString_Private(delimiter, VERBOSE); }
    
private:

    enum Verbosity { NON_VERBOSE, VERBOSE };

    std::string AsString_Private (std::string const &delimiter, Verbosity verbosity) const;

    class PathEntry
    {
    public:

        PathEntry (std::string const &path, bool is_writable, std::string const &set_by)
            :
            m_path(path),
            m_is_writable(is_writable),
            m_set_by(set_by)
        {
            ASSERT1(!m_path.empty());
            ASSERT1(!m_set_by.empty());
        }

        std::string const &Path () const { return m_path; }
        bool IsWritable () const { return m_is_writable; }
        std::string const &SetBy () const { return m_set_by; }

    private:

        std::string m_path;
        bool m_is_writable;
        std::string m_set_by;
    }; // end of class FileSystem::PathEntry

    typedef std::vector<PathEntry> PathEntryStack;

    PathEntryStack m_path_stack;
}; // end of class FileSystem

} // end of namespace Xrb

#endif // !defined(_XRB_FILESYSTEM_HPP_)

