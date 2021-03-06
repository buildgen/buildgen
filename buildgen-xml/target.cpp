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

#include "test.hpp"

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
#ifdef TEST
TEST(Target, Constructor)
{
	const char *p = "test";
	Target t(NULL, p);
	EXPECT_NE(p, t.path) << "Path was not coppied.";
	EXPECT_STREQ("test", t.path);
	EXPECT_FALSE(t.magic);
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

	EXPECT_STREQ(p1, t1.path);
	EXPECT_STREQ(p1, t2.path);
	EXPECT_NE(p1, t1.path) << "Path was not coppied.";
	EXPECT_NE(p1, t2.path) << "Path was not coppied.";
	EXPECT_NE(t1.path, t2.path);

	EXPECT_EQ(t1.magic, t2.magic);
	EXPECT_EQ(t1.generator, t2.generator);
	EXPECT_EQ(t1.depends, t2.depends);
}
#endif

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

#ifdef TEST
TEST(Target, addDependancy_Target)
{
	TargetManager m;

	Target *t1 = m.newTarget("t1");
	Target *t2 = m.newTarget("t2");

	Target t3(NULL, "test");

	t1->addDependancy(t2);
	t1->addDependancy(&t3);

	EXPECT_EQ(2, t1->depends.size());
	EXPECT_TRUE(m.findTarget("test") != NULL);
}
#endif

void Target::addDependancy( const char *path )
{
	msg::log("Added dependacy \"%s\" to \"%s\"", path, this->path);

	depends.insert(manager->newTarget(path));
}
#ifdef TEST
TEST(Target, addDependancy_Path)
{
	TargetManager m;

	Target *t = m.newTarget("targ");

	t->addDependancy("t1");
	t->addDependancy("t2");
	t->addDependancy("t3");
	t->addDependancy("t4");
	t->addDependancy("t5");

	EXPECT_EQ(5, t->depends.size());
	EXPECT_TRUE(m.findTarget("t1") != NULL);
	EXPECT_TRUE(m.findTarget("t2") != NULL);
	EXPECT_TRUE(m.findTarget("t3") != NULL);
	EXPECT_TRUE(m.findTarget("t4") != NULL);
	EXPECT_TRUE(m.findTarget("t5") != NULL);
}
#endif

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
#ifdef TEST
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

	EXPECT_STREQ(t->path, c.path);
	EXPECT_NE(t->path, c.path);
	EXPECT_NE(p, c.path);
	EXPECT_NE(p, t->path);

	EXPECT_EQ(t->depends, c.depends);
	EXPECT_EQ(t->generator, c.generator);
}
#endif

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
#ifdef TEST
TEST(Generator, DefaultConstructor)
{
	Generator g;

	EXPECT_EQ(0, g.cmds.size());
}
#endif

Generator::Generator( const std::vector<const char*> &cmd ):
	desc(NULL)
{
	addCommand(cmd);
}
#ifdef TEST
TEST(Generator, CmdConstructor)
{
	std::vector<const char*> cmd;
	cmd.push_back("this");
	cmd.push_back("is");
	cmd.push_back("a");
	cmd.push_back("test");
	Generator g(cmd);

	EXPECT_EQ(1, g.cmds.size());
	EXPECT_EQ(4, g.cmds[0].size());
}
#endif

void Generator::addDescription ( const char *d )
{
	free(desc);
	desc = strdup(d);
}
#ifdef TEST
TEST(Generator, addDescription)
{
	const char *d = "This is the description.";
	Generator g;
	g.addDescription(d);

	EXPECT_STREQ(d, g.desc);
	EXPECT_NE(d, g.desc) << "Description was not coppied.";
}
#endif

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

	xml_node<> *n = src->first_node(XML::target_generator_descriptionNName);

	if ( n != NULL )
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
#ifdef TEST
TEST(Generator, addCommand)
{
	std::vector<const char*> cmd1;
	cmd1.push_back("this");
	cmd1.push_back("is");
	cmd1.push_back("a");
	cmd1.push_back("test");

	std::vector<const char*> cmd2;
	cmd2.push_back("this");
	cmd2.push_back("is");
	cmd2.push_back("a");
	cmd2.push_back("second");
	cmd2.push_back("test");

	Generator g;
	g.addCommand(cmd1);

	EXPECT_EQ(1, g.cmds.size());
	EXPECT_EQ(4, g.cmds[0].size());

	g.addCommand(cmd2);
	EXPECT_EQ(2, g.cmds.size());
	EXPECT_EQ(4, g.cmds[0].size());
	EXPECT_EQ(5, g.cmds[1].size());

	g.addCommand(cmd1);
	EXPECT_EQ(3, g.cmds.size());
	EXPECT_EQ(4, g.cmds[0].size());
	EXPECT_EQ(5, g.cmds[1].size());
	EXPECT_EQ(4, g.cmds[2].size());
}
#endif

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
#ifdef TEST
TEST(Target, ComparisonOperator)
{
	Target t1(NULL, "1 low");
	Target t2(NULL, "5 mid");
	Target t3(NULL, "5 mid");
	Target t4(NULL, "9 high");

	EXPECT_LT(t1, t2);
	EXPECT_LT(t1, t3);
	EXPECT_LT(t1, t4);

	EXPECT_GT(t2, t1);
	EXPECT_EQ(t2, t3);
	EXPECT_LT(t2, t4);

	EXPECT_GT(t3, t1);
	EXPECT_EQ(t3, t2);
	EXPECT_LT(t3, t4);

	EXPECT_GT(t4, t1);
	EXPECT_GT(t4, t2);
	EXPECT_GT(t4, t3);
}
#endif
