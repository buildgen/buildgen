-- Copyright 2011-2012 Kevin Cox

--[[---------------------------------------------------------------------------]
[                                                                              ]
[  Permission is hereby granted, free of charge, to any person obtaining a     ]
[  copy of this software and associated documentation files (the "Software"),  ]
[  to deal in the Software without restriction, including without limitation   ]
[  the rights to use, copy, modify, merge, publish, distribute, sublicense,    ]
[  and/or sell copies of the Software, and to permit persons to whom the       ]
[  Software is furnished to do so, subject to the following conditions:        ]
[                                                                              ]
[  The above copyright notice and this permission notice shall be included in  ]
[  all copies or substantial portions of the Software.                         ]
[                                                                              ]
[  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  ]
[  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    ]
[  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL     ]
[  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER  ]
[  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     ]
[  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         ]
[  DEALINGS IN THE SOFTWARE.                                                   ]
[                                                                              ]
[-----------------------------------------------------------------------------]]


--- The Standard library namespace.

S = {} -- Standard libraries
L = {} -- User libraries
S.imported = {}
L.imported = {}

if not P.S then P.S = {} end --
if not P.L then P.L = {} end --

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
		dofile(S.lualibsRoot.."stdlib.lua")
		S.imported["stdlib"] = true
	end

	if not S.imported[name] then
		S.imported[name] = true

		name:gsub("%.", "/") -- Make into a path.
		dofile(S.lualibsRoot..name..".lua")
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
		print("NAME: "..name.."     GLOBAL: ", global)
		if global then
			name = name:gsub("%.", "/") -- Make into a path.
			dofile(L.lualibsRoot..name..".lua")
		else
			dofile(C.path(name))
		end
	end
end
