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
