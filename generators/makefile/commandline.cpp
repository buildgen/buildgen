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

#include <iostream>
#include <fstream>

#include <stdlib.h>
#include <getopt.h>
#include <sysexits.h>

#include "commandline.hpp"
#include "buildgen-exe/messages.hpp"

namespace opt
{
	std::ostream makefile_out(NULL);
	std::istream xml_in(std::cin.rdbuf());

	void get_options ( int *argc, char ***argv )
	{
		static struct option longopts[] = {
			{ "out",      required_argument,	NULL,	'o' },
		//	{ "verbose",  optional_argument,	NULL,	'v' },
		};

		int i;
		while ((i = getopt_long(*argc, *argv, "o:", longopts, NULL)) >= 0 )
		{
			switch (i)
			{
				case 'o': // Where to put the makefile.  Default is Makefile
					std::ofstream *makefile = new std::ofstream(optarg, std::ios::out | std::ios::binary);
					if (!*makefile)
					{
						msg::error("Could not open makefile output file.");
						exit(EX_CANTCREAT);
					}
					makefile_out.rdbuf(makefile->rdbuf());
					break;
			}
		}
		*argc -= optind; *argv += optind;

		if (__builtin_expect( *argc ,1))
		{
			std::ifstream *xml = new std::ifstream((*argv)[0], std::ifstream::in);
			(*argc)--; (*argv)--;
			if (!*xml)
			{
				msg::error("Could not open BuildGen input file.");
				exit(EX_NOINPUT);
			}
			xml_in.rdbuf(xml->rdbuf());
		}

		if (!makefile_out)
		{
			std::ofstream *makefile = new std::ofstream("Makefile", std::ios::out | std::ios::binary);
			if (!*makefile)
			{
				msg::error("Could not open makefile output file.");
				exit(EX_CANTCREAT);
			}
			makefile_out.rdbuf(makefile->rdbuf());
		}
	}
}
