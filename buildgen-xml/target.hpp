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

#include "itargetmanager.hpp"

class Generator;

class Target
{
public:
	ITargetManager *manager;
	char *path;  ///< The lcoation of the target.  This is an absolute value.
	short int magic; ///< If the target is a magic target.
	std::set<const Target*> depends; ///< The targets that this target depends on
	Generator *generator; ///< The generator used to create this target.

	Target(ITargetManager *mgnr, const char *path = NULL );
	Target(const Target &t);
	~Target( );
	void addDependancy(const Target *);
	void addDependancy(const char *);
	void addGenerator(Generator *gen);
	void addGenerator(std::vector<char*> cmd);
	void addGenerator(std::vector<const char*> cmd);

	static Target *fromXML(ITargetManager *mgnr, const rapidxml::xml_node<> *n);
	virtual rapidxml::xml_node<> *toXML(rapidxml::xml_document<> &d) const;

	Target &operator = (const Target&);

	bool operator > (const Target &c) const;
	bool operator >= (const Target &c) const;
	bool operator < (const Target &c) const;
	bool operator <= (const Target &c) const;
	bool operator == (const Target &c) const;
	bool operator != (const Target &c) const;
};

class Generator
{
public:
	Generator(void);
	Generator(const std::vector<const char*> &cmds);
	char *desc;
	void addDescription(const char *d);
	std::vector< std::vector<char*> > cmds;
	virtual void addCommand(const std::vector<const char*> &cmd);
	virtual rapidxml::xml_node<> *toXML(rapidxml::xml_document<> &d);
	static Generator *fromXML(const rapidxml::xml_node<> *n);
};

#endif
