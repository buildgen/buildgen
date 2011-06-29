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
#include <libgen.h>

#include <lua.hpp>

#include "globals.hpp"
#include "commandline.hpp"
#include "buildgen-xml/target.hpp"
#include "lua-init.hpp"
#include "files.hpp"
#include "buildgen-xml/save.hpp"

int main ( int argc, char **argv )
{
	//setlocale(LC_CTYPE, "en_ca.UTF-8");

	opt::get_options(&argc, &argv);

	files = new Files(opt::src_dir);

	std::set<std::string> runfiles;

	while (files->infofile.size())
	{
		std::pair<std::set<std::string>::iterator,bool> p(
			runfiles.insert(std::string(files->infofile.front()))
		);
		if (p.second) // New file
		{
			chdir(files->project_root);
			BuildGenLuaEnv lua(files->rootfilename);

			char *d = dirname(strdup(files->infofile.front()));
			chdir(d);
			free(d);

			lua.runFile(files->infofile.front());
		}

		files->infofile.pop();
	}

	fputs(XML::create(Target::targets).c_str(), opt::xml_out);

	return 0;
}
