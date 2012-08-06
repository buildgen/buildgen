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
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sysexits.h>
#include <execinfo.h>

#include <set>

#include <gtest/gtest.h>

#include "buildgen-exe/messages.hpp"
#include "buildgen-xml/target.hpp"
#include "buildgen-xml/targetmanager.hpp"

#include "buildgen-xml/common.hpp"

Target::Target (ITargetManager *mgnr, const char *path ):
    manager(mgnr)
{
	if (!path) this->path = NULL;
	else       this->path = strdup(path);

	generator = NULL;
	magic = 0;
}
TEST(Target, Constructor)
{
	const char *p = "test";
	Target t(NULL, p);
	ASSERT_NE(t.path, p);
	ASSERT_STREQ(t.path, "test");
	ASSERT_FALSE(t.magic);
}

Target::Target(const Target &t):
    manager(t.manager)
{
	if (!t.path) this->path = NULL;
	else         this->path = strdup(t.path);

	generator = t.generator;
	magic = t.magic;

	depends = t.depends;
}
TEST(Target, CopyConstructor)
{
	const char *p1 = "test";
	Target t1(NULL, p1);

	std::vector<const char*> cmd;
	cmd.push_back("this");
	cmd.push_back("is");
	cmd.push_back("a");
	cmd.push_back("test");
	t1.generator = new Generator(cmd);

	t1.magic = 1;

	Target t2(t1);

	ASSERT_STREQ(t1.path, p1);
	ASSERT_STREQ(t2.path, p1);
	ASSERT_NE(t1.path, p1);
	ASSERT_NE(t2.path, p1);
	ASSERT_NE(t1.path, t2.path);

	ASSERT_EQ(t1.magic, t2.magic);
	ASSERT_EQ(t1.generator, t2.generator);
	ASSERT_EQ(t1.depends, t2.depends);
}

Target::~Target ( )
{
	free(path);
}

void Target::addDependancy( const Target *t )
{
	msg::log("Added dependacy \"%s\" to \"%s\"", t->path, path);

	if ( t->manager == this->manager ) // We can't depend on something from a
	{                                  // different manager.
		depends.insert(t);
	}
	else
	{
		Target *n = manager->newTarget(t->path);
		n->magic = t->magic;
		n->generator = t->generator;

		depends.insert(n);
	}
}
TEST(Target, addDependancy_Target)
{
	TargetManager m;

	Target *t1 = m.newTarget("t1");
	Target *t2 = m.newTarget("t2");

	Target t3(NULL, "test");

	t1->addDependancy(t2);
	t1->addDependancy(&t3);

	ASSERT_EQ(t1->depends.size(), 2);
	ASSERT_TRUE(m.findTarget("test") != NULL);
}


void Target::addDependancy( const char *path )
{
	msg::log("Added dependacy \"%s\" to \"%s\"", path, this->path);

	depends.insert(manager->newTarget(path));
}
TEST(Target, addDependancy_Path)
{
	TargetManager m;

	Target *t = m.newTarget("targ");

	t->addDependancy("t1");
	t->addDependancy("t2");
	t->addDependancy("t3");
	t->addDependancy("t4");
	t->addDependancy("t5");

	ASSERT_EQ(t->depends.size(), 5);
	ASSERT_TRUE(m.findTarget("t1") != NULL);
	ASSERT_TRUE(m.findTarget("t2") != NULL);
	ASSERT_TRUE(m.findTarget("t3") != NULL);
	ASSERT_TRUE(m.findTarget("t4") != NULL);
	ASSERT_TRUE(m.findTarget("t5") != NULL);
}

void Target::addGenerator( Generator *gen )
{
	msg::log("Added generator \"%s\" to \"%s\"", gen->cmds[0][0], path);

	generator = gen;
}

void Target::addGenerator(std::vector<char*> cmd)
{
	addGenerator(*(std::vector<const char*>*)&cmd);
}

void Target::addGenerator(std::vector<const char*> cmd)
{
	addGenerator(new Generator(cmd));
}

rapidxml::xml_node<> *Target::toXML ( rapidxml::xml_document<> &d ) const
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

	for ( std::set<const Target*>::iterator i = depends.begin(); i != depends.end(); ++i)
	{
		t->append_node(d.allocate_node(node_element, XML::target_dependsNName, (*i)->path));
	}

	return t;
}

Target &Target::operator =(const Target &t)
{
	manager = t.manager;

	free(path);
	path = strdup(t.path);

	depends = t.depends;

	generator = t.generator;
	magic = t.magic;

	return *this;
}
TEST(Target, AsignmentOperator)
{
	TargetManager m;
	const char *p = "targ";
	Target *t = m.newTarget(p);

	t->addDependancy("t1");
	t->addDependancy("t2");
	t->addDependancy("t3");
	t->addDependancy("t4");
	t->addDependancy("t5");

	std::vector<const char*> cmd;
	cmd.push_back("this");
	cmd.push_back("is");
	cmd.push_back("a");
	cmd.push_back("test");
	t->generator = new Generator(cmd);

	t->magic = 1;

	Target c(NULL, "garbage");
	c = *t;

	ASSERT_STREQ(t->path, c.path);
	ASSERT_NE(t->path, c.path);
	ASSERT_NE(p, c.path);
	ASSERT_NE(p, t->path);

	ASSERT_EQ(t->depends, c.depends);
	ASSERT_EQ(t->generator, c.generator);
}

Target *Target::fromXML ( ITargetManager *mgnr, const rapidxml::xml_node<> *src )
{
	using namespace rapidxml;
	xml_node<> *p = src->first_node(XML::target_outNName);
	Target *t = mgnr->newTarget(p->value());

	xml_attribute<> *magic = p->first_attribute(XML::target_out_magicAName);
	if (magic && !strcmp(magic->value(), "true")) t->magic = 1;

	if ( xml_node<> *n = src->first_node(XML::target_dependsNName) )
	{
		do {
			t->addDependancy(mgnr->newTarget(n->value()));
		} while ( n = n->next_sibling(XML::target_dependsNName) );
	}

	if ( xml_node<> *n = src->first_node(XML::target_generatorNName) )
		t->generator = Generator::fromXML(n);

	return t;
}

Generator::Generator():
	desc(NULL)
{
}

Generator::Generator( const std::vector<const char*> &cmd ):
	desc(NULL)
{
	addCommand(cmd);
}

void Generator::addDescription ( const char *d )
{
	free(desc);
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
