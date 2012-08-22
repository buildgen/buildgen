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

#ifndef LUA_FUNCTIONS_HPP
#define LUA_FUNCTIONS_HPP

#include <lua.hpp>
#include "files.hpp"
#include "pathresolver.hpp"

namespace LuaFunctions
{
	extern Files *files;

	namespace C
	{
		int add_depandancy(lua_State *L);
		int add_dir(lua_State *L);
		int add_generator(lua_State *L);
		int path(lua_State *L);
	}
	namespace S
	{
		void call_shutdown(lua_State *L);
	}
};

#endif
