AC_INIT(configure.ac)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(libxrb, 0.1)

# C++ stuff
AC_LANG_CPLUSPLUS
AC_PROG_CXX
# lex
AC_PROG_LEX
# yacc
AC_PROG_YACC
# this enables libtool usage for creating libraries
AM_PROG_LIBTOOL
# this determines the machine's byte order
AC_C_BIGENDIAN
# checks for the strip program for `make install-strip`
AC_CHECK_TOOL(STRIP, strip)

##############################################################################
# configure script commandline options
##############################################################################

AC_ARG_WITH(
    [arch],
    AC_HELP_STRING(
        [--with-arch=type],
        [compile for a specific architecture type (e.g. i686, athlon, pentium4)]),
    [ARCH_TYPE=$withval],
    [ARCH_TYPE=])

AC_ARG_ENABLE(
    [debug],
    AC_HELP_STRING(
        [--enable-debug],
        [enable debugging symbols and disable optimizations]),
    [DEBUG_ENABLED="yes"],
    [DEBUG_ENABLED="no"])

AC_ARG_ENABLE(
    [gprof],
    AC_HELP_STRING(
        [--enable-gprof],
        [enable runtime profiling using gprof (only works on static compilations]),
    [GPROF_ENABLED="yes"],
    [GPROF_ENABLED="no"])

AC_ARG_ENABLE(
    [bitarray-uses-memcmp],
    AC_HELP_STRING(
        [--enable-bitarray-uses-memcmp],
        [Xrb::BitArray<> comparisons will use memcmp() instead of comparing each element in a loop]),
    [BITARRAY_USES_MEMCMP="yes"],
    [BITARRAY_USES_MEMCMP="no"])

AC_ARG_ENABLE(
    [bitarray-uses-memcpy],
    AC_HELP_STRING(
        [--enable-bitarray-uses-memcpy],
        [Xrb::BitArray<> assignments and copies will use memcpy() instead of assigning/copying each element in a loop]),
    [BITARRAY_USES_MEMCPY="yes"],
    [BITARRAY_USES_MEMCPY="no"])

AC_ARG_ENABLE(
    [bitarray-uses-memset],
    AC_HELP_STRING(
        [--enable-bitarray-uses-memset],
        [Xrb::BitArray<> assignments and copies will use memset() instead of assigning/copying each element in a loop]),
    [BITARRAY_USES_MEMSET="yes"],
    [BITARRAY_USES_MEMSET="no"])

AC_ARG_ENABLE(
    [matrix2-uses-memcpy],
    AC_HELP_STRING(
        [--enable-matrix2-uses-memcpy],
        [Xrb::Matrix2<> assignments and copies will use memcpy() instead of assigning/copying each element in a loop]),
    [MATRIX2_USES_MEMCPY="yes"],
    [MATRIX2_USES_MEMCPY="no"])

AC_ARG_ENABLE(
    [ntuple-uses-memcmp],
    AC_HELP_STRING(
        [--enable-ntuple-uses-memcmp],
        [Xrb::NTuple<> comparisons will use memcmp() instead of comparing each element in a loop]),
    [NTUPLE_USES_MEMCMP="yes"],
    [NTUPLE_USES_MEMCMP="no"])

AC_ARG_ENABLE(
    [ntuple-uses-memcpy],
    AC_HELP_STRING(
        [--enable-ntuple-uses-memcpy],
        [Xrb::NTuple<> assignments and copies will use memcpy() instead of assigning/copying each element in a loop]),
    [NTUPLE_USES_MEMCPY="yes"],
    [NTUPLE_USES_MEMCPY="no"])

AC_ARG_ENABLE(
    [vector-uses-memcmp],
    AC_HELP_STRING(
        [--enable-vector-uses-memcmp],
        [Xrb::Vector<> comparisons will use memcmp() instead of comparing each element in a loop]),
    [VECTOR_USES_MEMCMP="yes"],
    [VECTOR_USES_MEMCMP="no"])

AC_ARG_ENABLE(
    [vector-uses-memcpy],
    AC_HELP_STRING(
        [--enable-vector-uses-memcpy],
        [Xrb::Vector<> assignments and copies will use memcpy() instead of assigning/copying each element in a loop]),
    [VECTOR_USES_MEMCPY="yes"],
    [VECTOR_USES_MEMCPY="no"])

AC_ARG_ENABLE(
    [nan-sanity-check],
    AC_HELP_STRING(
        [--enable-nan-sanity-check],
        [Performs pedantic checks in Engine2::Entity code to detect when any NaN (or Inf) is introduced]),
    [NAN_SANITY_CHECK="yes"],
    [NAN_SANITY_CHECK="no"])

##############################################################################
# processing of above commandline options
##############################################################################

if test "x${ARCH_TYPE}" = "x"; then
    ARCH_OPTION_STRING=
else
    ARCH_OPTION_STRING="-march=${ARCH_TYPE}"
fi

if test "${DEBUG_ENABLED}" = "yes"; then
    CXXFLAGS="${CXXFLAGS} -g3 -O0 ${ARCH_OPTION_STRING}"
else
    CXXFLAGS="${CXXFLAGS} -O2"
    # the gprof option -pg is incompatible with -fomit-frame-pointer
    # so only add it if gprof is not enabled.
    if test "${GPROF_ENABLED}" != "yes"; then
        CXXFLAGS="${CXXFLAGS} -fomit-frame-pointer"
    fi
fi

if test "${GPROF_ENABLED}" = "yes"; then
    # setting DEBUG_LEVEL to 0 so asserts are not compiled in
    CXXFLAGS="${CXXFLAGS} -pg -fprofile-arcs -DDEBUG_LEVEL=0 -g3"
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
# add -Wall to the compiler flags
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

# check for OpenGL library (this seems not exactly necessary)
AC_CHECK_LIB(GL, glBegin)

# check for OpenGL Utilities library (this seems not exactly necessary)
AC_CHECK_LIB(GLU, gluBuild2DMipmaps)

# check for SDL_image library
AC_CHECK_LIB(SDL_image, IMG_Load)

# check for SDL
AC_PATH_PROG(SDL_CONFIG, sdl-config)
AC_SUBST(SDL_CONFIG)

# check for freetype
AC_PATH_PROG(FREETYPE_CONFIG, freetype-config)
AC_SUBST(FREETYPE_CONFIG)

##############################################################################
# configure script generation
##############################################################################

# specify the Makefiles
AC_OUTPUT(
    Makefile

    lib/Makefile

    app/Makefile
    app/example/Makefile
    app/example/disasteroids/Makefile
    app/example/guidemo/Makefile
    app/mapeditor2/Makefile
    app/unittest/Makefile
)

