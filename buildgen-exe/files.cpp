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

Files::Files ( const char *srcdir, char *buildgen_root ):
	project_root(NULL),
	out_root(NULL),

	infofilename("Buildinfo"),
	rootfilename("Buildroot"),
	config_file_system("/etc/buildgen/buildgen.conf"),
	config_file_user("~/.config/buildgen/buildgen.conf")
{
	init(srcdir, buildgen_root);
}

void Files::init ( const char *srcdir, char *buildgenroot )
{
	out_root = getcwd(NULL, 0);
	appendSlash(&out_root);

	/*** Get BuildGen root ***/
	char *br = normalizeFilename(buildgenroot);

	unsigned int ls = strlen(br);
	while ( br[ls] != '/' ) ls--;
	ls--;
	while ( br[ls] != '/' ) ls--;
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

		closedir(d);
	}
}

void Files::findInfoFile ( void )
{
	FILE *conf = fopen(Files::infofilename, "r");
	if (!conf)
	{
		msg::error("Could not open Buildinfo file \"%s\"", normalizeFilename(Files::infofilename));
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
			normalizeFilename(path), strerror(errno));
	}

	findInfoFile();

	fchdir(dirfd(d));
	closedir(d);
}

void Files::addInfoFile ( const char *path )
{
	char *info = normalizeFilename(path);
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
	FILE *root = fopen(rootfilename, "r");
	unsigned int i = 0;

	while (!root)
	{
		if ( i > 10 ) // Check for the root directory every once and a while.
		{
			char *cur = getcwd(NULL, 0);
			if (!cur[1]) // This is the root directory
			{
				free(cur);
				msg::error("Could not find project root");
				exit(EX_USAGE);
			}
			free(cur);

			i -= 5;
		}
		else i++;

		chdir("..");
		root = fopen(rootfilename, "r");
	}
	fclose(root);

	project_root = getcwd(NULL, 0);
	Files::appendSlash(&project_root);

	Target *t = Target::newTarget(normalizeFilename(rootfilename), false);
	Target::newTarget("regen")->addDependancy(t);

	msg::log("Project Root at \"%s\"", project_root);
}

char *Files::normalizeFilename( const char *path )
{
	switch (path[0])
	{
	case '<': // Input
			return prettyPath(mstrcat(project_root, path+1));
	case '>': // Output
		{
			return prettyPath(mstrcat(out_root, path+1));
		}
	case '@': // Auto path
		{
			char *p = mstrdup(path);
			p[0] = '!';
			char *b = normalizeFilename(p);
			free(p);
			char *n = myalloc(  strlen(b)
			                  - strlen(project_root)
			                  + strlen(out_root)
			                  + 1
			                 );
			strcpy(n, out_root);
			strcat(n, b+strlen(project_root)); // -1 is to include the slash.

			free(b);

			return prettyPath(n);
		}
	case '*': // System path
		{
			path = mstrdup(path);

			DIR *cwd  = opendir(".");

			char *pathdir = mstrdup(getenv("PATH"));
			char *nt = pathdir; // NULL-terminator
			bool moredirs = true;

			const char *exename = path + 1;
			while ( moredirs )
			{
				/*** NULL-terminate the end of the next dir ***/
				pathdir = nt;
				while ( *nt != '\0' && *nt != ':' )
					nt++;
				if (!*nt)
					moredirs = false;
				*(nt++) = '\0';

				/*** Check for executable ***/
				chdir(pathdir);
				FILE *e = fopen(exename, "r");
				if (e) // Found it.
				{
					char *n = mstrcat(pathdir, '/', exename);

					fchdir(dirfd(cwd));
					closedir(cwd);

					return prettyPath(n);
				}
			}

			fchdir(dirfd(cwd));
			closedir(cwd);
		}

		msg::error("System path \"%s\" not found.", path);
		exit(EX_DATAERR);
	case '/': // Already absolute
		return prettyPath(mstrdup(path));
	}

	// Regular relative path

	if ( *path == '!' ) path++; // We didn't want the first character
	                            // to be looked at.

	char *n = getcwd(NULL, 0);
	int cwdlen = strlen(n);

	n = (char*)realloc(n, (cwdlen+2+strlen(path))*sizeof(char));
	if ( cwdlen > 1 )
	{
		n[cwdlen]   = '/' ;
		n[cwdlen+1] = '\0';
	}
	strcat(n, path);

	return prettyPath(n);
}
#ifdef DEBUG
void _TEST_Files_normalizeFilename ( Files *of )
{
	Files f(*of);

	f.project_root = "/test/src/";
	f.out_root = "/test/build/";

	char *s, *e, *r;

	chdir("/home/");

	s = "/this/is/a/test/";
	e = "/this/is/a/test/";
	r = f.normalizeFilename(s);
	//printf("%s\n", r);
	assert( strcmp(r, e) == 0 );
	free(r);

	s = "!/this/is/a/test/";
	e = "/home/this/is/a/test/";
	r = f.normalizeFilename(s);
	//printf("%s\n", r);
	assert( strcmp(r, e) == 0 );
	free(r);

	s = "!!!/this/is/../is/a/test/";
	e = "/home/!!/this/is/a/test/";
	r = f.normalizeFilename(s);
	//printf("%s\n", r);
	assert( strcmp(r, e) == 0 );
	free(r);

	s = "*ls";
	e = "/bin/ls";
	r = f.normalizeFilename(s);
	//printf("%s\n", r);
	assert( strcmp(r, e) == 0 );
	free(r);

	s = "*env";
	e = "/usr/bin/env";
	r = f.normalizeFilename(s);
	//printf("%s\n", r);
	assert( strcmp(r, e) == 0 );
	free(r);

	s = ">file";
	e = "/test/build/file";
	r = f.normalizeFilename(s);
	//printf("%s\n", r);
	assert( strcmp(r, e) == 0 );
	free(r);

	s = ">dir/";
	e = "/test/build/dir/";
	r = f.normalizeFilename(s);
	//printf("%s\n", r);
	assert( strcmp(r, e) == 0 );
	free(r);

	s = ">dir/file";
	e = "/test/build/dir/file";
	r = f.normalizeFilename(s);
	//printf("%s\n", r);
	assert( strcmp(r, e) == 0 );
	free(r);

	s = "<file";
	e = "/test/src/file";
	r = f.normalizeFilename(s);
	//printf("%s\n", r);
	assert( strcmp(r, e) == 0 );
	free(r);

	s = "<dir/";
	e = "/test/src/dir/";
	r = f.normalizeFilename(s);
	//printf("%s\n", r);
	assert( strcmp(r, e) == 0 );
	free(r);

	s = "<dir/file";
	e = "/test/src/dir/file";
	r = f.normalizeFilename(s);
	//printf("%s\n", r);
	assert( strcmp(r, e) == 0 );
	free(r);
}
#endif

