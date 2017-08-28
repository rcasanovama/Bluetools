AC_PREREQ([2.69])
AC_INIT([bluetools], [1.0.0], [17550266+rcasanovama@users.noreply.github.com])

# Initialises automake.
AM_INIT_AUTOMAKE([foreign subdir-objects])

# Checks for programs.
AC_LANG(C)
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h malloc.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset munmap socket strdup strpbrk strstr strtoul])

AC_CONFIG_FILES([Makefile tools/Makefile test/Makefile])
AC_OUTPUT
