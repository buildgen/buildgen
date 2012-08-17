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

#include <test.hpp>

#include "buildgen-exe/messages.hpp"
#include "buildgen-xml/target.hpp"

#include "buildgen-xml/common.hpp"

#include "targetmanager.hpp"
#include "itargemanager-test.hpp"

RUN_IFACE_TEST(ITargetManager, TargetManager);

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
#ifdef TEST
TEST(TargetManager, changePath)
{
	TargetManager m;
	Target *t = m.newTarget("path");

	ASSERT_STREQ(t->path, "path");

	m.changePath("path", "newPath");

	ASSERT_STREQ(t->path, "newPath");
}
#endif

std::set<const Target*> TargetManager::allTargets(void)
{
	return *(std::set<const Target*>*)&targets;
}

TargetManager::TargetManager()
{

}

TargetManager::~TargetManager()
{
	for ( TargetSet::const_iterator i = targets.begin();
	      i != targets.end();
	      i++)
		free(*i);
}
