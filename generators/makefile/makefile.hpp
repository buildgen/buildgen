// Copyright 2011-2012 Kevin Cox

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

#ifndef MAKEFILE_HPP
#define MAKEFILE_HPP

#include <set>
#include <list>

#include "buildgen-xml/target.hpp"

class Makefile
{
	const char *cwd;
	const unsigned int cwdlen;

	std::set<Target*, Target::comparator> *targets;
	std::list<Target*> generated;

	std::string relitiveName(std::string path);
	std::string escape(std::string path);

	std::string writeTarget(Target *t);
	std::string writeGenerator(Generator *g);

	std::string writeClean(void);
	std::string writeHelp(void);

public:
    Makefile( std::set<Target*, Target::comparator> *targets);
	std::string generate(void);
};

#endif
