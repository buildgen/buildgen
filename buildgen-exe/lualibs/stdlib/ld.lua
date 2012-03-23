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


S.ld = {}
if not P.S.ld then P.S.ld = {} end

do -- So that we can hide our locals.
local state = {}

--- Create new ld state
-- Creates and returns an opaque state.  This state is a table and is therefore
-- passed by refrence.
--
-- @return The newly created state.
function S.ld.newState ( )
	data = {
		arguments = T.List(),
	}

	return data
end

--- Stashes the current state.
-- Returns the current state and loads a new state.  This is equivilent to
-- S.ld.swapState(S.ld.newState()).
--
-- @return The old state.
function S.ld.stashState ( )
	return S.ld.swapState(S.ld.newState())
end

--- Swap the state
-- Swaps new with the current state.
--
-- @param new The new state to load.
-- @return The old state.
function S.ld.swapState ( new )
	local old = state

	S.ld.loadState(new)

	return old
end

--- Load a state
-- Loads the state data
--
-- @param data The state to load.
function S.ld.loadState ( data )
	state = data
end

S.ld.swapState(S.ld.newState())

if not P.S.ld.linker then
	local linkers = T.List{
		{	name="cc", -- Name of the executable
			flags = {
				link = {}, -- commands that need to be present.
				shared = {"-shared"},
				out  = {"-o", "%s"},
				lib  = {"-l", "%s"}
			}
		},
	}
	local linker;
	for l in linkers:iter() do            -- Find the first linker they have
		if S.findExecutable(l.name) then -- installed on thier system.
			linker = l
			linker.name = S.findExecutable(linker.name)

			break
		end
	end

	if linker == nil then
		error("No linker found.", 0)
	else
		P.S.ld.linker = linker
	end
end

--- Add an argrment.
-- Add an argument to the linker command line.  Please try to avoid using this
-- as it is not portable across compilers.  Please use the other functions that
-- modify the command line (such as S.ld.addLib()) as they
-- are localized to the linker being used.
--
-- @param args a string or list of strings to be added to the linker command
--	line.
function S.ld.addArg ( arg )
	if type(arg) ~= "table" then
		arg = {tostring(arg)}
	end

	state.arguments:extend(arg)
end

--- Link in a Library
-- Link the library to the executable.
--
-- @param The name of the library to link.
function S.ld.addLib ( lib )
	if type(lib) ~= "table" then
		lib = {tostring(lib)}
	end

	local args = T.List()
	for k, v in pairs(lib) do
		for l, w in pairs(P.S.ld.linker.flags.lib) do
			args:append(w:format(v))
		end
	end
	S.ld.addArg(args)
end

--- Link an executable.
-- Links object files into an executable.
--
-- @param out The location to put the executable.  This is treated as a BuildGen
--	path.  This is only the base name and it will be modified to fit the platform.
--  For example, on windows ##.exe## will be added.
function S.ld.link ( objects, out )
	linker = P.S.ld.linker

	local cmd = T.List()
	cmd:append(linker.name)

	if type(linker.flags.link) == "table" then
		cmd:extend(linker.flags.link)
	else
		cmd:append(linker.flags.link)
	end

	for i in T.List(linker.flags.out):iter() do -- Add the desired output file
		cmd:append(i:format(out))               -- to the command line.
	end

	cmd:extend(state.arguments)
	cmd:extend(objects)

	C.addGenerator(objects, cmd, {out}, {
		description = "Linking "..out
	})

	return out
end

--- Link a Static library.
-- Links object files into a static library.
--
-- @param out The location to put the library.  This is treated as a BuildGen
--	path.  This is only the base name and it will be modified to fit the operating
--  system.
function S.ld.linkStatic ( objects, out )
	out = C.path(out)
	local dir, base = T.path.splitpath(out)

	out = T.path.join(dir, base..".a") -- If unix.

	local cmd = T.List{"*ar", "-cvq", out}
	cmd:extend(objects)

	C.addGenerator(objects, cmd, {out}, {
		description = "Creating static library "..out
	})

	return out
end

--- Link a Shared library.
-- Links object files into a shared library.
--
-- @param out The location to put the library.  This is treated as a BuildGen
--	path.  This is only the base name and it will be modified to fit the operating
--  system.
function S.ld.linkShared ( objects, out )
	out = C.path(out)
	local dir, base = T.path.splitpath(out)

	out = T.path.join(dir, "lib"..base..".so") -- If unix.

	linker = P.S.ld.linker

	local cmd = T.List()
	cmd:append(linker.name)

	cmd:extend(linker.flags.shared)

	for i in T.List(linker.flags.out):iter() do -- Add the desired output file
		cmd:append(i:format(out))               -- to the command line.
	end

	cmd:extend(state.arguments)
	cmd:extend(objects)

	C.addGenerator(objects, cmd, {out}, {
		description = "Linking "..out
	})

	return out
end

end
