AC_INIT(XuqRijBuh, 0.5, victor.dods@gmail.com, xuqrijbuh)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE

# C++ stuff
AC_LANG_CPLUSPLUS
AC_PROG_CXX
# this enables libtool usage for creating libraries
AM_PROG_LIBTOOL
# this determines the machine's byte order
AC_C_BIGENDIAN
# checks for the strip program for `make install-strip`
AC_CHECK_TOOL(STRIP, strip)
# checks for reflex, a lexical scanner generator
AC_CHECK_TOOL(REFLEX, reflex)
# checks for trison, a parser for LALR(k) grammars
AC_CHECK_TOOL(TRISON, trison)
# checks for doxygen (for `make doc`)
AC_CHECK_TOOL(DOXYGEN, doxygen)
# checks for perl (which seems to be used by doxygen)
AC_CHECK_TOOL(PERL, perl)
# checks for dot (used to turn .dot graphs into images)
AC_CHECK_TOOL(DOT, dot)

##############################################################################
# processing for above tool checks
##############################################################################

# these conditional will be used by Makefile.am in lib for determining
# wether or not to generate the reflex scanners and trison parsers from
# source, or use the pregenerated parsers.
AM_CONDITIONAL(REFLEX_EXISTS, [test "${REFLEX}" != ""])
AM_CONDITIONAL(TRISON_EXISTS, [test "${TRISON}" != ""])

##############################################################################
# configure script commandline options
##############################################################################

AC_ARG_WITH(
    [arch],
    AC_HELP_STRING(
        [--with-arch=type],
        [compile for a specific architecture type (e.g. native, i686, athlon, pentium4)]),
    [ARCH_TYPE=$withval],
    [ARCH_TYPE=])

AC_ARG_ENABLE(
    [debug],
    AC_HELP_STRING(
        [--enable-debug],
        [enable debugging symbols and disable optimizations]),
    [DEBUG_ENABLED=$enableval],
    [DEBUG_ENABLED="no"])

AC_ARG_WITH(
    [debug-level],
    AC_HELP_STRING(
        [--with-debug-level=N],
        [sets assert level -- 0 for fewest asserts, up to 3 for all asserts -- default is 0]),
    [DEBUG_LEVEL_VAL=$withval],
    [DEBUG_LEVEL_VAL=0])

AC_ARG_ENABLE(
    [warnings-as-errors],
    AC_HELP_STRING(
        [--disable-warnings-as-errors],
        [disables the default behavior of interpreting warnings as errors]),
    [WARNINGS_AS_ERRORS=$enableval],
    [WARNINGS_AS_ERRORS="yes"])

