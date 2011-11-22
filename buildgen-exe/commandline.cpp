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

#include <vector>

#include <getopt.h>
#include <sysexits.h>

#include "commandline.hpp"
#include "globals.hpp"
#include "messages.hpp"
#include "info.h"

void help ( void )
{
	puts("Usage:                                                                ");
	puts("  --define <key>=<value>, -D <key>=<value>                            ");
	puts("  --define <flag>, -D <flag>                                          ");
	puts("    Defines a variable for use in the configuration files.  The actual");
	puts("    use of the values depends on the project.  Some common values are ");
	puts("    listed below.  Check the projects documentation for the           ");
	puts("    definitions they support.                                         ");
	puts("      -D debug                                                        ");
	puts("        This flag sets the build to debug mode.  The default is to turn off");
	puts("        optimization and enable debuging and profiling.  The project can do");
	puts("        whatever they like with this value or may ingore it.");
	puts("      -D 'prefix=/opt/proj/'");
	puts("        Sets the default install directory to '/opt/proj' (or whatever you)");
	puts("        provide.");
	puts("  --generator <name>, -g <name>");
	puts("    Sets the generator to use.  <name> must be an executable in your path or");
	puts("    the full path to the executable.  The default is system dependant.");
	puts("  --help, -h");
	puts("    Print this message and quit.");
	puts("  --out <file>, -o <file>");
	puts("    Instead of passing the BuildGen xml output directly to a generator put it");
	puts("    in <file>.");
	puts("  --verbose, -v");
	puts("  --verbose <level>, -v <level>");
	puts("    Set verbosity level to <level>.  If no argument is given the verbosity");
	puts("    is raised by 1 for each time this argument is passed.");
	puts("  --version");
	puts("    Print the version of BuildGen and exit.");
	exit(0);
}

void version ( void )
{
	printf("buildgen (BuildGen) %s\n", VERSION);
	exit(0);
}

namespace opt
{
	FILE *xml_out = stdout;

	std::vector<Definition> defines;

	char *src_dir;

	void get_options ( int *argc, char ***argv )
	{
		char *gen = NULL;
		int flag = 0;

		static struct option longopts[] = {
			{ "define",    required_argument, NULL, 'D' },
			{ "generator", optional_argument, NULL, 'g' },
			{ "help",            no_argument, NULL, 'h' },
			{ "out",       optional_argument, NULL, 'o' },
			{ "verbose",   optional_argument, NULL, 'v' },
			{ "version",         no_argument, &flag, 1  },
			{ NULL,                        0, NULL,  0  },
		};

		int i;
		while ((i = getopt_long(*argc, *argv, "hD:g:o:v::", longopts, NULL)) >= 0 )
		{
			switch (i)
			{
			case 0:
				switch (flag)
				{
				case 1:
					version();
				}
				break;
			case 'D': // Define a build property
				{ // Hide variables
				char *v = optarg;

				Definition d;
				d.value = NULL;

				while ( *v != '\0' )
				{
					if ( *v == '=' )
					{
						*v = '\0';
						v++;

						d.value = v;

						break;
					}

					v++;
				}

				d.key = optarg;

				defines.push_back(d);
				} // End hide variables
				break;
			case 'g': // What generator to use
				gen = optarg;
				break;
			case 'h': // What generator to use
				help();
			case 'o': // Where to put the xml.  Default is stdout
				xml_out = fopen(optarg, "w");
				if (!xml_out)
				{
					msg::error("Could not open XML output file.");
					exit(EX_CANTCREAT);
				}
				break;
			case 'v': // Verbosity level
				if ( optarg )
				{
					while ( *optarg == 'v' )
					{
						msg::verbosity++;
						optarg++;
					}

					int v = 0;
					sscanf(optarg, "%d", &v);
					msg::verbosity = v;
				}
				else msg::verbosity++;
			}
		}
		*argc -= optind; *argv += optind;

		if (__builtin_expect( *argc ,1))
		{
			src_dir = (*argv)[0];
			(*argc)--; (*argv)--;
		}
		else src_dir = ".";

		if ( xml_out == stdout )
		{
			if ( gen == NULL ) xml_out = popen(DEFALUT_GENERATOR, "w");
			else if (!strcmp(gen, "makefile")) xml_out = popen("gen-makefile", "w");
		}
	}
}
