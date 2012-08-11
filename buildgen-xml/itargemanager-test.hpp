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

#ifndef ITARGEMANAGERTEST_HPP
#define ITARGEMANAGERTEST_HPP

#include "test.hpp"
#include "itargetmanager.hpp"

#ifdef TEST
IFACE_TEST(ITargetManager)
{
	{ ///// ITargetManager::newTarget(const char *);
		T m;
		Target *t = m.newTarget("path");

		ASSERT_STREQ("path", t->path);

		m.changePath("path", "newPath");

		ASSERT_STREQ("newPath", t->path);
	}
	{ ///// ITargetManager::allTargets(void);
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
	{ ///// Basic Usage.
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
}
#endif

#endif // ITARGEMANAGERTEST_HPP
