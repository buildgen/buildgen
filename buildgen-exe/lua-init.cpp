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
#include <sysexits.h> // BSD recomended exit stati
#include <libgen.h>

#include "messages.hpp"

#include "lua-init.hpp"
#include "lua-functions.hpp"

#include "info.h"

BuildGenLuaEnv::BuildGenLuaEnv ( const char *root )
{
	init();

	runFile(root);
	LuaFunctions::save_state(L);
}

BuildGenLuaEnv::~BuildGenLuaEnv ( )
{
	lua_close(L);
}

void BuildGenLuaEnv::init ( void )
{
	init_lua();
	dmakeify_lua();
}

void BuildGenLuaEnv::init_lua ( void )
{
	L = lua_open();
	luaL_openlibs(L);
}

void BuildGenLuaEnv::dmakeify_lua ( void )
{
	lua_register(L, "_d_add_depandancy", &LuaFunctions::D::add_depandancy);
	lua_register(L, "_d_add_dir", &LuaFunctions::D::add_dir);
	lua_register(L, "_d_add_generator", &LuaFunctions::D::add_generator);
	lua_register(L, "_d_path", &LuaFunctions::D::path);

	lua_pushstring(L, LUALIBS_ROOT);
	lua_setglobal(L, "_s_lualibs_root");
	lua_pushstring(L, OS_STRING);
	lua_setglobal(L, "_s_os");
}

void BuildGenLuaEnv::runFile ( const char *path )
{
	char *d = strdup(path);
	chdir(dirname(d));
	free(d);

	LuaFunctions::load_state(L);

	int s = luaL_loadfile(L, (char*)path);
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

	LuaFunctions::S::call_shutdown(L);
	//LuaFunctions::clean_up(L);
}
