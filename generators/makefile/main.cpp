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
#include <set>

#include "rapidxml/rapidxml.hpp"

#include "buildgen-exe/messages.hpp"
#include "buildgen-xml/target.hpp"
#include "buildgen-xml/load.hpp"

#include "commandline.hpp"
#include "makefile.hpp"


int main ( int argc, char **argv )
{
	opt::get_options(&argc, &argv);

	XML::Meta meta = XML::load(opt::xml_in);

	Target *t = Target::findTarget("regen");
	Target::targets.erase(t);
	t->path = "Makefile";
	t->magic = 0;
	Target::targets.insert(t);

	Target *n = Target::newTarget("regen");
	n->magic = 1;
	n->addDependancy(t);

	Makefile m(&Target::targets);
	opt::makefile_out << m.generate();
	opt::makefile_out.flush();

	return 0;
}
