// Copyright 2011 Kevin Cox

/*******************************************************************************
*    This file is part of BuildGen.                                            *
*                                                                              *
*    BuildGen is free software: you can redistribute it and/or modify          *
*    it under the terms of the GNU Lesser General Public License as published  *
*    by the Free Software Foundation, either version 3 of the License, or      *
*    (at your option) any later version.                                       *
*                                                                              *
*    BuildGen is distributed in the hope that it will be useful,               *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*    GNU Lesser General Public License for more details.                       *
*                                                                              *
*    You should have received a copy of the GNU Lesser General Public License  *
*    along with BuildGen.  If not, see <http://www.gnu.org/licenses/>.         *
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
