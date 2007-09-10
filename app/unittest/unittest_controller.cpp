// ///////////////////////////////////////////////////////////////////////////
// unittestcontroller.cpp by Victor Dods, created 2005/05/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "unittestcontroller.h"

#include "unittest.h"

UnitTestController::UnitTestController ()
    :
    m_failure_count(0),
    m_command_line_options_generated(false)
{
}

UnitTestController::~UnitTestController ()
{
    // delete all the tests
    for (UnitTestMapIterator it = m_unit_test_map.begin(),
                             it_end = m_unit_test_map.end();
         it != it_end;
         ++it)
    {
        UnitTest *unit_test = it->second;
        DELETE_AND_ZERO(unit_test)
    }
    m_unit_test_map.clear();
}

void UnitTestController::RegisterUnitTest (UnitTest *unit_test)
{
    ASSERT1(unit_test != NULL);
    ASSERT1(!m_command_line_options_generated && "You must not call GenerateCommandLineOptions() before any call to RegisterUnitTest()");

    UnitTestMapIterator it = m_unit_test_map.find(unit_test->GetName());
    UnitTestMapIterator it_end = m_unit_test_map.end();
    ASSERT1(it == it_end && "You may not register the same test twice");
    m_unit_test_map[unit_test->GetName()] = unit_test;
}

void UnitTestController::GenerateCommandLineOptions ()
{
    m_command_line_options_generated = true;
}

void UnitTestController::PrintHelpMessage (char const *executable_name)
{
    fprintf(stderr, "Usage: %s (blah... not done yet)\n", executable_name);
}

void UnitTestController::RunTests (int argc, char **argv)
{
    ASSERT1(m_command_line_options_generated && "You must call GenerateCommandLineOptions() before any call to RunTests()");

    m_failure_count = 0;

    for (UnitTestMapIterator it = m_unit_test_map.begin(),
                             it_end = m_unit_test_map.end();
         it != it_end;
         ++it)
    {
        UnitTest *unit_test = it->second;
        ASSERT1(unit_test != NULL);
        printf("//////////////////////////////////////////////////////////////////////////////\n");
        printf("// Running test \"%s\"\n", unit_test->GetName().c_str());
        printf("//////////////////////////////////////////////////////////////////////////////\n\n");
        unit_test->InitializeScaffold();
        unit_test->Run();
        m_failure_count += unit_test->GetFailureCount();
        if (unit_test->GetFailureCount() > 0)
            printf("\nTest \"%s\" completed with %u failures.\n\n\n",
                   unit_test->GetName().c_str(),
                   unit_test->GetFailureCount());
        else
            printf("\nTest \"%s\" passed all tests.\n\n\n",
                   unit_test->GetName().c_str());
    }

    printf("//////////////////////////////////////////////////////////////////////////////\n");
    printf("// Summary\n");
    printf("//////////////////////////////////////////////////////////////////////////////\n\n");

    printf("Total failure count: %u\n", m_failure_count);
}

