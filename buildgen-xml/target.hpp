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

#ifndef TARGET_H
#define TARGET_H

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <string.h>

#include <iostream>
#include <set>
#include <vector>

#include "rapidxml/rapidxml.hpp"

class Generator;

class Target
{
public:
	struct comparator
	{
		bool operator ()(Target* t1, Target* t2) const
		{
			return strcmp(t1->path, t2->path) < 0;
		}
	};

	static std::set<Target*, Target::comparator> targets; ///< A lsit of all targets created

	static Target *newTarget ( const char *path, bool autodepend = true );
	static Target *findTarget ( const char *path );

	char *path;  ///< The lcoation of the target.  This is an absolute value.
	short int magic; ///< If the target is a magic target.
	std::set<Target*> depends; ///< The targets that this target depends on
	Generator *generator; ///< The generator used to create this target.
private:
	void init(bool autodepend);
public:

	Target(const char *path = NULL, bool autodepend = true  );
	~Target( );
	void addDependancy(Target*);
	void addGenerator( Generator *gen );
	void addGenerator(std::vector<char*> cmd);

	static Target *fromXML(const rapidxml::xml_node<> *n);
	virtual rapidxml::xml_node<> *toXML(rapidxml::xml_document<> &d);

	bool operator > (const Target &c) const;
	bool operator >= (const Target &c) const;
	bool operator < (const Target &c) const;
	bool operator <= (const Target &c) const;
	bool operator == (const Target &c) const;
	bool operator != (const Target &c) const;
};

class Generator : public Target
{
public:
	Generator( void );
	Generator( const std::vector<const char*> &cmds );
	char *desc;
	void addDescription ( const char *d );
	std::vector< std::vector<char*> > cmds;
	void addCommand ( const std::vector<const char*> &cmd );
	virtual rapidxml::xml_node<> *toXML(rapidxml::xml_document<> &d);
	static Generator *fromXML(const rapidxml::xml_node<> *n);
};

#endif
