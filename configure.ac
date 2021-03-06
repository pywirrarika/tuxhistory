#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_INIT([Tux Hisotry],[0.0.1],[tuxmath-devel@lists.sourceforge.net],[tuxhistory])

AC_PREREQ(2.61)
AC_CANONICAL_HOST
AC_CANONICAL_BUILD
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(foreign)
AC_CONFIG_SRCDIR([src/tuxhistory.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

NAME_VERSION=$PACKAGE-$VERSION

AC_DEFINE_UNQUOTED([PROGRAM_NAME], "$PACKAGE", [Tux History])

AC_SUBST(NAME_VERSION)

AC_GNU_SOURCE

dnl For libtool support to link libt4k-common:
dnl NOTE - currently libtool init commented out because of errors in crossbuild.
dnl libtool not actually needed until we start to use libt4k-common.
dnl When crossbuild issues fixed, uncomment libtool init and remove ranlib line.
#LT_INIT
AC_PROG_RANLIB

# ----------------------------------------------------------------------
# Checks for programs.
# ----------------------------------------------------------------------

AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_MKDIR_P
AC_PROG_YACC


# ---------------------------------------------------------------------
# Gettext support:
# ---------------------------------------------------------------------

AM_GNU_GETTEXT_VERSION([0.17])

dnl Use the following line whenever possible:
AM_GNU_GETTEXT([external])
dnl If external intl lib unavailable on platform, comment out the
dnl above line and uncomment the following line to use the included
dnl intl subdirectory. This used to be needed for the Windows cross-
dnl build but is not longer necessary:
#AM_GNU_GETTEXT

# -----------------------------------------------------------------------
# Checks for libraries.
# -----------------------------------------------------------------------

# Check platform - see if WinMain needed:
AC_MSG_CHECKING([for native Win32])
case "$host" in
  *-*-mingw*)
    native_win32=yes
    ;;
  *)
    native_win32=no
    ;;
esac
AC_MSG_RESULT([$native_win32])

dnl FIXME - get rid of remaining win32 conditional code
AM_CONDITIONAL(BUILD_MINGW32, test $native_win32 = yes)



# -----------------------------------------------------------------------
# Checks for libraries.
# -----------------------------------------------------------------------



dnl ---------------------------------------------------------
dnl For crossbuild with mingw-cross-env, we use pkg_config,
dnl but for native linux build we use #AC_CHECK_LIB() because
dnl some distros (incl. Debian) don't support pkg_config for
dnl all of the SDL libraries:
dnl ---------------------------------------------------------


