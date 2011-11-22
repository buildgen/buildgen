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

#ifndef COMMANDLINE_HPP
#define COMMANDLINE_HPP

#include <iostream>

namespace opt
{
	extern std::ostream makefile_out;
	extern std::istream xml_in;

	extern void get_options(int *argc, char ***argv);
}

#endif
