--- Utility Functions.
-- @module S.util

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

S.util = {}
P.S.util = P.S.util or {}
P.S.util.cmd = P.S.util.cmd or {}

local function setup () -- So that we can hide our locals.

if not P.S.util.cmd.cp then
	local cps = T.List{
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

	local cp;
	for c in cps:iter() do          -- Find the first compiler they have
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
	local mvs = T.List{
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

	local mv;
	for c in mvs:iter() do          -- Find the first compiler they have
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
	local rms = T.List{
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

	local rm;
	for c in rms:iter() do          -- Find the first compiler they have
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
	local installs = T.List{
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

	local install;
	for c in installs:iter() do          -- Find the first compiler they have
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

--- Copy a file.
--
-- @tparam string src The file to copy.
-- @tparam string dest Where to copy it to.
function S.util.cp ( src, dest )
	T.utils.assert_string(1, src)
	T.utils.assert_string(2, dest)

	src  = C.path(src)
	dest = C.path(dest)

	cmd = T.List()
	cmd:append(P.S.util.cmd.cp.name)
	cmd:append(src)
	cmd:append(dest)

	C.addGenerator({src}, cmd, {dest}, {
		description = "Coppying "..src.." to "..dest
	})
end

--- Move a file.
--
-- @tparam string src The file to move.
-- @tparam string dest Where to move it to.
function S.util.mv ( src, dest )
	T.utils.assert_string(1, src)
	T.utils.assert_string(2, dest)

	src  = C.path(src)
	dest = C.path(dest)

	cmd = T.List()
	cmd:append(P.S.util.cmd.mv.name)
	cmd:append(src)
	cmd:append(dest)

	C.addGenerator({src}, cmd, {dest}, {
		description = "Moving "..src.." to "..dest
	})
end

--- Delete a file.
--
-- @tparam string file The file to delete.
function S.util.rm ( file )
	T.utils.assert_string(1, file)

	file = C.path(file)

	cmd = T.List()
	cmd:append(P.S.util.cmd.rm.name)
	cmd:append(file)

	C.addGenerator({file}, cmd, {}, {
		description = "Deleting "..file
	})
end

--- Safely install a file.
-- This installs a file without crashing programs.
--
-- @tparam string src The file to install.
-- @tparam string dest Where to install it.
function S.util.install ( src, dest )
	T.utils.assert_string(1, src)
	T.utils.assert_string(2, dest)

	src  = C.path(src)
	dest = C.path(dest)

	cmd = T.List()
	cmd:append(P.S.util.cmd.install.name)
	cmd:extend(P.S.util.cmd.install.flags.preargs)
	cmd:append(src)
	cmd:append(dest)

	C.addGenerator({src}, cmd, {dest}, {
		description = "Installing "..src.." to "..dest
	})
end

end
setup()
setup=nil
