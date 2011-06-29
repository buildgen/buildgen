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
	char *out_root;

	const char *infofilename;
	const char *rootfilename;
	const char *config_file_system;
	const char *config_file_user;
	const char *buildgen_root;
private:
	void init(char *srcdir);
	void appendSlash(char **inputoutput);
public:
	Files(char *srcdir);
	std::queue<char*> infofile;

	void findInfoFile(void);
	void addDirectory(const char *path);
	void addInfoFile(const char *path);

	void findProjectRoot(void);

	char *normalizeFilename(const char *path);
	char *prettyPath(char *path);
};

#endif
