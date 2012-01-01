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

#ifndef LUA_HPP
#define LUA_HPP

#include <lua.hpp>

class BuildGenLuaEnv
{
	lua_State* L;
	char *root_file;

	char *corefile;

	void init(void);
	void init_lua(void);
	void dmakeify_lua(void);
	void report_errors(lua_State *L, int status);

	void clenseEnvironment(void);

	void doRunFile ( const char *path );
public:
	BuildGenLuaEnv(const char *root);
	~BuildGenLuaEnv();

	void define(char *key, char *value);

	void runFile(const char *path);
};

#endif
