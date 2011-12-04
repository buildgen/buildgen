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

--- The Core namespace.
--[[[
	This is where the core build related functions go.
]]
C = {}

C.addGenerator = _c_add_generator
C.addDependancy = _c_add_depandancy
C.addDir = _c_add_dir
C.path = _c_path

function D.resolvePath ( path , default )
	if not D[path] then
		D[path] = default
	end

	if not D[path] then return end -- If the default was nil.

	if D[path]:find("/", 1, true) > 1 then -- Doesn't start with a slash.
		D[path] = C.path(">"..D[path])
	end
end

-- End of D

if not P then P = {} end
dofile(_s_lualibs_root.."stdlib.lua")

if _s_debug then
	dofile(_s_lualibs_root.."debug.lua")
end
