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

Files::Files ( char *srcdir ):
	project_root(NULL),
	out_root(NULL),

	infofilename("Buildinfo"),
	rootfilename("Buildroot"),
	config_file_system("/etc/buildgen/buildgen"),
	config_file_user("~/.config/BuildGen"),
	buildgen_root("/usr/lib/buildgen")
{
	init(srcdir);
}

void Files::init ( char *srcdir )
{
	out_root = get_current_dir_name();
	appendSlash(&out_root);

	DIR *d = opendir(srcdir);
	fchdir(dirfd(d));
	project_root = get_current_dir_name(); // As a default.  The value will be copied if needed
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
		msg::error("Could not open Buildinfo file \"%s\"\n", normalizeFilename(Files::infofilename));
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
		msg::error("Could not add directory \"%s\".  Reason: \"%s\"\n",
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

	msg::log("DMakeinfo file added at \"%s\"\n", info);
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
			char *cur = get_current_dir_name();
			if (!cur[1]) // This is the root directory
			{
				free(cur);
				msg::error("Could not find project root\n");
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

	project_root = get_current_dir_name();
	Files::appendSlash(&project_root);

	msg::log("Project Root at \"%s\"\n", project_root);
}

char *Files::normalizeFilename( const char *path )
{
	char *n, *r;
	switch (path[0])
	{
	case '<': // Input
		if ( path[1] == '<' )
		{
			path++;
			break;
		}

		n = (char*)malloc((strlen(project_root)+strlen(path))*sizeof(char));
		strcpy(n, project_root);
		strcat(n, path+1);
		return prettyPath(n);
	case '>': // Output
		if ( path[1] == '>' )
		{
			path++;
			break;
		}

		n = (char*)malloc((strlen(out_root)+strlen(path))*sizeof(char));
		strcpy(n, out_root);
		strcat(n, path+1);
		return prettyPath(n);
	case '*': // System path
		if ( path[1] == '*' )
		{
			path++;
			break;
		}
		else  // If we don't use an else we get errors because of case jumps
		{     // and uniniltized vars.  This limits the scope.
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
					n = (char*)malloc((dlen+flen+1)*sizeof(char));
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

	n = get_current_dir_name();
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
			if ( i[1] == '/' || i[1] == '\0' ) // . current directory
			{
				i += 2;
			}
			else if ( i[2] == '.' && ( i[3] == '/' || i[3] == '\0' ) ) // .. go up
			{
				i += 3;
				while ( o != path+1 && *o != '/' ) // The +1 is to keep the
					o--;                           // leading slash
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
