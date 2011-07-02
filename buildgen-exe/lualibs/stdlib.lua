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
L = {} -- User libraries
if not P.S then P.S = {} end

S.prefix = P.S.prefix
if not S.prefix then S.prefix = "/usr/local/" end

function S.setPrefix ( pre )
	P.S.prefix = pre
	S.prefix = pre
end

S.os = _s_os
S.lualibsRoot = _s_lualibs_root
S.imported = false

function S.import ( name )
	if not S.imported
	then
		dofile(S.lualibsRoot.."stdlib/stdlib.lua")
	end

	if not S[name]
	then
		dofile(S.lualibsRoot.."stdlib/"..name..".lua")
	end
end

function L.import ( path )
	print("L.import is not implemented yet")

	local name = path:reverse():find("/", 1, true)
	if i then
		name = name:sub(-i+1)
	end

	if not L[name]
	then
		if path:sub(-4) == ".lua"  then
			dofile(D.path(path))
		else
			dofile(S.lualibsRoot.."custom/"..name..".lua")
		end
	end
end
