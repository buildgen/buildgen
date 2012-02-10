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

#include "buildgen-xml/save.hpp"
#include "buildgen-xml/common.hpp"

#include <stdlib.h>
#include <time.h>
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"

#include "buildgen-exe/messages.hpp"
#include "buildgen-xml/target.hpp"
#include "buildgen-exe/globals.hpp"
#include "buildgen-exe/files.hpp"

using namespace rapidxml;

std::string XML::create(std::set<Target*, Target::comparator> &targets, Files *files)
{
	msg::log("Linking and Generating XML");

	char *timestamp = (char*)malloc(11*sizeof(char));
	sprintf(timestamp, "%ld", time(NULL));

	xml_document<> doc;    // character type defaults to char

	// xml declaration
	xml_node<> *n = doc.allocate_node(node_declaration);
	n->append_attribute(doc.allocate_attribute("version", "1.0"));
	n->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(n);

	xml_node<> *root = doc.allocate_node(node_element, XML::rootNName);
	root->append_attribute(doc.allocate_attribute(XML::versionAName, "0.1.0"));
	doc.append_node(root);

	/*** Meta info ***/
	xml_node<> *meta = doc.allocate_node(node_element, XML::metaNName);
	root->append_node(meta);

	meta->append_node(doc.allocate_node(node_element, XML::meta_projectRootNName, files->project_root));
	meta->append_node(doc.allocate_node(node_element, XML::meta_outRootNName, files->out_root));
	meta->append_node(doc.allocate_node(node_element, XML::meta_timeNName, timestamp));

	/*** Targets ***/

	xml_node<> *targ = doc.allocate_node(node_element, XML::targetsNName);
	root->append_node(targ);

	std::set<Target*>::iterator it;
	for ( it = Target::targets.begin(); it != Target::targets.end(); ++it )
	{
		xml_node<> *n = (*it)->toXML(doc);
		if (n) targ->append_node(n);
	}

	std::string xml_as_string;
	print(std::back_inserter(xml_as_string), doc);
	return xml_as_string;
}
