--- The S Namespace
-- The standard library base BuildGen functions.
-- @module S

-- Copyright 2011-2012 Kevin Cox

--[[ --------------------------------------------------------------------------]
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

S = {} -- Standard libraries
L = {} -- User libraries
S.imported = {}
L.imported = {}

if not P.S then P.S = {} end -- Don't smash the persistant namespace if it
if not P.L then P.L = {} end -- already exists.
if not P.T then P.T = {} end --

--- Operating System Information
-- Fields are:
--
-- - style: The style of OS.
-- - kernel: The kernel the OS uses.
-- - compliance: The standards that the os complies with.
S.os = _G.S.os

if S.os.style == "win32" then
	package.config = "\\"..package.config:sub(2)
end

--- Import Penlight
T = {}

if not P.T.lfs then -- Lua won't re-import it even if we loose our handle.
	require "lfs"
	P.T.lfs = lfs
	lfs = nil
end
T.lfs = P.T.lfs

T.List = require "pl.List"
T.class = require "pl.class"
T.Map = require "pl.Map"
T.Set = require "pl.Set"

T.OrderedMap = require "pl.OrderedMap"
T.MultiMap = require "pl.MultiMap"
--T.TypedList = require "pl.TypedList"

T.app = require "pl.app"
T.array2d = require "pl.array2d"
T.comprehension = require "pl.comprehension"
T.config = require "pl.config"
T.data = require "pl.data"
T.dir = require "pl.dir"
T.file = require "pl.file"
T.func = require "pl.func"
T.input = require "pl.input"
T.lapp = require "pl.lapp"
T.lexer = require "pl.lexer"
T.luabalanced = require "pl.luabalanced"
T.operator = require "pl.operator"
T.path = require "pl.path"
T.permute = require "pl.permute"
T.pretty = require "pl.pretty"
T.seq = require "pl.seq"
T.sip = require "pl.sip"
T.stringio = require "pl.stringio"
T.stringx = require "pl.stringx"
T.tablex = require "pl.tablex"
T.test = require "pl.test"
T.text = require "pl.text"
T.utils = require "pl.utils"

T.stringx.import()

do

local function runInDir ( script )
	local dir = T.path.dirname(script)

	local oldpwd = T.lfs.currentdir()

	T.lfs.chdir(dir)
	dofile(script)
	T.lfs.chdir(oldpwd)
end

local function runProperScript ( basename )
	if T.path.exists(basename..".luo") then
		runInDir(basename..".luo")
	else
		runInDir(basename..".lua")
	end
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

--- Load a Standard Library
--
-- Loads the library `name`.  The library will become available in `S[name]`.
-- There is no harm in trying to load a library more than once.
--
-- @tparam string name The name of the library to load.
function S.import ( name )
	if not S.imported["stdlib"] then
		runProperScript(S.lualibsRoot.."stdlib")
		S.imported["stdlib"] = true
	end

	if not S.imported[name] then
		S.imported[name] = true

		name:gsub("%.", "/") -- Make into a path.
		runProperScript(S.lualibsRoot..name)
	end
end

--- Load a Custom Library
--
-- Loads the library `name`.  There is no harm in trying
-- to load a library more than once.
--
-- If `name` ends in `.lua` it will be treated as a BuildGen path, else it will
-- be loaded form the system library directory.
--
-- @tparam string name The name of the library to load.
function L.import ( name )
	local global = false
	local lname

	if name:sub(-4) == ".lua" then
		global = false
		lname = name:sub(name:rfind("/") or 1, -3)
	else
		global = true
		lname = name
	end

	if not L.imported[lname] then
		L.imported[name] = true
		if global then
			name = name:gsub("%.", "/") -- Make into a path.
			runProperScript(L.lualibsRoot..name)
		else
			runInDir(C.path(name)) -- It is a .lua file already.
		end
	end
end
end
