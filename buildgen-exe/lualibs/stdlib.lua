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

function L.import ( path )
	local name = path:reverse():find("/", 1, true)
	if i then
		name = name:sub(-i+1)
	end

	if not L[name]
	then
		if path:find(".") then
			dofile(C.path(path))
		else
			dofile(L.lualibsRoot..name..".lua")
		end
	end
end
