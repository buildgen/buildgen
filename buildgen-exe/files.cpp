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

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sysexits.h>
#include <math.h> // BSD recomended exit stati
#include <errno.h>

#include "messages.hpp"
#include "files.hpp"
#include "buildgen-xml/target.hpp"
#include "info.h"

Files::Files ( char *srcdir, char *buildgen_root ):
	project_root(NULL),
	out_root(NULL),

	infofilename("Buildinfo"),
	rootfilename("Buildroot"),
	config_file_system("/etc/buildgen/buildgen"),
	config_file_user("~/.config/BuildGen")
{
	init(srcdir, buildgen_root);
}

void Files::init ( char *srcdir, char *buildgenroot )
{
	out_root = getcwd(NULL, 0);
	appendSlash(&out_root);

	char *br = normalizeFilename(buildgenroot);

	unsigned int ls = strlen(br);
	while ( br[ls] != '/' ) ls--;
	ls--;
	while ( br[ls] != '/' ) ls--;
	br[ls+1] = '\0';

	buildgen_root = strdup(br);

	free(br);

	unsigned int bgl = strlen(buildgen_root);
	unsigned int llrl = strlen(LUALIBS_ROOT);

	lualibs_root = (char*)malloc((bgl+llrl+1)*sizeof(char));
	strcpy(lualibs_root, buildgen_root);
	strcpy(lualibs_root+bgl, LUALIBS_ROOT);

	DIR *d = opendir(srcdir);
	if ( d == NULL )
	{
		msg::error("Can not switch to the source directory.");
		exit(EX_USAGE);
	}
	fchdir(dirfd(d));
	project_root = getcwd(NULL, 0); // As a default.  The value will be copied if needed
	findProjectRoot();
	fchdir(dirfd(d));
	findInfoFile();

	closedir(d);
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

	msg::log("DMakeinfo file added at \"%s\"", info);
}

void Files::appendSlash ( char **inputoutput )
{
	char *io = *inputoutput;
	int len = strlen(io);

	if ( io[len] == '/' ) return;

	io = (char*)realloc(io, (len+2*sizeof(char)));
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
		if (__builtin_expect( i > 10 , 0)) //@todo But in a macro for windows ( i think only gcc has this )
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
		{
			char *n = (char*)malloc((strlen(project_root)+strlen(path))*sizeof(char));
			strcpy(n, project_root);
			strcat(n, path+1);
			return prettyPath(n);
		}
	case '>': // Output
		{
			char *n = (char*)malloc((strlen(out_root)+strlen(path))*sizeof(char));
			strcpy(n, out_root);
			strcat(n, path+1);
			return prettyPath(n);
		}
	case '@': // Auto path
		{
			char *p = strdup(path);
			p[0] = '!';
			char *b = normalizeFilename(p);
			free(p);
			char *n = (char*)malloc((strlen(b)
							   - strlen(project_root)
							   + strlen(out_root)
							   + 1
							  )*sizeof(char));
			strcpy(n, out_root);
			strcat(n, b+strlen(project_root)); // -1 is to include the slash.

			free(b);

			return prettyPath(n);
		}
	case '*': // System path
		{
			path = strdup(path);

			DIR *cwd  = opendir(".");

			char *pathdir = strdup(getenv("PATH"));
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
					unsigned int dlen = strlen(pathdir);
					unsigned int flen = strlen(exename);
					char *n = (char*)malloc((dlen+flen+2)*sizeof(char));
					strcpy(n, pathdir);
					n[dlen] = '/';
					strcpy(n+dlen+1, exename);

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
		return prettyPath(strdup(path));
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

char *Files::prettyPath ( char *path )
{
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
				o--;
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
