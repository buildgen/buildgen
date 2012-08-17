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
#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <sysexits.h> // BSD recomended exit stati

#include <lua.hpp>

#include "globals.hpp"
#include "messages.hpp"
#include "mystring.hpp"

#include "commandline.hpp"
#include "buildgen-xml/target.hpp"
#include "buildgen-xml/targetmanager.hpp"
#include "lua-init.hpp"
#include "files.hpp"
#include "buildgen-xml/save.hpp"
#include "lua-functions.hpp"

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
		cmd[i] = mstrdup(argv[i]);  // Or else getopt changes them.
	cmd[0] = findOurPath(argv[0]); // We will process this after files is init'ed.

	opt::get_options(&argc, &argv);

	TargetManager targetmanager;

	Files files(&targetmanager, opt::src_dir, cmd[0]);
	LuaFunctions::files = &files;

	opt::do_options(&files); // May not return.

	std::set<std::string> runfiles;

	chdir(files.project_root);
	char *rootFileName = files.resolver.normalizeFilename(files.rootfilename);
	BuildGenLuaEnv lua(&files, rootFileName);
	free(rootFileName);

	Target *regen = targetmanager.newTarget("regen");
	regen->magic = 1;

	cmd[0] = files.resolver.normalizeFilename(cmd[0]);

	regen->addGenerator(cmd);
	regen->generator->addDescription("Regenerating build information");

	for ( int i = cmd.size()-1; i--; )
		free(cmd[i]);

	for ( int i = opt::defines.size()-1; i >= 0; i-- )
		lua.define(opt::defines[i].key, opt::defines[i].value);

	while (files.infofile.size())
	{
		std::pair<std::set<std::string>::iterator,bool> p(
			runfiles.insert(std::string(files.infofile.front()))
		);
		if (p.second) // New file
		{
			Target *t = targetmanager.newTarget(files.infofile.front());
			regen->addDependancy(t);

			lua.runFile(files.infofile.front());
		}

		files.infofile.pop();
	}

	fputs(XML::create(targetmanager.targets, &files).c_str(), opt::xml_out);
	opt::close_xml_out();

	return 0;
}
