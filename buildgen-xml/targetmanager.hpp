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

#ifndef TARGETMANAGER_H
#define TARGETMANAGER_H

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <string.h>

#include <iostream>
#include <set>
#include <vector>

#include "rapidxml/rapidxml.hpp"

#include "itargetmanager.hpp"

class TargetManager: public ITargetManager
{
public:
	struct comparator
	{
		bool operator ()(const Target *t1, const Target *t2) const
		{
			return strcmp(t1->path, t2->path) < 0;
		}
	};
	typedef std::set<Target*, comparator> TargetSet;
	TargetSet targets; ///< A lsit of all targets created

	virtual Target *newTarget ( const char *path );
	virtual Target *findTarget ( const char *path );
	virtual Target *changePath ( const char *oldp, const char *newp );

	virtual const std::set<const Target*> *allTargets(void);

	TargetManager();
	virtual ~TargetManager();
};

#endif
