/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2019, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */
#ifndef XSPLATFORM_H
#define XSPLATFORM_H

#include "xstypesconfig.h"

#ifdef _WIN32
/// microsoft / windows
#include <windows.h>

#include <stdio.h>
#define XsIoHandle HANDLE

#else
/// gcc / linux
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include "pstdint.h"
#define _strnicmp strncasecmp
typedef int32_t XsIoHandle;

#endif

#endif  // file guard