dnl Check for SDL
PKG_CHECK_MODULES([SDL],
  [sdl >= 1.2.0],
  [],
  [ SDL_VERSION=1.2.0
    AM_PATH_SDL($SDL_VERSION, :,
       AC_MSG_ERROR([SDL version $SDL_VERSION not found! http://www.libsdl.org])
    ) 
  ]
)

CFLAGS="$CFLAGS $SDL_CFLAGS" 
LIBS="$LIBS $SDL_LIBS"




dnl ---------------------------------------------------------------------------
dnl In the following checks we attempt to use pkg-config if available, falling
dnl back to #AC_CHECK_LIB if #PKG_CHECK_MODULES fails.
dnl ---------------------------------------------------------------------------

dnl Check for SDL_image: ------------------------------------------------------

PKG_CHECK_MODULES([SDL_IMAGE],
    [SDL_image],
    ,
    [  
      [echo "SDL_IMAGE not found via pkg_config, checking with AC_CHECK_LIB:"]
      AC_CHECK_LIB([SDL_image],
	[IMG_Load],
        ,
        [AC_MSG_ERROR([SDL_image not found! http://www.libsdl.org/projects/SDL_image])]
      )
      [echo "SDL_IMAGE successfully located"]
    ]
)

AC_DEFINE([HAVE_LIBSDL_IMAGE],[1],[Define to 1 if you have the `SDL_image` library])
CFLAGS="$CFLAGS $SDL_IMAGE_CFLAGS"
LIBS="$LIBS $SDL_IMAGE_LIBS"


dnl Check for SDL_mixer: --------------------------------------------------------

PKG_CHECK_MODULES([SDL_MIXER],
    [SDL_mixer],
    ,
    [ [echo "SDL_MIXER not found via pkg_config, checking with AC_CHECK_LIB:"]
      AC_CHECK_LIB([SDL_mixer],
	 [Mix_OpenAudio],
         ,
         [AC_MSG_ERROR([SDL_mixer not found! http://www.libsdl.org/projects/SDL_mixer])]
      )
      [echo "SDL_MIXER successfully located"]
    ] 
)

AC_DEFINE([HAVE_LIBSDL_MIXER],[1],[Define to 1 if you have the `SDL_mixer` library])
CFLAGS="$CFLAGS $SDL_MIXER_CFLAGS"
LIBS="$LIBS $SDL_MIXER_LIBS"

dnl Check for MiniXML: --------------------------------------------------------

PKG_CHECK_MODULES([MXML],
    [mxml],
    ,
    [
      [echo "MXML not found via pkg_config, checking with AC_CHECK_LIB:"]
      AC_CHECK_LIB([mxml],
	 [mxmlNewXML],
         ,
         [AC_MSG_ERROR([MiniXML not found! http://www.minixml.org])]
      )
      [echo "MiniXML successfully located"]
    ] 
)

AC_DEFINE([HAVE_LIBMXML],[1],[Define to 1 if you have the `mxml` library])
CFLAGS="$CFLAGS $MXML_CFLAGS"
LIBS="$LIBS $MXML_LIBS"

dnl Check for (somewhat) higher-level math functions - needed for SDL_extras
AC_CHECK_LIB([m],
              [csin],
              ,
              [AC_MSG_ERROR([Math library not found - functions in <math.h> may not be available.])])

dnl Check for LibLua: --------------------------------------------------------

AC_MSG_CHECKING([for Lua package name])
PKG_CHECK_EXISTS([lua5.1], [LUA="lua5.1"],
    [PKG_CHECK_EXISTS([lua-5.1], [LUA="lua-5.1"], [LUA="lua"])])
AC_MSG_RESULT([$LUA])

PKG_CHECK_MODULES([LUA],
    [$LUA],
    ,
    [ [echo "$LUA not found via pkg_config, checking with AC_CHECK_LIB:"]
      AC_CHECK_LIB([$LUA],
	 [lua_newstate],
         ,
         [AC_MSG_ERROR([LUA no found! http://www.lua.org/download.html])]
      )
      [echo "$LUA successfully located"]
    ] 
)

AC_DEFINE([HAVE_LUA],[1],[Define to 1 if you have the `LUA` library])


CFLAGS="$CFLAGS $LUA_CFLAGS"
LIBS="$LIBS $LUA_LIBS"

dnl AX_WITH_LUA()
dnl AX_LUA_VERSION(500)
dnl AX_LUA_HEADERS()
dnl AX_LUA_HEADERS_VERSION(500)
dnl AX_LUA_LIBS()
dnl AC_DEFINE([HAVE_LIBLUA],[1],[Define to 1 if you have the `lua` library])
dnl CFLAGS="$CFLAGS $LUA_INCLUDE"
dnl LIBS="$LIBS $LUA_LIB"

dnl Check for SDL_Pango: --------------------------------------------------------

dnl SDL_Pango is enabled by default.
dnl If SDL_Pango disabled at configure time, or if we can't find it,
dnl we look for SDL_ttf:

AC_ARG_WITH([sdlpango],
	    [AS_HELP_STRING([--without-sdlpango],
	    [Do not use SDL_Pango even if available])],
	    [],
	    [with_sdlpango=yes]
)

if test "x$with_sdlpango" != xno; then

PKG_CHECK_MODULES([SDL_PANGO],
    [SDL_Pango],
    ,
    [
      [echo "SDL_PANGO not found via pkg_config, checking with AC_CHECK_LIB:"]
      AC_CHECK_LIB([SDL_Pango],
	 [SDLPango_Init],
         ,
         [AC_MSG_ERROR([SDL_Pango not found! http://sdlpango.sourceforge.net])]
      )
      [echo "SDL_PANGO successfully located"]
    ] 
)

AC_DEFINE([HAVE_LIBSDL_PANGO],[1],[Define to 1 if you have the `SDL_Pango` library])
CFLAGS="$CFLAGS $SDL_PANGO_CFLAGS"
LIBS="$LIBS $SDL_PANGO_LIBS"

fi
	
dnl We only use SDL_ttf if SDL_Pango disabled: --------------------------------------

if [test "x$with_sdlpango" = xno]; then

dnl Add freetype link "by hand" until mingw-cross-env takes care
dnl of this automatically
LIBS="-lfreetype $LIBS"

AC_CHECK_LIB([SDL_ttf],
             [TTF_Init],
             ,
             [AC_MSG_ERROR([SDL_ttf not found! http://www.libsdl.org/projects/SDL_ttf])])
fi



dnl Check for libRSVG and cairo. SVG support is enabled by default

AC_ARG_WITH([rsvg],
            [AS_HELP_STRING([--without-rsvg],[do not use libRSVG even if available])],
            [with_rsvg=no],
            [with_rsvg=yes])

if test "x$with_rsvg" = xyes; then
  RSVG_CFLAGS=""
  RSVG_LIBS=""
  PKG_CHECK_MODULES([RSVG],
                    [librsvg-2.0 >= 2.18],
                    ,
                    [AC_MSG_FAILURE([libRSVG test failed (--without-rsvg to disable svg support)])])
  CFLAGS="$CFLAGS $RSVG_CFLAGS"
  LIBS="$LIBS $RSVG_LIBS"
fi

if test "x$with_rsvg" = xyes; then
  CAIRO_CFLAGS=""
  CAIRO_LIBS=""
  PKG_CHECK_MODULES([CAIRO],
                    [cairo >= 1.4.10],
                    ,
                    [AC_MSG_FAILURE([cairo test failed (--without-rsvg to disable svg support)])])
  CFLAGS="$CFLAGS $CAIRO_CFLAGS"
  LIBS="$LIBS $CAIRO_LIBS"
  AC_DEFINE([HAVE_RSVG],[1],[Define to 1 if you have the `libRSVG` library])
fi





dnl Check for SDL_net: --------------------------------------------------------

dnl SDL_net is enabled by default.

AC_ARG_WITH([sdlnet],
	    [AS_HELP_STRING([--without-sdlnet],
	    [Do not use SDL_net even if available])],
	    [],
	    [with_sdlnet=no]
)

if test "x$with_sdlnet" != xno; then
  dnl We will need this for socket programming on Win32
  AC_CHECK_LIB([ws2_32],
  	       [WSAStartup],
	       ,
	       [AC_MSG_NOTICE([Windows socket library ws2_32 not found - (only needed on win32)])]
  )



  PKG_CHECK_EXISTS([SDL_net],
	dnl Use pkg-config if possible:
	[SDL_NET_CFLAGS=""
	 SDL_NET_LIBS=""
	 PKG_CHECK_MODULES([SDL_NET],
			   [SDL_net],
			   ,
			   [with_sdlnet=no;
			    AC_MSG_FAILURE([SDL_net test failed (--without-sdlnet to disable)])])
	 CFLAGS="$CFLAGS $SDL_NET_CFLAGS"
	 LIBS="$LIBS $SDL_NET_LIBS"
	 AC_DEFINE([HAVE_LIBSDL_NET],[1],[Define to 1 if you have the `SDL_net` library]
	)],
	dnl Fallback if we can't use pkg-config:
	[AC_CHECK_LIB([SDL_net], 
		      [SDLNet_Init],
		      ,
		      [with_sdlnet=no;
		       AC_MSG_FAILURE([SDL_net test failed (--without-sdlnet to disable)])]
	)]
  )
fi



dnl Use of libt4kcommon is disabled for now by default 
dnl but can be enabled at configure time for testing and development.

AC_ARG_WITH([libt4kcommon],
            [AS_HELP_STRING([--with-libt4kcommon],[use libtk4common if available])],
            [with_libt4kcommon=yes],
            [with_libt4kcommon=no])

if test "x$with_libt4kcommon" != xno; then
AC_CHECK_LIB([t4kcommon],
             [InitT4KCommon],
             ,
             [with_libt4kcommon=no;
              AC_MSG_FAILURE([libt4kcommon test failed (--without-libt4kcommon to disable use of libt4kcommon)])])
fi


dnl Check for (somewhat) higher-level math functions - needed for SDL_extras
dnl AC_CHECK_LIB([m],
dnl              [csin],
dnl              ,
dnl              [AC_MSG_ERROR([Math library not found - functions in <math.h> may not be available.])])



# --------------------------------------------------------------------------------------
# Checks for header files.
# --------------------------------------------------------------------------------------

AC_FUNC_ALLOCA
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([argz.h error.h errno.h fcntl.h float.h iconv.h inttypes.h langinfo.h libgen.h libintl.h limits.h locale.h malloc.h math.h pthread.h stddef.h stdint.h stdio_ext.h stdlib.h string.h strings.h sys/param.h unistd.h wchar.h])


# --------------------------------------------------------------------------------------------
# Checks for typedefs, structures, and compiler characteristics.
# --------------------------------------------------------------------------------------------

AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_C_RESTRICT
AC_TYPE_MODE_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_C_VOLATILE
AC_CHECK_TYPES([ptrdiff_t])



# -------------------------------------------------------------------------------------------
# Checks for library functions.
# -------------------------------------------------------------------------------------------


AC_FUNC_CLOSEDIR_VOID
AC_FUNC_ERROR_AT_LINE
#AC_FUNC_MALLOC
AC_FUNC_MMAP
#AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_STRCOLL
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([__argz_count __argz_next __argz_stringify atexit basename bcopy fcntl floor getcwd localeconv localtime_r memmove mempcpy memset Mix_Init mkdir munmap nl_langinfo scandir setlocale sqrt stpcpy strcasecmp strchr strcspn strdup strncasecmp strndup strrchr strstr strtoul])


# Set the 
MINGW32_PACKAGE_DATA_DIR="data"
AC_SUBST(MINGW32_PACKAGE_DATA_DIR)

#AM_CONDITIONAL(BUILD_MINGW32, test "$native_win32" = yes)

if test $native_win32 = yes; then
   AC_DEFINE([BUILD_MINGW32], 1,[Native MinGW32 build])
   CFLAGS="$CFLAGS -D__GW32__"
   LIBS="$LIBS -luuid -lole32 -lwsock32 -mwindows"
   CPPFLAGS="$CPPFLAGS -idirafter $prefix/include/glibc"
   AC_PATH_PROG(WINDRES, "$target_alias-windres", no, [$PATH])
fi


# ------------------------------------------------------------
# Support for building NSIS Win32 installer:
# ------------------------------------------------------------

# This is the temporary DESTDIR target into which we do a "make install" before
# the files get copied into the NSIS installer:
NSI_INSTALL_DIR=mingw32
AC_SUBST(NSI_INSTALL_DIR)

# This is the path (relative to the TuxMath.exe binary) where the data files are
# placed within the TuxMath folder on the user's Windows machine:
MINGW32_PACKAGE_DATA_DIR="data"
AC_SUBST(MINGW32_PACKAGE_DATA_DIR)

AC_ARG_WITH([dll-directory],
            AS_HELP_STRING([--with-dll-directory=path], [provide location of DLL files needed for build  [$(NSI_DLL_DIR)]]),
            [dll_path=$withval], 
            [dll_path=no])

AM_CONDITIONAL(NSI_BUILD, test $dll_path = no)

if test $dll_path != no; then
NSI_DLL_DIR=$dll_path
else
NSI_DLL_DIR=~/tuxmath_dll
fi
AC_SUBST(NSI_DLL_DIR)

AC_PATH_PROG(NSIS, makensis, [], [$PATH])



# ---------------------------------------------------------------
# Create Makefiles
# ---------------------------------------------------------------

AC_CONFIG_FILES([
Makefile
tuxmath.spec
tuxmath_preview.spec
data/Makefile
data/fonts/Makefile
data/images/Makefile
data/images/backgrounds/Makefile
data/images/icons/Makefile
data/images/gui/Makefile
data/images/others/Makefile
data/images/penguins/Makefile
data/images/status/Makefile
data/images/title/Makefile
data/images/tux/Makefile
data/images/units/Makefile
data/images/buildings/Makefile
data/images/terrain/Makefile
data/images/terrain/arctic/Makefile
data/images/terrain/desert/Makefile
data/images/terrain/grassland/Makefile
data/images/terrain/highSeas/Makefile
data/images/terrain/marsh/Makefile
data/images/terrain/ocean/Makefile
data/images/terrain/plains/Makefile
data/images/terrain/prairie/Makefile
data/images/terrain/savannah/Makefile
data/images/terrain/swamp/Makefile
data/images/terrain/tundra/Makefile
data/images/terrain/unexplored/Makefile
data/images/forest/Makefile
data/maps/Makefile
data/objects/Makefile
data/menus/Makefile
data/sounds/Makefile
doc/Makefile
nsis/tuxmath.nsi
nsis/tuxmath_with_conf.nsi
src/Makefile
intl/Makefile
linebreak/Makefile
po/Makefile.in
])

AC_OUTPUT
