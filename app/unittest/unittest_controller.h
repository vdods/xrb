// ///////////////////////////////////////////////////////////////////////////
// unittestcontroller.h by Victor Dods, created 2005/05/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_UNITTESTCONTROLLER_H_)
#define _UNITTESTCONTROLLER_H_

#include "../defines.h"

#include <map>
#include <string>

class UnitTest;

class UnitTestController
{
public:

    UnitTestController ();
    ~UnitTestController ();

    inline Uint32 GetFailureCount () const { return m_failure_count; }

    // this must be called before GenerateCommandLineOptions(),
    // PrintHelpMessage() or Test() are called
    void RegisterUnitTest (UnitTest *unit_test);
    // this must be called before PrintHelpMessage() or Test()
    // are called
    void GenerateCommandLineOptions ();
    void PrintHelpMessage (char const *executable_name);

    void RunTests (int argc, char **argv);

private:

    typedef std::map<std::string, UnitTest *> UnitTestMap;
    typedef UnitTestMap::iterator UnitTestMapIterator;

//     typedef std::list<std::string> OptionStringList;
//     typedef OptionStringList::iterator OptionStringListIterator;

    UnitTestMap m_unit_test_map;
//     OptionStringList m_option_string_list;
    Uint32 m_failure_count;
    bool m_command_line_options_generated;
}; // end of class UnitTestController

#endif // !defined(_UNITTESTCONTROLLER_H_)

