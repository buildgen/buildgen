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

#include "pathresolver.hpp"

/// Info about the filesystem as it relates to us.
class Files
{
public:
	char *project_root;
	char *lualibs_root;
	char *out_root;

	const char *infofilename;
	const char *rootfilename;
	char *config_file_system;
	char *config_file_user;
	const char *buildgen_root;

	ITargetManager * const manager;
	PathResolver resolver;
private:
	void init(const char *srcdir, const char *buildgen_root);
	void appendSlash(char **inputoutput);
public:
	Files(ITargetManager * const mgnr, const char *srcdir, const char *buildgen_root);
	~Files();
	std::queue<char*> infofile;

	bool fileExists (const char *path );

	void findInfoFile(void);
	void addDirectory(const char *path);
	void addInfoFile(const char *path);

	void findProjectRoot(void);
};

#endif
