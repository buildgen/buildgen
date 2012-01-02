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
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sysexits.h> // BSD recomended exit stati
#include <libgen.h>

#include "info.h"
#include "globals.hpp"
#include "messages.hpp"

#include "mystring.hpp"

#include "lua-init.hpp"
#include "lua-functions.hpp"

BuildGenLuaEnv::BuildGenLuaEnv ( const char *root ):
	root_file(NULL)
{
	assert( root != NULL );

	root_file = mstrdup(root);

	init();
}

BuildGenLuaEnv::~BuildGenLuaEnv ( )
{
	assert(L);

	lua_close(L);
	L = NULL;
}

void BuildGenLuaEnv::init ( void )
{
	const char *cf = "core.lua";
	corefile = mstrcat(files->lualibs_root, cf);

	init_lua();
	dmakeify_lua();
}

void BuildGenLuaEnv::init_lua ( void )
{
	L = luaL_newstate();
	if (!L)
	{
		msg::error("Could not create lua state.");
		exit(EX_SOFTWARE);
	}
	luaL_openlibs(L);
}

void BuildGenLuaEnv::dmakeify_lua ( void )
{
	lua_settop(L, 0);

	/*** C ***/
	lua_newtable(L); // Index: 1

	lua_pushcfunction(L, &LuaFunctions::C::add_depandancy);
	lua_setfield(L, 1, "addDependancy");
	lua_pushcfunction(L, &LuaFunctions::C::add_dir);
	lua_setfield(L, 1, "addDir");
	lua_pushcfunction(L, &LuaFunctions::C::add_generator);
	lua_setfield(L, 1, "addGenerator");
	lua_pushcfunction(L, &LuaFunctions::C::path);
	lua_setfield(L, 1, "path");

	lua_setglobal(L, "C");

	/*** S ***/
	lua_newtable(L); // Index: 1

	lua_pushstring(L, files->lualibs_root);
	lua_setfield(L, 1, "lualibsRoot");

	lua_newtable(L); // Index: 2

	lua_pushstring(L, OS_STYLE);
	lua_setfield(L, 2, "style");
	lua_pushstring(L, OS_KERNEL);
	lua_setfield(L, 2, "kernel");
	lua_pushstring(L, OS_COMPLIANCE);
	lua_setfield(L, 2, "compliance");

	lua_setfield(L, 1, "os");
	lua_setglobal(L, "S");

	/*** D ***/
	lua_newtable(L);
	lua_setglobal(L, "D");

	/*** P ***/
	lua_newtable(L);
	lua_setglobal(L, "P");

	/*** Save the current "clean" environment ***/

	lua_pushvalue(L, LUA_GLOBALSINDEX);
	lua_setfield(L, LUA_REGISTRYINDEX, "buildgen_G");
}

void BuildGenLuaEnv::define( char *key, char *value )
{
	lua_getglobal(L, "D");

	if ( value == NULL ) lua_pushboolean(L, 1);
	else                 lua_pushstring(L, value);

	lua_setfield(L, -2, key);

	lua_pop(L, 1);
}

void BuildGenLuaEnv::clenseEnvironment ( void )
{
	lua_newtable(L);

	lua_newtable(L);
	lua_getfield(L, LUA_REGISTRYINDEX, "buildgen_G");
	lua_setfield(L, -2, "__index");

	lua_setmetatable(L, -2);

	lua_replace(L, LUA_GLOBALSINDEX); // Replace the global table with our proxy.
}

void BuildGenLuaEnv::doRunFile ( const char *path )
{
	char *d = mstrdup(path);
	chdir(dirname(d));
	free(d);

	int s = luaL_loadfile(L, (char*)path);
	if ( s == 0 )
    {
		s = lua_pcall(L, 0, LUA_MULTRET, 0); // execute Lua program
	}

	if (s) // Errors
	{
		msg::error("%s", lua_tostring(L, -1));
		lua_pop(L, 1); // remove error message
		exit(EX_DATAERR);
	}
}

void BuildGenLuaEnv::runFile ( const char *path )
{
	clenseEnvironment();

	doRunFile(corefile);

	if ( root_file != NULL ) // Run the root file.
		doRunFile(root_file);

	doRunFile(path);
}
