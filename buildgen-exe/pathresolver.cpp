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

#include "pathresolver.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sysexits.h>
#include <assert.h>

#include "test.hpp"

#include "messages.hpp"

#include "mystring.hpp"

PathResolver::PathResolver(const char *project_root, const char *out_root):
	project_root(mstrdup(project_root)),
	project_root_len(strlen(project_root)),
	out_root(mstrdup(out_root)),
	out_root_len(strlen(out_root))
{
	assert(project_root[project_root_len-1] == '/');
	assert(out_root[out_root_len-1] == '/');
}

PathResolver::~PathResolver()
{
	free(project_root);
	free(out_root);
}

PathResolver &PathResolver::operator =(const PathResolver &pr)
{
	setProjectRoot(pr.project_root);
	setOutRoot(pr.out_root);
}

char *PathResolver::normalizeFilename( const char *path )
{
	switch (path[0])
	{
	case '<': // Input
			return prettyPath(mstrcat(project_root, path+1));
	case '>': // Output
			return prettyPath(mstrcat(out_root, path+1));
	case '~': // Output
		return prettyPath(mstrcat(getenv("HOME"), path+1));
	case '@': // Auto path
		{
			/***** Expand the path *****/
			char *p = mstrdup(path);
			char *pf = p;
			if ( p[1] != '/' ) p[0] = '!';
			else               p++;
			char *b = normalizeFilename(p);
			free(pf);

			char *n = NULL;

			size_t olen = strlen(out_root);
			size_t plen = strlen(project_root);
			size_t blen = strlen(b);

			if (!strncmp(b, out_root, olen))
			{
				n = b; // Already in the out directory.
			}
			else if (!strncmp(b, project_root, plen))
			{
				n = myalloc(blen - plen + olen + 1);

				strcpy(n,      out_root);
				strcpy(n+olen, b+plen);

				free(b);
			}
			else // Outside of our directories
			{
				n = myalloc(olen + blen + 1);

				strcpy(n,      out_root);
				strcpy(n+olen, b);

				free(b);
			}

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
#ifdef TEST
TEST(PathResolver, normalizeFilename)
{
	PathResolver f("/test/src/", "/test/build/");

#define T(e, s) { \
		char *r = f.normalizeFilename(s); \
		EXPECT_STREQ(e, r); \
		EXPECT_NE(s, r); \
		free(r); \
	}

	chdir("/home/");

	T("/this/is/a/test/", "/this/is/a/test/");
	T("/home/this/is/a/test/", "!/this/is/a/test/");
	T("/home/!!/this/is/a/test/", "!!!/this/is/../is/a/test/");
	T("/bin/ls", "*ls");
	T("/usr/bin/env", "*env");
	T("/test/build/file", ">file");
	T("/test/build/dir/", ">dir/");
	T("/test/build/dir/file", ">dir/file");
	T("/test/src/file", "<file");
	T("/test/src/dir/", "<dir/");
	T("/test/src/dir/file", "<dir/file");

#undef T
}
#endif

char *PathResolver::prettyPath ( char *path )
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

#ifdef TEST
TEST(PathResolver, prettyPath)
{
#define T(e, s) { \
		char *s2 = mstrdup(s); \
		for ( int i = 1; i <= 5; i++ ) /* Re-running shouldn't change */ \
		{                              /* the results.                */ \
			char *r  = PathResolver::prettyPath(s2); \
			EXPECT_STREQ(e, r) << "After run " << i; \
			EXPECT_EQ(s2, r)   << "After run " << i; \
		} \
		free(s2); \
	}

	T("/this/is/a/test/", "/this/is/a/test/");
	T("/this/test/", "//////this///////////test////////");
	T("/test/", "//////this/../../../../test////////");
	T("/this/this/test", "/../this/is/./../this/./test");
	T("/test/", "//////this/../../../../test////////");

#undef T
}
#endif

void PathResolver::setProjectRoot(const char *path)
{
	free(project_root);
	project_root = mstrdup(path);
	project_root_len = strlen(project_root);
}

void PathResolver::setOutRoot(const char *path)
{
	free(out_root);
	out_root = mstrdup(path);
	out_root_len = strlen(out_root);
}
