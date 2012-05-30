// Copyright 2011-2012 Kevin Cox

/*******************************************************************************
*                                                                              *
*  This software is provided 'as-is', without any express or implied           *
*  warranty. In no event will the authors be held liable for any damages       *
*  arising from the use of this software.                                      *
*                                                                              *
*  Permission is granted to anyone to use this software for any purpose,       *
*  including commercial applications, and to alter it and redistribute it      *
*  freely, subject to the following restrictions:                              *
*                                                                              *
*  1. The origin of this software must not be misrepresented; you must not     *
*     claim that you wrote the original software. If you use this software in  *
*     a product, an acknowledgment in the product documentation would be       *
*     appreciated but is not required.                                         *
*                                                                              *
*  2. Altered source versions must be plainly marked as such, and must not be  *
*     misrepresented as being the original software.                           *
*                                                                              *
*  3. This notice may not be removed or altered from any source distribution.  *                                                           *
*                                                                              *
*******************************************************************************/

#include <stdlib.h>
#include <sysexits.h>
#include <set>
#include <ext/slist>

#include "messages.hpp"
#include "files.hpp"
#include "buildgen-xml/target.hpp"

//Files *files;

#if defined(__linux__)
	const char *OS_STYLE = "linux";
	const char *OS_COMPLIANCE = "posix";
	const char *OS_KERNEL = "linux";
	const char *OS_TOOLSET = "GNU";
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
	const char *OS_TOOLSET = "GNU";
#elif defined(_WIN32)
	const char *OS_STYLE = "win32";
	const char *OS_COMPLIANCE = "win32";
	const char *OS_KERNEL = "win32";
	const char *OS_TOOLSET = "microsoft";
#elif defined(__CYGWIN__)
	const char *OS_STYLE = "win32";
	const char *OS_COMPLIANCE = "posix";
	const char *OS_KERNEL = "win32";
	const char *OS_TOOLSET = "cygwin";
#elif defined(__APPLE__)
	const char *OS_STYLE = "mac";
	const char *OS_COMPLIANCE = "mac";
	const char *OS_KERNEL = "darwin";
	const char *OS_TOOLSET = "GNU";
#else
	const char *OS_STYLE = "unknown";
	const char *OS_COMPLIANCE = "unknown";
	const char *OS_KERNEL = "unknown";
	const char *OS_TOOLSET = "unknown";
#endif

void checkAlloc ( void *p )
{
	if (!p)
	{
		msg::error("Error could not allocate memory");
		exit(EX_OSERR);
	}
}
