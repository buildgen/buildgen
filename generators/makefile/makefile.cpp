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

#include "libgen.h"

#include "makefile.hpp"

#include <algorithm>

Makefile::Makefile ( std::set<Target *, Target::comparator> *targets ):
	targets(targets),
	cwd(get_current_dir_name()),
	cwdlen(strlen(cwd)+1) // The one is to mimic the trailing slash
{

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

std::string Makefile::generate ( void )
{
	std::string out("# Generated by BuildGen Makefile Generator on ");
	time_t curtime = time(NULL);
	out += ctime(&curtime);
	out += "\n\n.DEFAULT_GOAL := all\n\n";

	for ( std::set<Target *, Target::comparator>::iterator ti = targets->begin();
		  ti != targets->end();
		  ++ti
		)
	{
		out += writeTarget(*ti);
	}

	return out;
}

std::string Makefile::writeTarget(Target *t)
{
	std::string out;

	if ( !t->generator && !t->depends.size()) return out; // This is an existing file

	out += relitiveName(t->path);
	out += ": ";

	for ( std::set<Target *, Target::comparator>::iterator di = t->depends.begin();
		  di != t->depends.end();
		  ++di
		)
	{
		Target *d = *di;

		out += relitiveName(d->path);
		out += " ";
	}
	if (t->generator) out += relitiveName(t->generator->path);
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
		out += '	';
		for ( unsigned int i = 0; i < g->cmd.size(); i++ )
		{
			out += '\'';
			out += g->cmd[i];
			out += "\' ";
		}
		out += '\n';
	}

	return out;
}
