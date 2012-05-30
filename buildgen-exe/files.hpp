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

#ifndef FILES_HPP
#define FILES_HPP

#include <queue>
#include "buildgen-xml/target.hpp"

/// Info about the filesystem as it relates to us.
class Files
{
public:
	char *project_root;
	char *lualibs_root;
	char *out_root;

	const char *infofilename;
	const char *rootfilename;
	const char *config_file_system;
	const char *config_file_user;
	const char *buildgen_root;
private:
	void init(const char *srcdir, char *buildgen_root);
	void appendSlash(char **inputoutput);
public:
	Files(const char *srcdir, char *buildgen_root);
	std::queue<char*> infofile;

	void findInfoFile(void);
	void addDirectory(const char *path);
	void addInfoFile(const char *path);

	void findProjectRoot(void);

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

void _TEST_Files_normalizeFilename ( Files *f );
void _TEST_Files_prettyPath(void);

#endif
