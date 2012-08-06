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
#include <math.h>
#include <string.h>
#include <sysexits.h>

#include <set>

#include <gtest/gtest.h>

#include "buildgen-exe/messages.hpp"
#include "buildgen-xml/target.hpp"

#include "buildgen-xml/common.hpp"

#include "targetmanager.hpp"

Target *TargetManager::newTarget ( const char *path )
{
	Target *t = new Target(this, path);
	std::pair<TargetSet::iterator,bool> r = targets.insert(t);

	if (!r.second) free(t); // It was already there.

	return *r.first;
}

Target *TargetManager::findTarget ( const char *path )
{
	Target t(this, path);
	TargetSet::iterator r = targets.find(&t);

	if ( r == targets.end() )
		return NULL;

	return *r;
}

Target *TargetManager::changePath(const char *oldp, const char *newp)
{
	Target *t = findTarget(oldp);
	targets.erase(t);
	free(t->path);
	t->path = strdup(newp);

	return t;
}
TEST(TargetManager, changePath)
{
	TargetManager m;
	Target *t = m.newTarget("path");

	ASSERT_STREQ(t->path, "path");

	m.changePath("path", "newPath");

	ASSERT_STREQ(t->path, "newPath");
}

std::set<const Target*> TargetManager::allTargets(void)
{
	return *(std::set<const Target*>*)&targets;
}
TEST(TargetManager, allTargets)
{
	TargetManager m;
	m.newTarget("1");
	m.newTarget("2");
	m.newTarget("3");
	m.newTarget("4");
	m.newTarget("5");

	const std::set<const Target*> tgts = m.allTargets();
	for ( std::set<const Target*>::iterator i = tgts.begin();
	      i != tgts.end();
	      i++)
	{
		const Target *t = *i;
	}
}

TargetManager::TargetManager()
{

}

TargetManager::~TargetManager()
{
}


TEST(TargetManager, Basic)
{
	TargetManager m;

	Target *t1 = m.newTarget("t1");

	ASSERT_EQ(t1, m.newTarget("t1"));
	ASSERT_EQ(t1, m.findTarget("t1"));

	Target *t2 = m.newTarget("t2");

	ASSERT_NE(t1, m.newTarget("t2"));
	ASSERT_NE(t1, m.findTarget("t2"));
	ASSERT_NE(t2, m.newTarget("t1"));
	ASSERT_NE(t2, m.findTarget("t1"));
}
