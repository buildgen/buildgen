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
#include <locale.h>
#include <sysexits.h> // BSD recomended exit stati

#include <lua.hpp>

#include "globals.hpp"
#include "messages.hpp"
#include "mystring.hpp"

#include "commandline.hpp"
#include "buildgen-xml/target.hpp"
#include "lua-init.hpp"
#include "files.hpp"
#include "buildgen-xml/save.hpp"

char *findOurPath ( char *a ) // Turn argv[0] into a BuildGen path
{
	for ( char *b = a; *b != '\0'; b++ )
		if ( *b == '/' ) // Relitive or absolute.
			return a;

	return mstrcat('*', a); // System path
}

int main ( int argc, char **argv )
{
	std::vector<char*> cmd(argc);  // Grab command line before we mess with it.
	for (int i = argc; --i; )      // Note that 0 doesn't get hit
		cmd[i] = strdup(argv[i]);  // Or else getopt changes them.
	cmd[0] = findOurPath(argv[0]); // We will process this after files is init'ed.

	opt::get_options(&argc, &argv);

	files = new Files(opt::src_dir, cmd[0]);

	opt::do_options(); // May quit.

	std::set<std::string> runfiles;

	chdir(files->project_root);
	char *rootFileName = files->normalizeFilename(files->rootfilename);
	BuildGenLuaEnv lua(rootFileName);
	free(rootFileName);

	Target *regen = Target::newTarget("regen");
	regen->magic = 1;

	cmd[0] = files->normalizeFilename(cmd[0]);

	regen->addGenerator(cmd);
	regen->generator->addDescription("Regenerating build information");

	for ( int i = opt::defines.size()-1; i >= 0; i-- )
		lua.define(opt::defines[i].key, opt::defines[i].value);

	while (files->infofile.size())
	{
		std::pair<std::set<std::string>::iterator,bool> p(
			runfiles.insert(std::string(files->infofile.front()))
		);
		if (p.second) // New file
		{
			Target *t = Target::newTarget(files->infofile.front(), false);
			regen->addDependancy(t);

			lua.runFile(files->infofile.front());
		}

		files->infofile.pop();
	}

	fputs(XML::create(Target::targets).c_str(), opt::xml_out);
	opt::close_xml_out();

	return 0;
}
