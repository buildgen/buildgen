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

#ifndef LUA_HPP
#define LUA_HPP

#include <lua.hpp>

#include "files.hpp"

class BuildGenLuaEnv
{
	lua_State* L;
	Files *files;

	char *root_file;

	char *corefile;

	void init(void);
	void init_lua(void);
	void dmakeify_lua(void);
	void report_errors(lua_State *L, int status);

	void clenseEnvironment(void);

	void doRunFile ( const char *path );
public:
	BuildGenLuaEnv(Files *files, const char *root);
	~BuildGenLuaEnv();

	void define(char *key, char *value);

	void runFile(const char *path);
};

#endif
