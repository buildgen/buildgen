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

#ifndef ITARGETMANAGER_H
#define ITARGETMANAGER_H

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <string.h>

#include <iostream>
#include <set>
#include <vector>

class Target;
class Generator;

/** A Target Manager
 * A target manager holds and owns targets.  It ensures that there are no
 * duplicates and keeps the internal refrences valid.  The Targets returned by
 * this interface can be modified in any way except changing their path.
 */
class ITargetManager
{
public:
	/** Add a new target.
	 * This function returns a target that corrisponds to the passed in path.
	 * It will create a target if it does not exist already or just return the
	 * existing one.  Please note that targets returned by this are not
	 * nessairaly new and may be magic, have dependancies or other features set
	 * already.
	 *
	 * This is considered a write operation.
	 *
	 * \param path The path to the target as a string, this should be absolute.
	 *   This value is copied (the ownership of the passed in string is
	 *   retained).
	 * \return A pointer to a target.
	 */
	virtual Target *newTarget ( const char *path ) = 0;

	/** Find a target.
	 *
	 * \param path The path to the target as a string.
	 * \return A pointer to a target or `NULL` if the target was not found.
	 */
	virtual Target *findTarget ( const char *path ) = 0;

	/** Get all targets.
	 * This pointer is to a list of targets.  This is a copy and will NOT be
	 * updated and will not update the ITargetManager.
	 *
	 * \return The set of targets.  The targets inside are owned by the
	 *   ITargetManager.
	 */
	virtual std::set<Target*> allTargets(void) = 0;
};

#endif
