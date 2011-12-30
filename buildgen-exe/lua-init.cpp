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

	root_file = strdup(root);

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
	L = lua_open();
	if (!L)
	{
		msg::error("Could not create lua state.");
		exit(EX_SOFTWARE);
	}
	luaL_openlibs(L);

	lua_newtable(L);
	lua_setglobal(L, "D");
}

void BuildGenLuaEnv::dmakeify_lua ( void )
{
	lua_register(L, "_c_add_depandancy", &LuaFunctions::C::add_depandancy);
	lua_register(L, "_c_add_dir", &LuaFunctions::C::add_dir);
	lua_register(L, "_c_add_generator", &LuaFunctions::C::add_generator);
	lua_register(L, "_c_path", &LuaFunctions::C::path);

	lua_pushstring(L, files->lualibs_root);
	lua_setglobal(L, "_s_lualibs_root");
	lua_pushstring(L, OS_STRING);
	lua_setglobal(L, "_s_os");
#ifdef DEBUG
	lua_pushboolean(L, 1);
	lua_setglobal(L, "_s_debug");
#endif
}

void BuildGenLuaEnv::define( char *key, char *value )
{
	lua_getglobal(L, "D");

	if ( value == NULL ) lua_pushboolean(L, 1);
	else                 lua_pushstring(L, value);

	lua_setfield(L, -2, key);

	lua_pop(L, 1);
}

void BuildGenLuaEnv::doRunFile ( const char *path )
{
	char *d = strdup(path);
	checkAlloc(d);
	chdir(dirname(d));
	free(d);

	int s = luaL_loadfile(L, (char*)path);
	if ( s == 0 )
	{
		// execute Lua program
		s = lua_pcall(L, 0, LUA_MULTRET, 0);
	}

	if (s) // Errors
	{
		msg::error("%s", lua_tostring(L, -1));
		lua_pop(L, 1); // remove error message
		exit(EX_DATAERR);
	}

	//LuaFunctions::clean_up(L);
}

void BuildGenLuaEnv::runFile ( const char *path )
{
	doRunFile(corefile);

	if ( root_file != NULL ) // Run the root file.
		doRunFile(root_file);

	doRunFile(path);

	//LuaFunctions::clean_up(L);
}
