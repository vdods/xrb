// ///////////////////////////////////////////////////////////////////////////
// xrb_filesystem.cpp by Victor Dods, created 2011/09/22
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_filesystem.hpp"

#include "xrb_pal.hpp"
#include "xrb_util.hpp"

namespace Xrb {

std::string FileSystem::OsPath (std::string const &path, bool writable) const throw(FileSystemException)
{
    if (path.find("fs://") != 0)
        return path; // no search path lookup.

    std::string fs_path(path.substr(5)); // 5 is the length of the string "fs://"
    
    if (fs_path.empty())
        throw FileSystemException(fs_path, "invalid argument -- fs_path is empty");
    
    std::cerr << "FileSystem::OsPath(); looking for " << (writable ? "writable" : "readable") << " file \"" << fs_path << "\" in filesystem..." << std::endl;
    Uint32 relevant_directory_count = 0;
    for (PathEntryStack::const_reverse_iterator it = m_path_stack.rbegin(), it_end = m_path_stack.rend(); it != it_end; ++it)
    {
        PathEntry const &path_entry = *it;
        // if we're searching only writable directories, skip non-writable directories.
        if (writable && !path_entry.IsWritable())
            continue;
        
        ++relevant_directory_count;
        std::string file_os_path(path_entry.Path() + fs_path);

        if (writable)
        {
            std::cerr << "FileSystem::OsPath(); OS path \"" << file_os_path << "\" is being returned for writable-requested file \"" << fs_path << "\"" << std::endl;
            return file_os_path;
        }
        
        if (Singleton::Pal().FileExists(file_os_path.c_str()))
        {
            std::cerr << "FileSystem::OsPath(); file \"" << fs_path << "\" was found in directory \"" + path_entry.Path() + "\"" << std::endl;
            return file_os_path;
        }
        else
            std::cerr << "FileSystem::OsPath(); file \"" << fs_path << "\" was not found in directory \"" << path_entry.Path() + "\"" << std::endl;
    }
    if (relevant_directory_count == 0)
        throw FileSystemException(path, "file not found -- no " + std::string(writable ? "writable" : "") + " directories in filesystem search fs_path");
    else
        throw FileSystemException(path, "file not found");

    // gcc was complaining about a non-void function not returning a value.
    ASSERT0(false && "this should never happen"); // because of the above throw statements.
    return std::string();
}

void FileSystem::AddDirectory (std::string os_path, bool is_writable, std::string const &set_by) throw(FileSystemException)
{
    if (os_path.empty())
        throw FileSystemException(os_path, "invalid argument -- directory path is empty");

    // make sure there's a slash at the end of the os_path.
    if (*os_path.rbegin() != '/')
        os_path += '/';

    // only add the os_path if it actually exists.
    if (!Singleton::Pal().DirectoryExists(os_path.c_str()))
        throw FileSystemException(os_path, "directory does not exist");

    // add the os_path to the top of the stack
    m_path_stack.push_back(PathEntry(os_path, is_writable, set_by));
}

std::string FileSystem::AsString_Private (std::string const &delimiter, FileSystem::Verbosity verbosity) const
{
    std::string path_string;
    for (PathEntryStack::const_reverse_iterator it = m_path_stack.rbegin(), it_end = m_path_stack.rend(); it != it_end; ++it)
    {
        PathEntry const &path_entry = *it;
        path_string += Util::StringLiteral(path_entry.Path());
        if (verbosity == VERBOSE)
        {
            path_string += ", is_writable = ";
            path_string += BOOL_TO_STRING(path_entry.IsWritable());
            path_string += " (";
            path_string += path_entry.SetBy();
            path_string += ')';
        }
        // only add the delimiter if there's another path to iterate over.
        PathEntryStack::const_reverse_iterator next_it = it;
        ++next_it;
        if (next_it != it_end)
            path_string += delimiter;
    }
    return path_string;
}

} // end of namespace Xrb
