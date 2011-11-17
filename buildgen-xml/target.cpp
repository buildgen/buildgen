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

#include <math.h>
#include <string.h>
#include <sysexits.h>

#include <set>

#include "buildgen-exe/messages.hpp"
#include "buildgen-xml/target.hpp"

#include "buildgen-xml/common.hpp"

Target Target::all;

std::set<Target*, Target::comparator> Target::targets; // All of the targets

Target::Target ( const char *path )
{
	if (!path) this->path = NULL;
	else       this->path = strdup(path);

	init();
}

Target::~Target ( )
{
	free(path);
}

void Target::init()
{
	generator = NULL;
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
Target *Target::newTarget ( const char *path )
{
	Target *t = new Target(path);

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

	t->append_node(d.allocate_node(node_element, XML::target_outNName, path));

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
	Target *t = Target::newTarget(src->first_node(XML::target_outNName)->value());

	if ( xml_node<> *n = src->first_node(XML::target_dependsNName) )
	{
		do {
			t->addDependancy(Target::newTarget(n->value()));
		} while ( n = n->next_sibling(XML::target_dependsNName) );
	}

	if ( xml_node<> *n = src->first_node(XML::target_generatorNName) )
		t->generator = Generator::fromXML(n);

	return t;
}

Generator::Generator( void ):
	Target()
{
}

Generator::Generator( const std::vector<const char*> &cmd ):
	Target(cmd[0])
{
	addCommand(cmd);
}

rapidxml::xml_node<> *Generator::toXML(rapidxml::xml_document<> &d)
{
	using namespace rapidxml;
	xml_node<> *g = d.allocate_node(node_element, XML::target_generatorNName);

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

	if (src->first_node(XML::target_generator_commandNName))
	{
		cmd.empty();

		xml_node<> *n = src->first_node(XML::target_generator_commandNName);
		do
		{
			if (n->first_node(XML::target_generator_commandNName))
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

	for ( unsigned int i = cmd.size(); i--; )
	{
		if (!cmd[i])
		{
			msg::error("Generator command missing an argument.");
			exit(EX_DATAERR);
		}
	}

	return new Generator(cmd);
}

void Generator::addCommand ( const std::vector<const char *> &cmd )
{
	std::vector<char*> n(cmd.size());

	for ( unsigned int i = cmd.size()-1; i--; )
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