AC_ARG_ENABLE(
    [gprof],
    AC_HELP_STRING(
        [--enable-gprof],
        [enable runtime profiling using gprof (only works on static compilations]),
    [GPROF_ENABLED=$enableval],
    [GPROF_ENABLED="no"])

AC_ARG_ENABLE(
    [bitarray-uses-memcmp],
    AC_HELP_STRING(
        [--enable-bitarray-uses-memcmp],
        [Xrb::BitArray<> comparisons will use memcmp() instead of comparing each element in a loop]),
    [BITARRAY_USES_MEMCMP=$enableval],
    [BITARRAY_USES_MEMCMP="no"])

AC_ARG_ENABLE(
    [64bit-integer-types],
    AC_HELP_STRING(
        [--enable-64bit-integer-types],
        [Enable the use of 64bit integers]),
    [ENABLE_64BIT_INTEGER_TYPES=$enableval],
    [ENABLE_64BIT_INTEGER_TYPES="no"])

AC_ARG_ENABLE(
    [bitarray-uses-memcpy],
    AC_HELP_STRING(
        [--enable-bitarray-uses-memcpy],
        [Xrb::BitArray<> assignments and copies will use memcpy() instead of assigning/copying each element in a loop]),
    [BITARRAY_USES_MEMCPY=$enableval],
    [BITARRAY_USES_MEMCPY="no"])

AC_ARG_ENABLE(
    [bitarray-uses-memset],
    AC_HELP_STRING(
        [--enable-bitarray-uses-memset],
        [Xrb::BitArray<> assignments and copies will use memset() instead of assigning/copying each element in a loop]),
    [BITARRAY_USES_MEMSET=$enableval],
    [BITARRAY_USES_MEMSET="no"])

AC_ARG_ENABLE(
    [matrix2-uses-memcpy],
    AC_HELP_STRING(
        [--enable-matrix2-uses-memcpy],
        [Xrb::Matrix2<> assignments and copies will use memcpy() instead of assigning/copying each element in a loop]),
    [MATRIX2_USES_MEMCPY=$enableval],
    [MATRIX2_USES_MEMCPY="no"])

AC_ARG_ENABLE(
    [ntuple-uses-memcmp],
    AC_HELP_STRING(
        [--enable-ntuple-uses-memcmp],
        [Xrb::NTuple<> comparisons will use memcmp() instead of comparing each element in a loop]),
    [NTUPLE_USES_MEMCMP=$enableval],
    [NTUPLE_USES_MEMCMP="no"])

AC_ARG_ENABLE(
    [ntuple-uses-memcpy],
    AC_HELP_STRING(
        [--enable-ntuple-uses-memcpy],
        [Xrb::NTuple<> assignments and copies will use memcpy() instead of assigning/copying each element in a loop]),
    [NTUPLE_USES_MEMCPY=$enableval],
    [NTUPLE_USES_MEMCPY="no"])

AC_ARG_ENABLE(
    [vector-uses-memcmp],
    AC_HELP_STRING(
        [--enable-vector-uses-memcmp],
        [Xrb::Vector<> comparisons will use memcmp() instead of comparing each element in a loop]),
    [VECTOR_USES_MEMCMP=$enableval],
    [VECTOR_USES_MEMCMP="no"])

AC_ARG_ENABLE(
    [vector-uses-memcpy],
    AC_HELP_STRING(
        [--enable-vector-uses-memcpy],
        [Xrb::Vector<> assignments and copies will use memcpy() instead of assigning/copying each element in a loop]),
    [VECTOR_USES_MEMCPY=$enableval],
    [VECTOR_USES_MEMCPY="no"])

AC_ARG_ENABLE(
    [nan-sanity-check],
    AC_HELP_STRING(
        [--enable-nan-sanity-check],
        [Performs pedantic checks in Engine2::Entity code to detect when any NaN (or Inf) is introduced]),
    [NAN_SANITY_CHECK=$enableval],
    [NAN_SANITY_CHECK="no"])

##############################################################################
# processing for above commandline options
##############################################################################

if test "${ARCH_TYPE}" = ""; then
    ARCH_OPTION_STRING=
else
    ARCH_OPTION_STRING="-mtune=${ARCH_TYPE}"
fi

if test "${DEBUG_ENABLED}" = "yes"; then
    CXXFLAGS="${CXXFLAGS} -g3 -O0 ${ARCH_OPTION_STRING}"
else
    CXXFLAGS="${CXXFLAGS} -g0 -O2"
    # the gprof option -pg is incompatible with -fomit-frame-pointer
    # so only add it if gprof is not enabled.
    if test "${GPROF_ENABLED}" != "yes"; then
        CXXFLAGS="${CXXFLAGS} -fomit-frame-pointer"
    fi
fi

if test "${GPROF_ENABLED}" = "yes"; then
    # setting DEBUG_LEVEL to 0 so asserts are not compiled in
    DEBUG_LEVEL_VAL=0
    # adding compiler flags necessary for gprof
    CXXFLAGS="${CXXFLAGS} -pg -fprofile-arcs -g3"
fi

# this is utterly retarded, but AC_DEFINE([DEBUG_LEVEL],${DEBUG_LEVEL_VAL})
# doesn't work.  fuck this retarded macro language.
if test "${DEBUG_LEVEL_VAL}" = "0"; then
    AC_DEFINE(
        [XRB_DEBUG_LEVEL],
        [0],
        [Defines the assert/debug code level.  0 for fewest asserts, ranging up to 3 for all asserts.  If undefined, the value is 3.])
else
if test "${DEBUG_LEVEL_VAL}" = "1"; then
    AC_DEFINE(
        [XRB_DEBUG_LEVEL],
        [1],
        [Defines the assert/debug code level.  0 for fewest asserts, ranging up to 3 for all asserts.  If undefined, the value is 3.])
else
if test "${DEBUG_LEVEL_VAL}" = "2"; then
    AC_DEFINE(
        [XRB_DEBUG_LEVEL],
        [2],
        [Defines the assert/debug code level.  0 for fewest asserts, ranging up to 3 for all asserts.  If undefined, the value is 3.])
else
    AC_DEFINE(
        [XRB_DEBUG_LEVEL],
        [3],
        [Defines the assert/debug code level.  0 for fewest asserts, ranging up to 3 for all asserts.  If undefined, the value is 3.])
fi
fi
fi

# if warnings should be interpreted as errors, add -Wall to the compiler flags
if test "${WARNINGS_AS_ERRORS}" = "yes"; then
    CXXFLAGS="${CXXFLAGS} -Werror"
fi

if test "${ENABLE_64BIT_INTEGER_TYPES}" = "yes"; then
    # define XRB_ENABLE_64BIT_INTEGER_TYPES so that the 64bit int types
    # and various associated functions are compiled in.
    AC_DEFINE(
        [XRB_ENABLE_64BIT_INTEGER_TYPES],
        [],
        [Define if 64bit integer types should be enabled.])
fi

if test "${BITARRAY_USES_MEMCMP}" = "yes"; then
    # define XRB_BITARRAY_USES_MEMCMP so that the memcmp() code in Xrb::BitArray<>
    # will be compiled in (instead of the element-iterating code.
    AC_DEFINE(
        [XRB_BITARRAY_USES_MEMCMP],
        [],
        [Define if Xrb::BitArray<> should use memcmp for comparisons instead of comparing each element.])
fi

if test "${BITARRAY_USES_MEMCPY}" = "yes"; then
    # define XRB_BITARRAY_USES_MEMCPY so that the memcpy() code in Xrb::BitArray<>
    # will be compiled in (instead of the element-iterating code.
    AC_DEFINE(
        [XRB_BITARRAY_USES_MEMCPY],
        [],
        [Define if Xrb::BitArray<> should use memcpy for assignment and copying instead of assigning/copying each element.])
fi

if test "${BITARRAY_USES_MEMSET}" = "yes"; then
    # define XRB_BITARRAY_USES_MEMSET so that the memset() code in Xrb::BitArray<>
    # will be compiled in (instead of the element-iterating code.
    AC_DEFINE(
        [XRB_BITARRAY_USES_MEMSET],
        [],
        [Define if Xrb::BitArray<> should use memset for memory filling instead of filling each element.])
fi

if test "${MATRIX2_USES_MEMCPY}" = "yes"; then
    # define XRB_MATRIX2_USES_MEMCPY so that the memcpy() code in
    # Xrb::Matrix2<> will be compiled in (instead of the element-iterating
    # code.
    AC_DEFINE(
        [XRB_MATRIX2_USES_MEMCPY],
        [],
        [Define if Xrb::Matrix2<> should use memcpy for assignment and copying instead of assigning/copying each element.])
fi

if test "${NTUPLE_USES_MEMCMP}" = "yes"; then
    # define XRB_NTUPLE_USES_MEMCMP so that the memcmp() code in Xrb::NTuple<>
    # will be compiled in (instead of the element-iterating code.
    AC_DEFINE(
        [XRB_NTUPLE_USES_MEMCMP],
        [],
        [Define if Xrb::NTuple<> should use memcmp for comparisons instead of comparing each element.])
fi

if test "${NTUPLE_USES_MEMCPY}" = "yes"; then
    # define XRB_NTUPLE_USES_MEMCPY so that the memcpy() code in Xrb::NTuple<>
    # will be compiled in (instead of the element-iterating code.
    AC_DEFINE(
        [XRB_NTUPLE_USES_MEMCPY],
        [],
        [Define if Xrb::NTuple<> should use memcpy for assignment and copying instead of assigning/copying each element.])
fi

if test "${VECTOR_USES_MEMCMP}" = "yes"; then
    # define XRB_VECTOR_USES_MEMCMP so that the memcmp() code in Xrb::Vector<>
    # will be compiled in (instead of the element-iterating code.
    AC_DEFINE(
        [XRB_VECTOR_USES_MEMCMP],
        [],
        [Define if Xrb::Vector<> should use memcmp for comparisons instead of comparing each element.])
fi

if test "${VECTOR_USES_MEMCPY}" = "yes"; then
    # define XRB_VECTOR_USES_MEMCPY so that the memcpy() code in Xrb::Vector<>
    # will be compiled in (instead of the element-iterating code.
    AC_DEFINE(
        [XRB_VECTOR_USES_MEMCPY],
        [],
        [Define if Xrb::Vector<> should use memcpy for assignment and copying instead of assigning/copying each element.])
fi

if test "${NAN_SANITY_CHECK}" = "yes"; then
    # define XRB_NAN_SANITY_CHECK so that pedantic checks in Engine2::Entity
    # code are performed to detect when any NaN (or Inf) is introduced.
    AC_DEFINE(
        [XRB_NAN_SANITY_CHECK],
        [],
        [Define if pedantic checks in Engine2::Entity code should be performed to detect when any NaN (or Inf) is introduced.])
fi

##############################################################################
# add some good stuff to the compiler flags
##############################################################################

CXXFLAGS="${CXXFLAGS} -Wall"

##############################################################################
# set up the include directory properly
##############################################################################

includedir=${includedir}/xrb

##############################################################################
# check for libraries and lib config programs
##############################################################################

# check for the math library
AC_CHECK_LIB(m, sin)

# check for OpenGL library
AC_CHECK_LIB(GL, glBegin)

# check for SDL library
AC_CHECK_LIB(SDL, SDL_Init)

# check for SDL
AC_PATH_PROG(SDL_CONFIG, sdl-config)
AC_SUBST(SDL_CONFIG)
AM_CONDITIONAL(SDL_CONFIG_EXISTS, [test "${SDL_CONFIG}" != ""])

# check for freetype
AC_PATH_PROG(FREETYPE_CONFIG, freetype-config)
AC_SUBST(FREETYPE_CONFIG)
AM_CONDITIONAL(FREETYPE_CONFIG_EXISTS, [test "${FREETYPE_CONFIG}" != ""])

# check for libpng
AC_PATH_PROG(LIBPNG_CONFIG, libpng-config)
AC_SUBST(LIBPNG_CONFIG)
AM_CONDITIONAL(LIBPNG_CONFIG_EXISTS, [test "${LIBPNG_CONFIG}" != ""])

##############################################################################
# configure script generation
##############################################################################

# specify the Makefiles
AC_OUTPUT(
    Makefile
    Doxyfile
)

