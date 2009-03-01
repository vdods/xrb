// ///////////////////////////////////////////////////////////////////////////
// test/main.cpp by Victor Dods, created 2005/05/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "../defines.hpp"

#include "unittestcontroller.hpp"

#include "matrix2unittest.hpp"
#include "serializerunittest.hpp"
#include "simpletransform2unittest.hpp"
#include "transform2unittest.hpp"
#include "vectorunittest.hpp"

int main (int argc, char **argv)
{
    UnitTestController unit_test_controller;

    // for this to work, each unit test must have the
    // default constructor defined.
    #define REGISTER_UNIT_TEST_BY_CLASS_NAME(ClassName) \
    unit_test_controller.RegisterUnitTest(new ClassName);

    // register all the unit tests
//     REGISTER_UNIT_TEST_BY_CLASS_NAME(Matrix2UnitTest)
//     REGISTER_UNIT_TEST_BY_CLASS_NAME(SerializerUnitTest)
//     REGISTER_UNIT_TEST_BY_CLASS_NAME(SimpleTransform2UnitTest)
    REGISTER_UNIT_TEST_BY_CLASS_NAME(Transform2UnitTest)
//     REGISTER_UNIT_TEST_BY_CLASS_NAME(VectorUnitTest)

    unit_test_controller.GenerateCommandLineOptions();
    unit_test_controller.RunTests(argc, argv);

    return unit_test_controller.GetFailureCount() == 0 ? 0 : -1;
}
