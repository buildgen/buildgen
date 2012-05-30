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
