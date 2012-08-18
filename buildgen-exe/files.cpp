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
#include <stdio.h>
#include <assert.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sysexits.h>
#include <math.h> // BSD recomended exit stati
#include <errno.h>

#include "info.h"
#include "globals.hpp"

#include "mystring.hpp"

#include "messages.hpp"
#include "files.hpp"
#include "buildgen-xml/target.hpp"

Files::Files (ITargetManager * const mgnr, const char *srcdir, const char *buildgen_root ):
	manager(mgnr),
	project_root(NULL),
	out_root(NULL),

	resolver("/", "/"),

	infofilename("Buildinfo"),
	rootfilename("Buildroot"),
	config_file_system(mstrdup("/etc/buildgen/conf.lua")),
	config_file_user(mstrdup("~/.config/buildgen/conf.lua"))
{
	init(srcdir, buildgen_root);
}

Files::~Files()
{
	free(config_file_system);
	free(config_file_user);
}

bool Files::fileExists(const char *path)
{
	FILE *f = fopen(path, "r");
	if ( f != NULL )
	{
		fclose(f);
		return true;
	}

	return false;
}

void Files::init ( const char *srcdir, const char *buildgenroot )
{
	out_root = getcwd(NULL, 0);
	appendSlash(&out_root);

	/*** Get BuildGen root ***/
	char *br = resolver.normalizeFilename(buildgenroot);

	unsigned int ls = strlen(br);
	while ( br[ls] != '/' ) ls--; // Remove the executable name.
	ls--;
	while ( br[ls] != '/' ) ls--; // Remove "bin".
	br[ls+1] = '\0';

	buildgen_root = mstrdup(br);
	free(br);

	/*** Get LuaLibs root ***/
	lualibs_root = mstrcat(buildgen_root, LUALIBS_ROOT);

	if (srcdir) // We don't need this if we are just listing directories.
	{
		DIR *d = opendir(srcdir);
		if ( d == NULL )
		{
			msg::error("Can not switch to the source directory.");
			exit(EX_USAGE);
		}
		fchdir(dirfd(d));

		findProjectRoot();

		fchdir(dirfd(d));
		findInfoFile();

		resolver = PathResolver(project_root, out_root);

		char *o = config_file_system;
		config_file_system = resolver.normalizeFilename(o);
		free(o);
		o = config_file_user;
		config_file_user = resolver.normalizeFilename(o);
		free(o);

		closedir(d);
	}
}

void Files::findInfoFile ( void )
{
	FILE *conf = fopen(Files::infofilename, "r");
	if (!conf)
	{
		msg::error("Could not open Buildinfo file \"%s\"",
		           resolver.normalizeFilename(Files::infofilename));
		exit(EX_NOINPUT);
	}
	fclose(conf);

	addInfoFile(Files::infofilename);
}

void Files::addDirectory( const char *path )
{
	DIR *d = opendir(".");

	if (chdir(path)) // Returns 0 on success
	{
		msg::error("Could not add directory \"%s\".  Reason: \"%s\"",
		           resolver.normalizeFilename(path), strerror(errno));
	}

	findInfoFile();

	fchdir(dirfd(d));
	closedir(d);
}

void Files::addInfoFile ( const char *path )
{
	char *info = resolver.normalizeFilename(path);
	infofile.push(info);

	msg::log("Buildinfo file added at \"%s\"", info);
}

void Files::appendSlash ( char **inputoutput )
{
	char *io = *inputoutput;
	int len = strlen(io);

	if ( io[len] == '/' ) return;

	io = myrealloc(io, len+2);
	io[len]   = '/' ;
	io[len+1] = '\0';

	*inputoutput = io;
}

void Files::findProjectRoot( void )
{
	unsigned int i = 0;
	while (!fileExists(rootfilename))
	{
		if ( i > 5 ) // Check for the root directory every once and a while.
		{
			char *cur = getcwd(NULL, 0);
			if ( cur[1] == '\0' ) // This is the root directory
			{
				free(cur);
				msg::error("Could not find project root");
				exit(EX_USAGE);
			}
			free(cur);

			i -= 3;
		}
		else i++;

		chdir("..");
	}

	project_root = getcwd(NULL, 0);
	Files::appendSlash(&project_root);

	Target *t = manager->newTarget(resolver.normalizeFilename(rootfilename));
	manager->newTarget("regen")->addDependancy(t);

	msg::log("Project Root at \"%s\"", project_root);
}
