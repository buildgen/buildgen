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
#include "pathresolver.hpp"

#include "lua-functions.hpp"
#include "buildgen-xml/target.hpp"
#include "lua-init.hpp"

#ifdef __CYGWIN__

	#include <sys/cygwin.h>

	static char *toWindowsPath ( const char *path, bool freep = false )
	{
		msg::debug("Converting: %s\n", path);

		size_t size = cygwin_conv_path(CCP_POSIX_TO_WIN_A, path, NULL, 0);
		if ( size < 0 )
		{
			msg::error("Error cygwin_conv_path failed (%d).", size);
			exit(EX_OSERR);
		}
		char *winpath = (char*)malloc(size);
		checkAlloc(winpath);

		if (cygwin_conv_path(CCP_POSIX_TO_WIN_A, path, winpath, size))
		{
			msg::error("Error cygwin_conv_path failed.");
			exit(EX_OSERR);
		}

		if (freep) free(const_cast<char*>(path));

		msg::debug("Converted: %s\n", winpath);
		return winpath;
	}
	static char *toUnixPath ( const char *path, bool freep = false )
	{
		msg::debug("Converting back: %s\n", path);

		size_t size = cygwin_conv_path(CCP_WIN_A_TO_POSIX|CCP_RELATIVE, path, NULL, 0);
		if ( size < 0 )
		{
			msg::error("Error cygwin_conv_path failed (%d).", size);
			exit(EX_OSERR);
		}
		char *upath = (char*)malloc(size);
		checkAlloc(upath);

		if (cygwin_conv_path(CCP_WIN_A_TO_POSIX|CCP_RELATIVE, path, upath, size))
		{
			msg::error("Error cygwin_conv_path failed.");
			exit(EX_OSERR);
		}

		if (freep) free(const_cast<char*>(path));

		msg::debug("Converted: %s\n", upath);
		return upath;
	}

	#define WINP(x)    toWindowsPath(x, false)
	#define WINPF(x)   toWindowsPath(x, true)
	#define UNIXP(x)   toUnixPath(x, false)
	#define UNIXPF(x)  toUnixPath(x, true)
	#define CYGFREE(x) free(const_cast<char*>(x))
#else
	#define WINP(x)    (x)
	#define WINPF(x)   (x)
	#define UNIXP(x)   (x)
	#define UNIXPF(x)  (x)
	#define CYGFREE(x)
#endif

namespace LuaFunctions
{
	Files *files;

int error_handler(lua_State *L)
{
	lua_Debug d;
	char buf[1024];

	char *msg = mstrdup(lua_tostring(L, -1));
	mstrapp(&msg, "\n\nStack trace:\n~~~~~~~~~~~~~~~~~~~~\n");

	for ( unsigned int l = 0; lua_getstack(L, l, &d); l++)
	{
		lua_getinfo(L, "nSl", &d);
		snprintf(buf, 1024, "%s:%d: %s\n", d.short_src, d.currentline, d.name);
		mstrapp(&msg, buf);
	}
	mstrapp(&msg, "~~~~~~~~~~~~~~~~~~~~\n");

	lua_pushstring(L, msg);
	free(msg);

	return 1;
}

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

	unsigned int magic = 0; // Check to see if this is a magic path.
	lua_pushstring(L, "magic"); // If the path to add to is magic.
	lua_gettable(L, 3);
	if ( lua_isboolean(L, -1) && lua_toboolean(L, -1))
		magic = 1;
	lua_pop(L, 1);

	lua_pushstring(L, "magicsrc"); // If the path being added is magic.
	lua_gettable(L, 3);
	if ( lua_isboolean(L, -1) && lua_toboolean(L, -1))
		magic |= 0x02;
	lua_pop(L, 1);

	char *targ = NULL;
	char *dep = NULL;
	if (!(magic & 0x01))
	{
		const char *t = UNIXP(lua_tostring(L, 1));
		targ = files->resolver.normalizeFilename(t);
		CYGFREE(t);
	}
	else targ = mstrdup(lua_tostring(L, 1));
	if (!(magic & 0x02))
	{
		const char *t = UNIXP(lua_tostring(L, 2));
		dep = files->resolver.normalizeFilename(t);
		CYGFREE(t);
	}
	else dep = mstrdup(lua_tostring(L, 2));

	Target *t = files->manager->newTarget(targ);
	Target *d = files->manager->newTarget(dep);
	if ( magic & 0x01 ) t->magic = 1;
	if ( magic & 0x02 ) d->magic = 1;
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
	luaL_checktype(L, 1, LUA_TTABLE); // Inputs

