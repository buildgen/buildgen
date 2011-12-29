-- Copyright 2011 Kevin Cox

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

S = {} -- Std libraries
S.imported = false
L = {} -- User libraries
if not P.S then P.S = {} end -- Persistant storage
if not P.L then P.L = {} end --

S.lualibsRoot = _s_lualibs_root.."stdlib/"
L.lualibsRoot = _s_lualibs_root.."custom/"

function S.import ( name )
	if not S.imported
	then
		dofile(S.lualibsRoot.."stdlib.lua")
	end

	if not S[name] and name ~= "stdlib"
	then
		dofile(S.lualibsRoot..name..".lua")
	end
end

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

	if not L[lname]	then
		if global then
			dofile(L.lualibsRoot..name..".lua")
		else
			dofile(C.path(name))
		end
	end
end
