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


S.util = {}
P.S.util = P.S.util or {}
P.S.util.cmd = P.S.util.cmd or {}

local function setup () -- So that we can hide our locals.

if not P.S.util.cmd.cp then
	local cps = {
		{	name = S.lualibsRoot.."util/cp.lua", -- Name of the executable
			flags = {
				noover  = "-n",
				recurse = "-r",
				verbose = "-v",
			}
		},
		--[[{	name = "cp", -- Name of the executable
			flags = {
				noover  = "-n",
				recurse = "-r",
				verbose = "-v",
			}
		},]]
	}
	List(cps) -- turn tabe into a penlight 'list'

	local cp;
	for c in iter(cps) do          -- Find the first compiler they have
		if S.findExecutable(c.name) then -- installed on thier system.
			cp = c
			cp.name = S.findExecutable(cp.name)

			break
		end
	end

	if cp == nil then
		error("Error: No copy command found.", 0)
	else
		P.S.util.cmd.cp = cp
	end
end

function S.util.cp ( src, dest )
	cmd = List()
	cmd:append(P.S.util.cmd.cp.name)
	cmd:append(C.path(src))
	cmd:append(C.path(dest))

	C.addGenerator({dest}, {src}, cmd)
end

end
setup()
setup=nil