	luaL_checktype(L, 2, LUA_TTABLE); // Command line
	lua_pushinteger(L, 1); // Get first item from table
	lua_gettable(L, 2);    //
	if (lua_isstring(L, -1)) // Check the first element of the table.
	{
		lua_createtable(L, 1, 0); // Create the wrapper table.
		lua_insert(L, 2);         // Put it at the proper index
		lua_pushinteger(L, 1);    // Push a 1 for the index of the settable()
		lua_pushvalue(L, 3);      // Copy the old table.
		lua_remove(L, 3);         // Delete the original table.
		lua_settable(L, 2);       // wrapper[1] = oldtable
	}
	lua_pop(L, 1); // Pop the result of the test.

	luaL_checktype(L, 3, LUA_TTABLE); // Outputs

	if (lua_isnone(L, 4))
		lua_newtable(L);

	luaL_checktype(L, 4, LUA_TTABLE); // Options

	lua_settop(L, 4);

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

	unsigned int magic = 0; // Check to see if this is a magic path.
	lua_pushstring(L, "magic"); // If the path to add to is magic.
	lua_gettable(L, 4);
	if ( lua_isboolean(L, -1) && lua_toboolean(L, -1))
		magic = 1;
	lua_pop(L, 1);

	lua_pushstring(L, "magicsrc"); // If the path being added is magic.
	lua_gettable(L, 4);
	if ( lua_isboolean(L, -1) && lua_toboolean(L, -1) )
		magic |= 0x02;
	lua_pop(L, 1);

	if ( desc != NULL ) gen->addDescription(desc);

	lua_settop(L, 4);

	/*** Get Command ***/
	std::vector<const char*> cmd;
	for ( unsigned int i = 1; i <= lua_objlen(L, 2); i++ )
	{
		lua_pushnumber(L, i);
		lua_gettable(L, 2);

		if (!lua_istable(L, -1))
			luaL_error(L, "C.addGenerator was given a generator command line that is not a table.");

		int curcmd = lua_gettop(L);

		cmd.resize(lua_objlen(L, curcmd));
		for ( unsigned int j = lua_objlen(L, curcmd); j >= 1; j-- )
		{
			lua_pushnumber(L, j);
			lua_gettable(L, curcmd);
			if (!lua_isstring(L, -1))
				luaL_error(L, "C.addGenerator was given a generator command argument that is not a string.");

			cmd[j-1] = lua_tostring(L, -1);
			lua_pop(L, 1);
		}
		if (lua_objlen(L, curcmd))
		{
			const char *t = UNIXP(cmd[0]);
			cmd[0] = generatorCmd = files->resolver.normalizeFilename(t);
			CYGFREE(t);
		}

		gen->addCommand(cmd);
		lua_pop(L, 1);
	}

	lua_settop(L, 4);

	std::vector<Target*> in(luaL_getn(L, 1));

	/*** Get Inputs ***/
	for ( unsigned int i = lua_objlen(L, 1); i >= 1; i-- ) // For each input
	{
		/*** Get the current filename ***/
		lua_pushnumber(L, i);
		lua_gettable(L, 1);

		if (!lua_isstring(L, -1))
			luaL_error(L, "C.addGenerator was given a source file that is "
			              "not a string."
			          );

		char *t;
		if (!(magic & 0x02))
		{
			const char *tmp = UNIXP(lua_tostring(L, -1));
			t = files->resolver.normalizeFilename(tmp);
			CYGFREE(tmp);
		}
		else t = mstrdup(lua_tostring(L, -1));

		in[i-1] = files->manager->newTarget(t);
		if (magic & 0x02) in[i-1]->magic = 1;

		free(t);
		lua_pop(L, 1);
	}

	lua_settop(L, 4);

	/*** Get Outputs ***/
	for ( unsigned int i = lua_objlen(L, 3); i >= 1; i-- ) // For each output
	{
		/*** Get the current filename ***/
		lua_pushnumber(L, i);
		lua_gettable(L, 3);

		if (!lua_isstring(L, -1))
			luaL_error(L, "C.addGenerator was given an output file that is not a string.");

		char *tpath;
		if (!(magic & 0x01))
		{
			const char *t = UNIXP(lua_tostring(L, -1));
			tpath = files->resolver.normalizeFilename(t);
			CYGFREE(t);
		}
		else tpath = mstrdup(lua_tostring(L, -1));

		Target *t = files->manager->newTarget(tpath);
		if (magic & 0x01) t->magic = 1;

		free(tpath);
		lua_pop(L, 1);

		t->addGenerator(gen);

		for ( unsigned int i = in.size(); i--; )
			t->addDependancy(in[i]);
	}

	lua_settop(L, 4);

	free(generatorCmd);

	return 0;
}

int path (lua_State *L)
{
	if (!lua_isstring(L, 1))
		luaL_error(L, "path was asked to convert a path that is not a string");

	const char *p1 = UNIXP(lua_tostring(L, 1));
	char *p2 = files->resolver.normalizeFilename(p1);
	files->resolver.prettyPath(p2);
	p2 = WINPF(p2);
	lua_pushstring(L, p2); // Push our result.
	CYGFREE(p1);
	free(p2);

	return 1;
}

} // End of d
} // End of LuaFunctions
