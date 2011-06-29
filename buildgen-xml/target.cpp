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
	msg::log("Added dependacy \"%s\" to \"%s\"\n", d->path, path);

	depends.insert(d);
}

void Target::addGenerator(std::vector<const char*> cmd)
{
	msg::log("Added generator \"%s\" to \"%s\"\n", cmd[0], path);

	std::vector<char*> dup(cmd.size());
	for ( int i = cmd.size(); --i; )
	{
		dup[i] = strdup(cmd[i]);
	}
	dup[0] = strdup(cmd[0]);

	generator = new Generator(dup);
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

Generator::Generator( const std::vector<char*> &cmd ):
	Target(cmd[0]),
	cmd(cmd)
{
}

rapidxml::xml_node<> *Generator::toXML(rapidxml::xml_document<> &d)
{
	using namespace rapidxml;
	xml_node<> *g = d.allocate_node(node_element, XML::target_generatorNName);

	for ( unsigned int i = 0; i < cmd.size(); ++i )
	{
		xml_node<> *n = d.allocate_node(node_element,
		                                  XML::target_generator_argumentNName,
		                                  cmd[i]
		                               );

		char *pos;
		if (i) pos = d.allocate_string(NULL, (ceil(log(i)/log(10))+1)*sizeof(char));
		else   pos = d.allocate_string(NULL, 1*sizeof(char));
		sprintf(pos, "%d", i);

		n->append_attribute(d.allocate_attribute(XML::target_generator_posAName, pos));

		g->append_node(n);
	}

	return g;
}

Generator *Generator::fromXML ( const rapidxml::xml_node<> *src )
{
	using namespace rapidxml;
	std::vector<char*> cmd;

	if (src->first_node(XML::target_generator_argumentNName))
	{
		xml_node<> *n = src->last_node(XML::target_generator_argumentNName);
		// We are going backwords in hope of minimising vector resizes.
		do
		{
			unsigned int pos;
			sscanf(n->last_attribute(XML::target_generator_posAName)->value(), "%u", &pos);

			if ( pos >= cmd.size() )
				cmd.resize(pos+1, NULL);

			cmd[pos] = strdup(n->value());
		} while ( n = n->previous_sibling(XML::target_generator_argumentNName) );
	}

	for ( unsigned int i = cmd.size(); i--; )
	{
		if (!cmd[i])
		{
			msg::error("Generator command missing an argument.\n");
			exit(EX_DATAERR);
		}
	}

	return new Generator(cmd);
}

bool Target::check ( void )
{
	if ( path[0] == '*' && path[1] != '*' ) // System path
	{
		char *pathdir = strdup(getenv("PATH"));
		char *nt = pathdir; // NULL-terminator

		bool moredirs = true;

		char *exename = path + 1;
		while ( moredirs )
		{
			/*** NULL-terminate the end of the next dir ***/
			pathdir = nt;
			while ( *nt != '\0' && *nt != ':' )
				nt++;
			if (!*nt)
				moredirs = false;
			*(nt++) = '\0';

			/*** Check for executable ***/
			chdir(pathdir);
			FILE *e = fopen(exename, "r");
			if (e) // Found it.
			{
				fclose(e);
				return true;
			}
		}
	}
	else
	{
		FILE *e = fopen(path, "r");
		if (e) // Found it.
		{
			fclose(e);
			return true;
		}
	}

	msg::info("Target \"%s\" doesn't exist.  You will run into problems "
		"if you try to build anything that depends on it.\n", path);
	return false;
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
