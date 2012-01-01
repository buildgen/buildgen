// Copyright 2011-2012 Kevin Cox

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

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <vector>
#include <queue>
#include <sysexits.h>

#include <lua.hpp>
#include <locale>
#include <math.h>

#include "messages.hpp"
#include "info.h"
#include "globals.hpp"
#include "mystring.hpp"

#include "lua-functions.hpp"
#include "buildgen-xml/target.hpp"
#include "lua-init.hpp"

namespace LuaFunctions
{

namespace C
{

int add_depandancy (lua_State *L)
{
	if (!lua_isstring(L, 1))
		luaL_error(L, "C.addDependancy was given an argument that is not a path. Arg 1");
	if (!lua_isstring(L, 2))
		luaL_error(L, "C.addDependancy was given an argument that is not a path. Arg 2");
	if (lua_isnone(L, 3))
		lua_newtable(L);
	luaL_checktype(L, 3, LUA_TTABLE); // Options

	int magic = 0; // Check to see if this is a magic path.
	lua_pushstring(L, "magic"); // If the path to add to is magic.
	lua_gettable(L, 3);
	if ( lua_isboolean(L, -1) && lua_toboolean(L, -1))
		magic = 1;
	lua_pop(L, 1);

	lua_pushstring(L, "magicsrc"); // If the path being added is magic.
	lua_gettable(L, 3);
	if ( lua_isboolean(L, -1) && lua_toboolean(L, -1))
		magic &= 0x02;
	lua_pop(L, 1);

	char *targ = NULL;
	char *dep = NULL;
	if (!(magic & 0x01)) targ = files->normalizeFilename(lua_tostring(L, 1));
	else                 targ = mstrdup(lua_tostring(L, 1));
	if (!(magic & 0x02)) dep  = files->normalizeFilename(lua_tostring(L, 2));
	else                 dep  = mstrdup(lua_tostring(L, 2));

	Target *t = Target::newTarget(targ);
	Target *d = Target::newTarget(dep);
	if ( magic & 0x01 ) t->magic = 1;
	if ( magic & 0x02 ) t->magic = 1;
	t->addDependancy(d);

	free(targ);
	free(dep);

	return 0;
}

int add_dir (lua_State *L)
{
	if (!lua_isstring(L, 1))
		luaL_error(L, "C.addDir was given an argument that is not a path.");

	files->addDirectory(lua_tostring(L, 1));

	return 0;
}

int add_generator (lua_State *L)
{
	luaL_checktype(L, 1, LUA_TTABLE); // Outputs
	luaL_checktype(L, 2, LUA_TTABLE); // Inputs

	luaL_checktype(L, 3, LUA_TTABLE); // Command line
	lua_pushinteger(L, 1);
	if (lua_isstring(L, -1)) // Check the first element of the table.
	{
		lua_createtable(L, 1, 0); // Create the wrapper table.
		lua_insert(L, 3);         // Put it at the proper index
		lua_pushinteger(L, 1);    // Push a 1 for the index of the settable()
		lua_pushvalue(L, 4);      // Copy the old table.
		lua_remove(L, 4);         // Delete the original old table.
		lua_settable(L, 3);       // wrapper[1] = oldtable
	}
	lua_pop(L, 1); // Pop the result of the test.

	if (lua_isnone(L, 4))
		lua_newtable(L);
	luaL_checktype(L, 4, LUA_TTABLE); // Options

	lua_Debug ar;
	lua_getstack(L, 1, &ar);
	lua_getinfo(L, "l", &ar);

	Generator *gen = new Generator();

	char *generatorCmd = NULL;
	const char *desc = NULL;

	lua_pushstring(L, "description");
	lua_gettable(L, 4);
	if (lua_isstring(L, -1))
		desc = lua_tostring(L, -1);
	lua_pop(L, 1);

	if ( desc != NULL ) gen->addDescription(desc);

	/*** Get Command ***/
	std::vector<const char*> cmd;
	for ( unsigned int i = lua_objlen(L, 3); i >= 1; i-- )
	{
		lua_pushnumber(L, i);
		lua_gettable(L, 3);

		int curcmd = lua_gettop(L);

		cmd.resize(lua_objlen(L, curcmd));
		for ( unsigned int j = lua_objlen(L, curcmd); j >= 1; j-- )
		{
			lua_pushnumber(L, j);
			lua_gettable(L, curcmd);
			if (!lua_isstring(L, -1))
				luaL_error(L, "C.addGenerator was given a generator command that is not a string.");

			cmd[j-1] = lua_tostring(L, -1);
			lua_pop(L, 1);
		}
		if (lua_objlen(L, curcmd))
		{
			generatorCmd = files->normalizeFilename(cmd[0]);
			cmd[0]       = generatorCmd;
		}

		gen->addCommand(cmd);
		lua_pop(L, 1);
	}

	std::vector<Target*> in(luaL_getn(L, 2));

	/*** Get Inputs ***/
	for ( unsigned int i = lua_objlen(L, 2); i >= 1; i-- ) // For each input
	{
		/*** Get the current filename ***/
		lua_pushnumber(L, i);
		lua_gettable(L, 2);
		if (!lua_isstring(L, -1))
			luaL_error(L, "C.addGenerator was given a source file that is"
						  "not a string."
					   );

		char *t = files->normalizeFilename(lua_tostring(L, -1));
		in[i-1] = Target::newTarget(t);
		free(t);
		lua_pop(L, 1);
	}

	/*** Get Outputs ***/
	for ( unsigned int i = lua_objlen(L, 1); i >= 1; i-- ) // For each output
	{
		/*** Get the current filename ***/
		lua_pushnumber(L, i);
		lua_gettable(L, 1);
		if (!lua_isstring(L, -1))
			luaL_error(L, "C.addGenerator was given an output file this is not a string.");

		char *tpath = files->normalizeFilename(lua_tostring(L, -1));
		Target *t = Target::newTarget(tpath);
		free(tpath);
		lua_pop(L, 1);

		if (lua_objlen(L, 3)) t->addGenerator(gen);
		for ( unsigned int i = in.size(); i--; )
			t->addDependancy(in[i]);
	}

	free(generatorCmd);

	return 0;
}

int path (lua_State *L)
{
	if (!lua_isstring(L, 1))
		luaL_error(L, "path was asked to convert a path that is not a string");

	char *p = files->normalizeFilename(lua_tostring(L, 1));
	files->prettyPath(p);
	lua_pushstring(L, p); // Push our result.
	free(p);

	return 1;
}

} // End of d
} // End of LuaFunctions
