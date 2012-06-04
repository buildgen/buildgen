-- Copyright 2011-2012 Kevin Cox

--[[---------------------------------------------------------------------------]
[                                                                              ]
[  This software is provided 'as-is', without any express or implied           ]
[  warranty. In no event will the authors be held liable for any damages       ]
[  arising from the use of this software.                                      ]
[                                                                              ]
[  Permission is granted to anyone to use this software for any purpose,       ]
[  including commercial applications, and to alter it and redistribute it      ]
[  freely, subject to the following restrictions:                              ]
[                                                                              ]
[  1. The origin of this software must not be misrepresented; you must not     ]
[     claim that you wrote the original software. If you use this software in  ]
[     a product, an acknowledgment in the product documentation would be       ]
[     appreciated but is not required.                                         ]
[                                                                              ]
[  2. Altered source versions must be plainly marked as such, and must not be  ]
[     misrepresented as being the original software.                           ]
[                                                                              ]
[  3. This notice may not be removed or altered from any source distribution.  ]                                                           *
[                                                                              ]
[-----------------------------------------------------------------------------]]


--- The Standard library namespace.

S = {} -- Standard libraries
L = {} -- User libraries
S.imported = {}
L.imported = {}

if not P.S then P.S = {} end --
if not P.L then P.L = {} end --

do

local function runScript ( basename )
	local status, err = pcall(function () dofile(basename..".luo") end)
	if not status then dofile(basename..".lua") end
end

--- The Standard Library Root.
--
-- This is the root directory of the standard library files.  This is mostly for
-- internal use.  This is an absolute path.
S.lualibsRoot = _G.S.lualibsRoot.."stdlib/"

--- The Custom Library Root.
--
-- This is the root directory of the custom library files.  This is mostly for
-- internal use and for installing custom libraries.  This is an absolute path.
L.lualibsRoot = _G.S.lualibsRoot.."custom/"

--- Operating System Information
-- Fields are:
-- <ul><li>
--		style: The style of OS.
-- </li><li>
--		kernel: The kernel the OS uses.
-- </li><li>
--		compliance: The standards that the os complies with.
-- </li></ul>
S.os = _G.S.os

--- Load a Standard Library
--
-- Loads the library <span class="code">name</span>.  The library will become
-- available in <span class="code">S[name]</span>.  There is no harm in trying
-- to load a library more than once.
--
-- @param name The name of the library to load.
function S.import ( name )
	if not S.imported["stdlib"] then
		runScript(S.lualibsRoot.."stdlib")
		S.imported["stdlib"] = true
	end

	if not S.imported[name] then
		S.imported[name] = true

		name:gsub("%.", "/") -- Make into a path.
		runScript(S.lualibsRoot..name)
	end
end

--- Load a Custom Library
--
-- Loads the library <span class="code">name</span>.  There is no harm in trying
-- to load a library more than once.
--
-- If <span class="code">name</span> ends in <span class="code">.lua</span> it
-- will be treated as a BuildGen path, else it will be loaded form the system
-- library directory.
--
-- @param name The name of the library to load.
function L.import ( name )
	local global = false
	local lname

	if name:sub(-4) == ".lua" then
		global = false
		lname = name:sub(name:rfind("/"), -3)
	else
		global = true
		lname = name
	end

	if not L.imported[lname] then
		L.imported[name] = true
		if global then
			name = name:gsub("%.", "/") -- Make into a path.
			runScript(L.lualibsRoot..name)
		else
			dofile(C.path(name)) -- It is a .lua file already.
		end
	end
end
end
