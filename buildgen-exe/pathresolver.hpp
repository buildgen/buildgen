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

#ifndef PATHRESOLVER_HPP
#define PATHRESOLVER_HPP

#include <stdlib.h>

/** A Path Resolving class.
 *
 * This class contains functions that make equalivant paths identical.  It also
 * makes them pretty to print.
 */
class PathResolver
{
	char *project_root;
	size_t project_root_len;
	char *out_root;
	size_t out_root_len;

public:
	/** Create a PathResolver.
	 *
	 * \param project_root The path to be used as the project root when
	 *   resolving paths, this need not actually exist or be accessable.  This
	 *   string should have a trailing slash.
	 * \param out_root The path to be used as the out root when
	 *   resolving paths, this need not actually exist or be accessable.  This
	 *   string should have a trailing slash.
	 */
	PathResolver(const char *project_root, const char *out_root);

	/** Makes a BuildGen path absolute.
	  *
	  * Takes a path and finds the absolute meathod of expressing it.  This
	  * function performs the following translations:
	  * 	>dir/file
	  * 		Turns into the file "file" inside the directory "dir" inside
	  * 		the build directory.
	  * 	<dir/file
	  * 		Turns into the file "file" inside the directory "dir" inside
	  * 		the source directory.
	  *		@file
	  *			returns the "matching" filename in the build directory.  For
	  *			example, if this is called in "/src/mylib" it will return
	  *			"/build/mylib/file" (assumeing "/build" is your build directory).
	  *
	  * 	*prog
	  * 		Finds the file "prog" in the installed programs. (on posix
	  * 	systems finds the file in your $PATH)
	  *
	  * 		rel/file
	  * 	Relitive pathnames are turned into absolute pathnames.
	  * 	/this/file
	  * 		Absolute pathnames are left as is.
	  *
	  * \arg path The path to normalize.
	  * \returns A newly malloc'ed path.
	  *		\note This function does not return a failure value because it does
	  *			it's own error handling.
	  */
	char *normalizeFilename(const char *path);

	/** Dumb fixing of pathnames inplace.
	  *
	  * Removes unessary components from a path.
	  *
	  * Example:
	  *		/dir/dir2/dir3/../dir4/.././../file
	  *		becomes: /dir/file
	  *
	  * \arg path An absolute path to be prettified in place.
	  * \return path (the same value you put in).  This function does not fail.
	  */
	static char *prettyPath(char *path);
};

#endif // PATHRESOLVER_HPP