char *Files::prettyPath ( char *path )
{
	assert( path != NULL );
	char *i, *o;
	i = o = path;

	while ( *i != '\0' )
	{
		if ( *i != '/' ) *(o++) = *(i++);
		else if ( i[1] == '/' ) // 2 slashes together
		{
			i++; // remove them
		}
		else if ( i[1] == '.' )
		{
			if ( i[2] == '/' || i[2] == '\0' ) // . current directory
			{ /* /dir/./file */
			  /*     ^^^     */
				i += 2; // skip it, it doesn't tell us anything
			}
			else if ( i[2] == '.' && ( i[3] == '/' || i[3] == '\0' ) ) // .. go up
			{ /* /dir/otherdir/../file */
			  /*              ^^^^     */
				i += 3;
				if ( o > path ) o--; // If is for root directory check.
				while ( o > path && *o != '/' )
					o--;                           // leading slash
			}
			else // Just a file starting with a '.'.
			{
				*(o++) = *(i++);
			}
		}
		else
		{
			*o = '/'; // Regular path component
			o++;
			i++;
		}
	}
	*o = '\0';

	return path;
}
#ifdef DEBUG
void _TEST_Files_prettyPath ( void )
{
	char *s, *r;

	s = mstrdup("/this/is/a/test/");
	r = "/this/is/a/test/";
	assert( strcmp(Files::prettyPath(s), r) == 0 );
	free(s);

	s = mstrdup("//////this///////////test////////");
	r = "/this/test/";
	assert( strcmp(Files::prettyPath(s), r) == 0 );
	free(s);

	s = mstrdup("//////this/../../../../test////////");
	r = "/test/";
	assert( strcmp(Files::prettyPath(s), r) == 0 );
	free(s);

	s = mstrdup("/../this/is/./../this/./test");
	r = "/this/this/test";
	assert( strcmp(Files::prettyPath(s), r) == 0 );
	free(s);

	s = mstrdup("//////this/../../../../test////////");
	r = "/test/";
	assert( strcmp(Files::prettyPath(s), r) == 0 );
	assert( strcmp(Files::prettyPath(s), r) == 0 );
	assert( strcmp(Files::prettyPath(s), r) == 0 );
	free(s);


}
#endif
