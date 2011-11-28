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
		{	name = "cp", -- Name of the executable
			flags = {
				noover  = "-n",
				recurse = "-r",
				verbose = "-v",
			}
		},
		{	name = S.lualibsRoot.."util/cp.lua", -- Name of the executable
			flags = {
				noover  = "-n",
				recurse = "-r",
				verbose = "-v",
			}
		},
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

if not P.S.util.cmd.mv then
	local mvs = {
		{	name = "mv", -- Name of the executable
			flags = {
				over    = {},
				noover  = {},
				recurse = {},
				verbose = "-v",
			}
		},
		{	name = S.lualibsRoot.."util/mv.lua", -- Name of the executable
			flags = {
				over    = {},
				noover  = {},
				recurse = {},
				verbose = "-v",
			}
		},
	}
	List(mvs) -- turn tabe into a penlight 'list'

	local mv;
	for c in iter(mvs) do          -- Find the first compiler they have
		if S.findExecutable(c.name) then -- installed on thier system.
			mv = c
			mv.name = S.findExecutable(mv.name)

			break
		end
	end

	if mv == nil then
		error("Error: No move command found.", 0)
	else
		P.S.util.cmd.mv = mv
	end
end

if not P.S.util.cmd.rm then
	local rms = {
		{	name = "rm", -- Name of the executable
			flags = {
				recurse = {"-r"},
				verbose = "-v",
			}
		},
		{	name = S.lualibsRoot.."util/rm.lua", -- Name of the executable
			flags = {
				recurse = {"-r"},
				verbose = "-v",
			}
		},
	}
	List(rms) -- turn tabe into a penlight 'list'

	local rm;
	for c in iter(rms) do          -- Find the first compiler they have
		if S.findExecutable(c.name) then -- installed on thier system.
			rm = c
			rm.name = S.findExecutable(rm.name)

			break
		end
	end

	if rm == nil then
		error("Error: No remove command found.", 0)
	else
		P.S.util.cmd.rm = rm
	end
end

if not P.S.util.cmd.install then
	local installs = {
		{	name = "install", -- Name of the executable
			flags = {
				preargs = {"-D"},
				verbose = "-v",
			}
		},
		{	name = S.lualibsRoot.."util/cp.lua", -- Name of the executable
			flags = {
				preargs = {},
				verbose = "-v",
			}
		},
	}
	List(installs) -- turn tabe into a penlight 'list'

	local install;
	for c in iter(installs) do          -- Find the first compiler they have
		if S.findExecutable(c.name) then -- installed on thier system.
			install = c
			install.name = S.findExecutable(install.name)

			break
		end
	end

	if install == nil then
		error("Error: No install command found.", 0)
	else
		P.S.util.cmd.install = install
	end
end

function S.util.cp ( src, dest )
	src  = C.path(src)
	dest = C.path(dest)

	cmd = List()
	cmd:append(P.S.util.cmd.cp.name)
	cmd:append(src)
	cmd:append(dest)

	C.addGenerator({dest}, {src}, cmd, {
		description = "Coppying "..src.." to "..dest
	})
end

function S.util.mv ( src, dest )
	src  = C.path(src)
	dest = C.path(dest)

	cmd = List()
	cmd:append(P.S.util.cmd.mv.name)
	cmd:append(src)
	cmd:append(dest)

	C.addGenerator({dest}, {src}, cmd, {
		description = "Moving "..src.." to "..dest
	})
end

function S.util.rm ( file )
	file = C.path(file)

	cmd = List()
	cmd:append(P.S.util.cmd.rm.name)
	cmd:append(file)

	C.addGenerator({}, {file}, cmd, {
		description = "Deleting "..file
	})
end

function S.util.install ( src, dest )
	src  = C.path(src)
	dest = C.path(dest)

	cmd = List()
	cmd:append(P.S.util.cmd.install.name)
	cmd:append(P.S.util.cmd.cp.flags.preargs)
	cmd:append(src)
	cmd:append(dest)

	C.addGenerator({dest}, {src}, cmd, {
		description = "Installing "..src.." to "..dest
	})
end

end
setup()
setup=nil
