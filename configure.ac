AC_PREREQ([2.69])
AC_INIT([bluetools], [1.0.0], [17550266+rcasanovama@users.noreply.github.com])

# Initialises automake.
AM_INIT_AUTOMAKE([foreign subdir-objects])

# Checks for programs.
AC_LANG(C)
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([BLUEZ], [bluez >= 5.0], [], [AC_MSG_ERROR(BlueZ >= 5.0 is required)])
PKG_CHECK_MODULES([DBUS], [dbus-1 >= 1.4], [], [AC_MSG_ERROR(D-Bus >= 1.4 is required)])

# Checks for header files.
AC_CHECK_HEADERS([stdio.h stdlib.h stdint.h stdbool.h string.h unistd.h \
                  assert.h errno.h endian.h signal.h getopt.h fcntl.h malloc.h \
                  sys/socket.h sys/ioctl.h sys/stat.h sys/mman.h \
                  dbus/dbus.h bluetooth/bluetooth.h])

AC_CHECK_HEADERS([bluetooth/l2cap.h bluetooth/rfcomm.h bluetooth/hci.h], [], [],
[#ifdef HAVE_BLUETOOTH_BLUETOOTH_H
 # include <bluetooth/bluetooth.h>
 #endif
])

AC_CHECK_HEADERS([bluetooth/hci_lib.h], [], [],
[#if defined(HAVE_BLUETOOTH_BLUETOOTH_H) && defined(HAVE_BLUETOOTH_HCI_H)
 # include <bluetooth/bluetooth.h>
 # include <bluetooth/hci.h>
 #endif
])

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
AC_CHECK_FUNCS([memset memcpy munmap free \
                printf fprintf snprintf \
                strcpy strncpy strcat strncat strlen strtoul strstr strpbrk strdup \
                socket bind listen accept connect send recv shutdown close])

AC_CONFIG_FILES([Makefile tools/Makefile test/Makefile])
AC_OUTPUT
