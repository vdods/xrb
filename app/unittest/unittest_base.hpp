// ///////////////////////////////////////////////////////////////////////////
// unittest.hpp by Victor Dods, created 2005/05/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_UNITTEST_HPP_)
#define _UNITTEST_HPP_

#include "../defines.hpp"

#include <list>
#include <string>

// a bit of jargon definition: "atom" will mean a single test which can
// be executed independently from others (i.e. does not rely on the results
// from other atoms).

class UnitTest
{
public:

    UnitTest (std::string const &name)
        :
        m_name(name),
        m_failure_count(0)
    { }
    virtual ~UnitTest () { }

    inline std::string const &GetName () const { return m_name; }
    inline Uint32 GetFailureCount () const { return m_failure_count; }

    virtual void Initialize () = 0;
    void InitializeScaffold ();
    void Run ();

protected:

    void Test (
        bool condition,
        char const *description,
        ...) const throw();
    void TestCritical (
        bool condition,
        char const *description,
        ...) const throw(Uint32);

    typedef void (UnitTest::*UnitTestAtom)();

    template <typename UnitTestSubclass>
    void RegisterAtom (void (UnitTestSubclass::*atom)())
    {
        ASSERT1(atom != NULL);
        m_atom_list.push_back(static_cast<UnitTestAtom>(atom));
    }

    void PrintSubsectionHeader (char const *description, ...);

    template <typename T>
    void TestEquality (
        T const &value0,
        char const *value0_name,
        T const &value1,
        char const *value1_name,
        char const *value_printf_format) const
    {
        Uint32 const printf_format_buffer_size = 0x1000;
        char printf_format_buffer[printf_format_buffer_size];
        snprintf(
            printf_format_buffer,
            printf_format_buffer_size,
            "%%s (%s) == %%s (%s)",
            value_printf_format,
            value_printf_format);
        printf_format_buffer[printf_format_buffer_size - 1] = '\0';
        Test(value0 == value1,
             printf_format_buffer,
             value0_name,
             value0,
             value1_name,
             value1);
    }

    template <typename T>
    void TestEquality (
        T const &value0,
        char const *value0_name,
        T const &value1,
        char const *value1_name,
        char const *value_printf_format,
        T epsilon) const
    {
        Uint32 const printf_format_buffer_size = 0x1000;
        char printf_format_buffer[printf_format_buffer_size];
        snprintf(
            printf_format_buffer,
            printf_format_buffer_size,
            "%%s (%s) == %%s (%s)",
            value_printf_format,
            value_printf_format);
        printf_format_buffer[printf_format_buffer_size - 1] = '\0';
        Test(Epsilon(value0, value1, epsilon),
             printf_format_buffer,
             value0_name,
             value0,
             value1_name,
             value1);
    }

    template <typename T>
    void TestInequality (
        T const &value0,
        char const *value0_name,
        T const &value1,
        char const *value1_name,
        char const *value_printf_format) const
    {
        Uint32 const printf_format_buffer_size = 0x1000;
        char printf_format_buffer[printf_format_buffer_size];
        snprintf(
            printf_format_buffer,
            printf_format_buffer_size,
            "%%s (%s) != %%s (%s)",
            value_printf_format,
            value_printf_format);
        printf_format_buffer[printf_format_buffer_size - 1] = '\0';
        Test(value0 != value1,
             printf_format_buffer,
             value0_name,
             value0,
             value1_name,
             value1);
    }

private:

    typedef std::list<UnitTestAtom> AtomList;
    typedef AtomList::iterator AtomListIterator;

    std::string m_name;
    mutable Uint32 m_failure_count;
    AtomList m_atom_list;
}; // end of class UnitTest

#endif // !defined(_UNITTEST_HPP_)

