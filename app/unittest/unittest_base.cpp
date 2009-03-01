// ///////////////////////////////////////////////////////////////////////////
// unittest.cpp by Victor Dods, created 2005/05/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "unittest.hpp"

#include <stdarg.h>

void UnitTest::InitializeScaffold ()
{
    m_failure_count = 0;
}

void UnitTest::Run ()
{
    for (AtomListIterator it = m_atom_list.begin(),
                          it_end = m_atom_list.end();
         it != it_end;
         ++it)
    {
        try
        {
            UnitTestAtom atom = *it;
            (this->*atom)();
        }
        catch (Uint32)
        {
            // do nothing -- we just want the behavior of immediately
            // escaping out of function calls of arbitrary stack depth.
        }
        printf("\n");
    }
}

void UnitTest::Test (
    bool condition,
    char const *description,
    ...) const throw()
{
    printf("%s", condition ? "PASS $$$ " : "FAIL !!! ");

    va_list list;
    va_start(list, description);
    vprintf(description, list);
    va_end(list);

    printf("\n");

    if (!condition)
        ++m_failure_count;
}

void UnitTest::TestCritical (
    bool condition,
    char const *description,
    ...) const throw(Uint32)
{
    printf("%s", condition ? "PASS $$$ " : "FAIL (CRITICAL) !!! ");

    va_list list;
    va_start(list, description);
    vprintf(description, list);
    va_end(list);

    printf("\n");

    if (!condition)
    {
        ++m_failure_count;
        throw static_cast<Uint32>(0);
    }
}

void UnitTest::PrintSubsectionHeader (char const *description, ...)
{
    printf("\t");

    va_list list;
    va_start(list, description);
    vprintf(description, list);
    va_end(list);

    printf("\n");
}
