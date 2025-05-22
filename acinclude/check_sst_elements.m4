AC_DEFUN([CHECK_SST_ELEMENTS], [

echo "SST_CPPFLAGS: $SST_CPPFLAGS"

AC_ARG_WITH([sst-elements],
    AS_HELP_STRING([--with-sst-elements@<:@=DIR@:>@],
        [Path to valid installation of sst-elements library],
    ), [
      SST_ELEMENTS="$withval"
      have_elements="yes"
    ], [
       AC_MSG_ERROR([sst-elements is required])
    ]
)

if test "X$have_elements" = "Xyes"; then
  AC_DEFINE_UNQUOTED([HAVE_SST_ELEMENTS], 1, [Build with sst-elements])
  AM_CONDITIONAL([HAVE_SST_ELEMENTS], true)

  SAVE_CPPFLAGS="$CPPFLAGS"
  SAVE_CXXFLAGS="$CXXFLAGS"
  SST_ELEMENTS_CPPFLAGS="-I$SST_ELEMENTS/include $SST_CPPFLAGS"
  CPPFLAGS="$CPPFLAGS $SST_ELEMENTS_CPPFLAGS $SST_CPPFLAGS"
  CXXFLAGS="$CXXFLAGS $SST_CXXFLAGS"

  AC_CHECK_HEADERS([sst/elements/merlin/router.h], [],
      [AC_MSG_ERROR([Could not locate SST element header files at $SST_ELEMENTS])])

  AC_SUBST(SST_ELEMENTS_CPPFLAGS)
  CPPFLAGS="$SAVE_CPPFLAGS"
  CXXFLAGS="$SAVE_CXXFLAGS"
else
  SST_ELEMENTS_CPPFLAGS=""
  AC_SUBST(SST_ELEMENTS_CPPFLAGS)
  AC_SUBST(SST_ELEMENTS)
  AM_CONDITIONAL([HAVE_SST_ELEMENTS], false)
fi

])

