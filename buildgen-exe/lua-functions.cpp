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

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <vector>
#include <queue>
#include <sysexits.h>

#include <lua.hpp>
#include <locale>
#include <math.h>
//#include <luabind/luabind.hpp>

#include "messages.hpp"
#include "lua-functions.hpp"
#include "globals.hpp"
#include "buildgen-xml/target.hpp"
#include "lua-init.hpp"
#include "info.h"

namespace LuaFunctions
{
bool statesaved = false;
void save_state(lua_State *L)
{
	msg::debug("1\n");
	lua_getglobal(L, "P");
	lua_getglobal(L, "_G");
	lua_setfield(L, 1, "_G");

	statesaved = true;
	msg::debug("2\n");
}

void load_state(lua_State *L)
{
	if (statesaved)
	{
		lua_getglobal(L, "P");
		lua_getfield(L, 1, "_G");
		lua_setglobal(L, "_G");
	}

	int s = luaL_loadfile(L, LUALIBS_ROOT"core.lua");
	if ( s == 0 )
	{
		// execute Lua program
		s = lua_pcall(L, 0, LUA_MULTRET, 0);
	}

	if (s) // Errors
	{
		msg::error("%s\n", lua_tostring(L, -1));
		lua_pop(L, 1); // remove error message
		exit(EX_DATAERR);
	}
}

namespace D
{

int add_depandancy (lua_State *L)
{
	if (!lua_isstring(L, 1))
		luaL_error(L, "D.addDependancy was given an argument that is not a path. Arg 1");
	if (!lua_isstring(L, 2))
		luaL_error(L, "D.addDependancy was given an argument that is not a path. Arg 2");

	char *targ = files->normalizeFilename(lua_tostring(L, 1));
	char *dep = files->normalizeFilename(lua_tostring(L, 2));

	Target *t = Target::newTarget(targ);
	t->addDependancy(Target::newTarget(dep));

	free(targ);
	free(dep);

	return 0;
}

int add_dir (lua_State *L)
{
	if (!lua_isstring(L, 1))
		luaL_error(L, "D.addDir was given an argument that is not a path.");

	files->addDirectory(lua_tostring(L, 1));

	return 0;
}

int add_generator (lua_State *L)
{
	luaL_checktype(L, 1, LUA_TTABLE); // Outputs
	luaL_checktype(L, 2, LUA_TTABLE); // Inputs
	luaL_checktype(L, 3, LUA_TTABLE); // Command line

	lua_Debug ar;
	lua_getstack(L, 1, &ar);
	lua_getinfo(L, "l", &ar);

	std::vector<const char*> gen(luaL_getn(L, 3));
	for ( unsigned int i = luaL_getn(L, 3); i >= 1; i-- )
	{
		lua_pushnumber(L, i);
		lua_gettable(L, 3);
		if (!lua_isstring(L, -1))
			luaL_error(L, "D.addGenerator was given a generator command this is not a string.");

		gen[i-1] = lua_tostring(L, -1);
		lua_pop(L, 1);
	}
	if (luaL_getn(L, 3)) gen[0] = files->normalizeFilename(gen[0]);

	std::vector<Target*> in(luaL_getn(L, 2));

	for ( unsigned int i = luaL_getn(L, 2); i >= 1; i-- ) // For each input
	{
		/*** Get the current filename ***/
		lua_pushnumber(L, i);
		lua_gettable(L, 2);
		if (!lua_isstring(L, -1))
			luaL_error(L, "D.addGenerator was given a source file this is not a string.");

		char *t = files->normalizeFilename(lua_tostring(L, -1));
		in[i-1] = Target::newTarget(t);
		free(t);
		lua_pop(L, 1);
	}

	for ( unsigned int i = luaL_getn(L, 1); i >= 1; i-- ) // For each output
	{
		/*** Get the current filename ***/
		lua_pushnumber(L, i);
		lua_gettable(L, 1);
		if (!lua_isstring(L, -1))
			luaL_error(L, "D.addGenerator was given an output file this is not a string.");

		char *tpath = files->normalizeFilename(lua_tostring(L, -1));
		Target *t = Target::newTarget(tpath);
		free(tpath);
		lua_pop(L, 1);

		if (luaL_getn(L, 3)) t->addGenerator(gen);
		for ( unsigned int i = in.size(); i--; )
			t->addDependancy(in[i]);
	}

	return 0;
}

int path (lua_State *L)
{
	if (!lua_isstring(L, 1))
		luaL_error(L, "path was asked to convert a path that is not a string");

	char *p = files->normalizeFilename(lua_tostring(L, 1));
	lua_settop(L, 0);
	lua_pushstring(L, p);
	free(p);

	return 1;
}

} // End of d
namespace S
{
void call_shutdown ( lua_State *L )
{
	lua_getglobal(L, "S");
	lua_pushstring(L, "_doShutdownFunctions");
	lua_gettable(L, -2);
	if (!lua_isfunction(L, -1))
		return;

	if (lua_pcall(L, 0, 0, 0)) // Errors
	{
		msg::error("%s\n", lua_tostring(L, -1));
		lua_pop(L, 1); // remove error message
		exit(EX_DATAERR);
	}

	lua_settop(L, 0);
}
} // End of S
} // End of LuaFunctions
