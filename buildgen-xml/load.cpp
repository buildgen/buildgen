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

	xml_node<> *n = meta->first_node(XML::meta_buildGenRootNName);
	if (!n) die_badFile();

	metaObj.buildGenRoot = (char*)malloc(n->value_size());
	strcpy(n->value(), metaObj.buildGenRoot);

	n = meta->first_node(XML::meta_outRootNName);
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
