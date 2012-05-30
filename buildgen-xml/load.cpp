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

#include <stdlib.h>
#include <iostream>
#include <streambuf>
#include <sysexits.h>
#include "rapidxml/rapidxml.hpp"

#include "buildgen-exe/messages.hpp"

#include "buildgen-xml/target.hpp"
#include "buildgen-xml/load.hpp"
#include "buildgen-xml/common.hpp"
#include "common.hpp"
#include "target.hpp"

using namespace rapidxml;

void die_badFile ( void )
{
	msg::error("Input is not a valid BuildGen file.");
	exit(EX_DATAERR);
}

XML::Meta XML::load ( std::istream &xml )
{
	msg::log("Loading XML");
	std::string xmlbuf((std::istreambuf_iterator<char>(xml)), std::istreambuf_iterator<char>());

	xml_document<> doc;
	doc.parse<0>(const_cast<char*>(xmlbuf.c_str()));

	xml_node<> *root = doc.first_node(XML::rootNName);
	if (!root) die_badFile();

	xml_node<> *meta = root->first_node(XML::metaNName);
	if (!meta) die_badFile();

	Meta metaObj;

	xml_node<> *n = meta->first_node(XML::meta_outRootNName);
	if (!n) die_badFile();

	metaObj.outRoot = (char*)malloc(n->value_size());
	strcpy(n->value(), metaObj.outRoot);

	n = meta->first_node(XML::meta_projectRootNName);
	if (!n) die_badFile();

	metaObj.projectRoot = (char*)malloc(n->value_size());
	strcpy(n->value(), metaObj.projectRoot);

	n = meta->first_node(XML::meta_timeNName);
	if (!n) die_badFile();
	sscanf(n->value(), "%ld", &metaObj.time);

	xml_node<> *targ = root->first_node(XML::targetsNName);
	if (!targ) die_badFile();

	if ( n = targ->first_node(XML::targetNName) )
	{
		do {
			Target::fromXML(n);
		} while ( n = n->next_sibling(XML::targetNName) );
	}

	return metaObj;
}
