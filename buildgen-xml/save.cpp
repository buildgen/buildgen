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

std::string XML::create(TargetManager::TargetSet &targets, Files *files)
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
	for ( it = targets.begin(); it != targets.end(); ++it )
	{
		xml_node<> *n = (*it)->toXML(doc);
		if (n) targ->append_node(n);
	}

	std::string xml_as_string;
	print(std::back_inserter(xml_as_string), doc);

	free(timestamp);

	return xml_as_string;
}
