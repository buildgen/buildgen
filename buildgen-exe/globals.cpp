// Copyright 2011-2012 Kevin Cox

/*******************************************************************************
*                                                                              *
*  Permission is hereby granted, free of charge, to any person obtaining a     *
*  copy of this software and associated documentation files (the "Software"),  *
*  to deal in the Software without restriction, including without limitation   *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
*  and/or sell copies of the Software, and to permit persons to whom the       *
*  Software is furnished to do so, subject to the following conditions:        *
*                                                                              *
*  The above copyright notice and this permission notice shall be included in  *
*  all copies or substantial portions of the Software.                         *
*                                                                              *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL     *
*  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER  *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
*  DEALINGS IN THE SOFTWARE.                                                   *
*                                                                              *
*******************************************************************************/

#include <stdlib.h>
#include <sysexits.h>
#include <set>
#include <ext/slist>

#include "messages.hpp"
#include "files.hpp"
#include "buildgen-xml/target.hpp"

Files *files;

#if defined(__linux__)
	const char *OS_STYLE = "linux";
	const char *OS_COMPLIANCE = "posix";
	const char *OS_KERNEL = "linux";
#elif defined(BSD)
	const char *OS_STYLE = "BSD";
	const char *OS_COMPLIANCE = "posix";
	#if defined(__FreeBSD__)
		const char *OS_KERNEL = "FreeBSD";
	#elif defined(__OpenBSD__)
		const char *OS_KERNEL = "OpenBSD";
	#elif defined(__NetBSD__)
		const char *OS_KERNEL = "NetBSD";
	#elif defined(__DragonFly__)
		const char *OS_KERNEL = "DragonFly";
	#endif
#elif defined(_WIN32)
	const char *OS_STYLE = "win32";
	const char *OS_COMPLIANCE = "win32";
	const char *OS_KERNEL = "win32";
#elif defined(__APPLE__)
	const char *OS_STYLE = "mac";
	const char *OS_COMPLIANCE = "posix";
	const char *OS_KERNEL = "darwin";
#else
	const char *OS_COMPLIANCE = "unknown";
	const char *OS_KERNEL = "unknown";
#endif

void checkAlloc ( void *p )
{
	if (!p)
	{
		msg::error("Error could not allocate memory");
		exit(EX_OSERR);
	}
}
