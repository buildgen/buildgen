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

#include <math.h>
#include <string.h>
#include <sysexits.h>

#include <set>

#include "buildgen-exe/messages.hpp"
#include "buildgen-xml/target.hpp"

#include "buildgen-xml/common.hpp"

std::set<Target*, Target::comparator> Target::targets; // All of the targets

Target::Target ( const char *path, bool autodepend )
{
	if (!path) this->path = NULL;
	else       this->path = strdup(path);

	init(autodepend);
}

Target::~Target ( )
{
	free(path);
}


Target *regen = NULL;
void Target::init( bool autodepend )
{
	generator = NULL;
	magic = 0;

	if ( regen == NULL && autodepend )
		regen = Target::newTarget("regen", false); // All targets depend on this.

	if (autodepend)
		addDependancy(regen); // All targets depend on this
}

/// Find a target
/**
 * Returns the target pointed to by \a path or \c NULL if it doesn't exist.
 *
 * \param path The location
 * \return a pointer to the target.
 */
Target *Target::findTarget ( const char *path )
{
	Target *t = new Target(path);
	return const_cast<Target*>(*targets.find(t));
}

/// Create a new target
/**
 * If the path give already relates to a target that target will be
 * returned else a new one will be created.
 *
 * \param path the loacation of the target.  This must be an absolute path.
 */
Target *Target:: newTarget ( const char *path, bool autodepend )
{
	Target *t = new Target(path, autodepend);

	std::pair<std::set<Target*>::iterator,bool> r(targets.insert(t)); // Crashes if we assign right away

	if (!*r.first)
	{
		delete t;
	}

	return const_cast<Target*>(*r.first);
}

void Target::addDependancy(Target* d)
{
	msg::log("Added dependacy \"%s\" to \"%s\"", d->path, path);

	depends.insert(d);
}

void Target::addGenerator( Generator *gen )
{
	msg::log("Added generator \"%s\" to \"%s\"", gen->cmds[0][0], path);

	generator = gen;
}

void Target::addGenerator(std::vector<char*> cmd)
{
	std::vector<const char*> dup(cmd.size());
	for ( int i = cmd.size(); --i; )
	{
		dup[i] = strdup(cmd[i]);
	}
	dup[0] = strdup(cmd[0]);

	addGenerator(new Generator(dup));
}

rapidxml::xml_node<> *Target::toXML ( rapidxml::xml_document<> &d )
{
	if ( !generator && !depends.size()) return NULL; // This is an existing file

	using namespace rapidxml;

	xml_node<> *t = d.allocate_node(node_element, XML::targetNName);
	xml_node<> *p = d.allocate_node(node_element, XML::target_outNName, path);
	if (magic)
	{
		xml_attribute<> *m = NULL;
		m = d.allocate_attribute(XML::target_out_magicAName, "true");
		p->append_attribute(m);
	}

	t->append_node(p);

	if (generator)
		t->append_node(generator->toXML(d));

	for ( std::set<Target*>::iterator i = depends.begin(); i != depends.end(); ++i)
	{
		t->append_node(d.allocate_node(node_element, XML::target_dependsNName, (*i)->path));
	}

	return t;
}
Target *Target::fromXML ( const rapidxml::xml_node<> *src )
{
	using namespace rapidxml;
	xml_node<> *p = src->first_node(XML::target_outNName);
	Target *t = Target::newTarget(p->value(), false);

	xml_attribute<> *magic = p->first_attribute(XML::target_out_magicAName);
	if (magic && !strcmp(magic->value(), "true")) t->magic = 1;

	if ( xml_node<> *n = src->first_node(XML::target_dependsNName) )
	{
		do {
			t->addDependancy(Target::newTarget(n->value(), false));
		} while ( n = n->next_sibling(XML::target_dependsNName) );
	}

	if ( xml_node<> *n = src->first_node(XML::target_generatorNName) )
		t->generator = Generator::fromXML(n);

	return t;
}

Generator::Generator( void ):
	Target(),
	desc(NULL)
{
}

Generator::Generator( const std::vector<const char*> &cmd ):
	Target(cmd[0]),
	desc(NULL)
{
	addCommand(cmd);
}

void Generator::addDescription ( const char *d )
{
	desc = strdup(d);
}

rapidxml::xml_node<> *Generator::toXML(rapidxml::xml_document<> &d)
{
	using namespace rapidxml;
	xml_node<> *g = d.allocate_node(node_element, XML::target_generatorNName);

	g->append_node(d.allocate_node(node_element,
	                XML::target_generator_descriptionNName,
	                desc
	));

	for ( unsigned int i = 0; i < cmds.size(); ++i )
	{
		xml_node<> *c = d.allocate_node(node_element, XML::target_generator_commandNName);

		for ( unsigned int j = 0; j < cmds[i].size(); ++j )
		{
			xml_node<> *n = d.allocate_node(node_element,
			                                 XML::target_generator_command_argumentNName,
			                                 cmds[i][j]
			                               );

			char *pos;
			if (j) pos = d.allocate_string(NULL, (ceil(log(j)/log(10))+1)*sizeof(char));
			else   pos = d.allocate_string(NULL, 1*sizeof(char));
			sprintf(pos, "%d", j);

			n->append_attribute(d.allocate_attribute(XML::target_generator_command_argument_posAName, pos));

			c->append_node(n);
		}

		g->append_node(c);
	}

	return g;
}

Generator *Generator::fromXML ( const rapidxml::xml_node<> *src )
{
	using namespace rapidxml;
	std::vector<const char*> cmd;

	Generator *g = new Generator();

	xml_node<> *n = NULL;

	if ( n = src->first_node(XML::target_generator_descriptionNName))
	{
		g->addDescription(n->value());
	}

	if (src->first_node(XML::target_generator_commandNName))
	{
		cmd.empty();

		n = src->first_node(XML::target_generator_commandNName);
		do
		{
			if (n->first_node(XML::target_generator_command_argumentNName))
			{
				xml_node<> *o = n->last_node(XML::target_generator_command_argumentNName);
				// We are going backwords in hope of minimising vector resizes.
				do
				{
					unsigned int pos;
					sscanf(o->last_attribute(XML::target_generator_command_argument_posAName)->value(), "%u", &pos);

					if ( pos >= cmd.size() )
						cmd.resize(pos+1, NULL);

					cmd[pos] = strdup(o->value());
				} while ( o = o->previous_sibling(XML::target_generator_command_argumentNName) );
			}

			g->addCommand(cmd);
		} while ( n = n->next_sibling(XML::target_generator_commandNName) );
	}

	return g;
}

void Generator::addCommand ( const std::vector<const char *> &cmd )
{
	std::vector<char*> n(cmd.size());

	for ( unsigned int i = cmd.size(); i--; )
		n[i] = strdup(cmd[i]);

	if ( cmds.size() == 0 )
		path = n[0];

	cmds.push_back(n);
}

bool Target::operator > (const Target &c) const
{
	return strcmp(this->path, c.path) > 0;
}
bool Target::operator >= (const Target &c) const
{
	return strcmp(this->path, c.path) >= 0;
}
bool Target::operator < (const Target &c) const
{
	return strcmp(this->path, c.path) < 0;
}
bool Target::operator <= (const Target &c) const
{
	return strcmp(this->path, c.path) <= 0;
}
bool Target::operator == (const Target &c) const
{
	return strcmp(this->path, c.path) == 0;
}
bool Target::operator != (const Target &c) const
{
	return strcmp(this->path, c.path) == 0;
}
