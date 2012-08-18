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

#include "libgen.h"

#include "makefile.hpp"

#include <algorithm>
#include <unistd.h>

Makefile::Makefile (ITargetManager *manager):
	cwd(getcwd(NULL, 0)),
	cwdlen(strlen(cwd)+1), // The one is for the trailing slash
	manager(manager),
	targets(manager->allTargets())
{
	cwd = (char*)realloc(cwd, (cwdlen+1)*sizeof(char));
	cwd[cwdlen++] = '/';
	cwd[cwdlen]   = '\0';
}

std::string Makefile::relitiveName(std::string path)
{
	if ( path.find(cwd) != std::string::npos ) // Found it
	{
		std::string s(path.c_str()+cwdlen);

		if ( s[0] == '\0' )
			s = '.';

		std::replace(s.begin(), s.end(), ' ', '?');

		return s;
	}

	std::replace(path.begin(), path.end(), ' ', '?');

	return path;
}

std::string Makefile::escape ( std::string path )
{
	for ( int i = path.length(); i--; ) // Go backwords go we don't have to
	{                                   // worry about the growth of the string.
		switch (path[i])
		{
		case ' ':
			path.replace(i, 2, "\\ ");
			break;
		case '\'':
			path.replace(i, 2, "\\'");
			break;
		case '"':
			path.replace(i, 2, "\\\"");
			break;
		case '\\':
			path.replace(i, 2, "\\\\");
			break;
		}
	}

	return path;
}

std::string Makefile::generate ( void )
{
	std::string out("# Generated by BuildGen Makefile Generator on ");
	time_t curtime = time(NULL);
	out += ctime(&curtime);
	out += "\n\n.DEFAULT_GOAL := all\n\n";

	for (std::set<const Target*>::iterator ti = targets.begin();
		  ti != targets.end();
		  ++ti
		)
	{
		out += writeTarget(*ti);
	}

	out += writeClean();
	out += writeHelp();

	return out;
}

std::string Makefile::writeTarget(const Target *t)
{
	std::string out;

	if ( (t->generator == NULL) && (!t->magic) ) return out; // This is an existing file

	if (t->magic)
	{
		out += ".PHONY: ";
		out += escape(std::string(t->path));
		out += "\n\n";
	}
	else
		generated.push_back(t); // We are creating it so it needs to be cleaned.

	out += escape(relitiveName(t->path));
	out += ": ";

	for (std::set<const Target *>::iterator di = t->depends.begin();
		  di != t->depends.end();
		  ++di
		)
	{
		const Target *d = *di;

		out += escape(relitiveName(d->path));
		out += " ";
	}
	if (t->generator) out += relitiveName(t->generator->cmds[0][0]);
	out += "\n";
	char *dup = strdup(relitiveName(t->path).c_str());
	std::string dir(dirname(dup));
	free(dup);
	if ( dir != "." )
	{
		out += "	@mkdir -p \'" + dir + "\'\n";
	}
	out += writeGenerator(t->generator);
	out += '\n'; // Skip a line

	return out;
}

std::string Makefile::writeGenerator(Generator *g)
{
	std::string out;

	if (g)
	{
		if ( g->desc != NULL ) out += "	@echo -e '\\e[1m"
		                              +std::string(g->desc)
		                              +"\\e[0m'\n";
		for ( unsigned int i = 0; i < g->cmds.size(); i++ )
		{
			out += '	';
			for ( unsigned int j = 0; j < g->cmds[i].size(); j++ )
			{
				out += '\'';
				out += g->cmds[i][j];
				out += "\' ";
			}
			out += '\n';
		}
	}

	return out;
}

std::string Makefile::writeClean (void)
{
	std::string out;

	Target *makefile = manager->findTarget("Makefile");

	out += ".PHONY: clean\n\n";

	out += "clean:\n";
	out += "	rm -rv";


	for ( std::list<const Target*>::const_iterator i = generated.begin();
	      i != generated.end();
	      i++)
	{
		const Target *t = *i;

		if ( !strncmp(cwd, t->path, cwdlen-1) && t != makefile ) // strcmp is so
		{ // that we only delete files in the build directory.
			out += " ";
			out += escape(relitiveName(t->path));
		}
	}

	out += " || true\n\n";

	return out;
}

std::string Makefile::writeHelp ( void )
{
	std::string out;

	out += ".PHONY: help\n\n";

	out += "help:\n";

	for ( std::set<const Target*>::iterator i = targets.begin();
	      i != targets.end();
	      i++
	    )
	{
		const Target *t = *i;

		if ( (t->generator == NULL) && (!t->magic) ) continue; // Not a generated file.

		out += "	@echo '";
		out += escape(relitiveName(t->path));
		out += "'\n";
	}

	return out;
}

Makefile::~Makefile()
{
	free(cwd);
}
