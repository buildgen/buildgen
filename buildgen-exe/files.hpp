// Copyright 2011 Kevin Cox

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
	void init(char *srcdir, char *buildgen_root);
	void appendSlash(char **inputoutput);
public:
	Files(char *srcdir, char *buildgen_root);
	std::queue<char*> infofile;

	void findInfoFile(void);
	void addDirectory(const char *path);
	void addInfoFile(const char *path);

	void findProjectRoot(void);

	/** Makes a BuildGen filename absolute.
//	  *
	  * Takes a path and finds the absolute meathod of expressing it.  This
	  * function performs the following translations:
	  * 	>dir/file
	  * 		Turns into the file "file" inside the directory "dir" inside
	  * 		the build directory.
	  * 	<dir/file
	  * 		Turns into the file "file" inside the directory "dir" inside
	  * 		the source directory.
	  *		*file
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
	  * \return path (the same value you put in)
	  */
	char *prettyPath(char *path);
};

#endif
