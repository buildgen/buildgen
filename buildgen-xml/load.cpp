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
	msg::error("Input is not a valid BuildGen file.\n");
	exit(EX_DATAERR);
}

void XML::load ( std::istream &xml,
	char **buildGenRoot, char **outRoot, char **projectRoot )
{
	msg::log("Loading XML\n");
	std::string xmlbuf((std::istreambuf_iterator<char>(xml)), std::istreambuf_iterator<char>());

	xml_document<> doc;
	doc.parse<0>(const_cast<char*>(xmlbuf.c_str()));

	xml_node<> *root = doc.first_node(XML::rootNName);
	if (!root) die_badFile();

	if ( projectRoot || outRoot || buildGenRoot )
	{
		xml_node<> *meta = root->first_node(XML::metaNName);
		if (!meta) die_badFile();

		if (buildGenRoot)
		{
			xml_node<> *n = meta->first_node(XML::meta_buildGenRootName);
			if (!n) die_badFile();

			*buildGenRoot = (char*)realloc(buildGenRoot, n->value_size());
			strcpy(n->value(), *buildGenRoot);
		}
		if (outRoot)
		{
			xml_node<> *n = meta->first_node(XML::meta_outRootNName);
			if (!n) die_badFile();

			*outRoot = (char*)realloc(outRoot, n->value_size());
			strcpy(n->value(), *outRoot);
		}
		if (projectRoot)
		{
			xml_node<> *n = meta->first_node(XML::meta_projectRootNName);
			if (!n) die_badFile();

			*projectRoot = (char*)realloc(projectRoot, n->value_size());
			strcpy(n->value(), *projectRoot);
		}
	}

	xml_node<> *targ = root->first_node(XML::targetsNName);
	if (!targ) die_badFile();

	if ( xml_node<> *n = targ->first_node(XML::targetNName) )
	{
		do {
			Target::fromXML(n);
		} while ( n = n->next_sibling(XML::targetNName) );
	}
}
