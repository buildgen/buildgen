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


S.c = {}

S.import "ld"

if not P.S.c then P.S.c = {} end

do -- So that we can hide our locals.
local state = {}

--- Create new c state
-- Creates and returns an opaque state.  This state is a table and is therefore
-- passed by refrence.
--
-- @return The newly created state.
function S.c.newState ( )
	local data = {
		arguments = T.List(),
		linker    = S.ld.newState(),
	}

	data.debug = false
	if D.debug then data.debug = true end

	data.optimization = "regular"
	if D.debug then data.optimization = "none" end

	data.profile = false
	if D.debug then data.profile = true end

	return data
end

--- Stashes the current state.
-- Returns the current state and loads a new state.  This is equivilent to
-- S.c.swapState(S.c.newState()).
--
-- @return The old state.
function S.c.stashState ( )
	return S.c.swapState(S.c.newState())
end

--- Swap the state
-- Swaps new with the current state.
--
-- @param new The new state to load.
-- @return The old state.
function S.c.swapState ( new )
	local old = state

	old.debug        = S.c.debug
	old.optimization = S.c.optimization
	old.profile      = S.c.profile

	S.c.loadState(new)

	return old
end

--- Load a state
-- Loads the state data
--
-- @param data The state to load.
function S.c.loadState ( data )
	state = data

	S.c.debug        = data.debug
	S.c.optimization = data.optimization
	S.c.profile      = data.profile
end

S.c.swapState(S.c.newState())

if not P.S.c.compiler then
	local compilers = {
		{	name = "gcc", -- Name of the executable
			flags = {
				compile  = "-c",
				output   = {"-o", "%s"}, -- the option to set the output file name.
				debug    = "-g",         -- the option to enable debug mode.
				profile  = "-p",         -- the option to enable profiling.
				include  = {"-I", "%s"}, -- the option to add an include directory.
				define   = {"-D%s"}, -- the option to add an include directory.
				optimize = {             -- Flags for different levels of optimization.
					none    = {},
					quick   = "-O",
					regular = "-O2",     -- Default optimazation.
					full    = "-O3",
					max     = {
								"-O3",
								"-fexpensive-optimizations",
								"-fomit-frame-pointer"
							  },     -- Highest possoble (possibly exparemental)
				}
			}
		},
	}
	T.List(compilers) -- turn tabe into a penlight 'list'

	local compiler;
	for c in compilers:iter() do          -- Find the first compiler they have
		if S.findExecutable(c.name) then -- installed on thier system.
			compiler = c
			compiler.name = S.findExecutable(compiler.name)

			break
		end
	end

	if compiler == nil then
		error("Error: No C compiler found.", 0)
	else
		P.S.c.compiler = compiler
	end
end

--- The optimization level.
--
-- A string value representing the level of optimization to use when building
-- the project. Possible values are:
-- <ul><li>
--		none - Perform no optimization.
--</li><li>
--		quick - Perform light optimization.
--</li><li>
--		regular - Perform regular optimization.
--</li><li>
--		full - Fully optimize the executable.
--</li><li>
--		max - Optimize as much as possible (possibly experimental optimizations).
--</li></ul>
S.c.optimization = S.c.optimization

--- Whether to profile.
--
-- If true profiling code will be present in the resulting executable. This
-- value defaults to true if D.debug is set otherwise false.
S.c.profile = S.c.profile

--- Whether to produce debugging symbols.
-- If true debugging symbols will be produced in the resulting executable. This
-- value defaults to true if D.debug is set otherwise false.
S.c.debug = S.c.debug

--- Add an argrment.
-- Add an argument to the compiler command line.  Please try to avoid using this
-- as it is not portable across compilers.  Please use the other functions that
-- modify the command line (Such as S.c.optimization and S.c.define()) as they
-- are localized to the compiler being used.
--
-- @param args a string or list of strings to be added to the compiler command
--	line.
function S.c.addArg ( args )
	if type(args) ~= "table" then
		args = T.List{tostring(args)}
	else
		args = T.List(args)
	end

	for a in args:iter() do state.arguments:append(a) end
end

--- Add an include directory
--
-- @param dir an string or list of strings.  These will be treated as BuildGen
--	paths.
function S.c.addInclude ( dir )
	if type(dir) ~= "table" then
		dir = {tostring(dir)}
	end

	for k, v in pairs(dir) do
		v = C.path(v)
		for l, w in pairs(P.S.c.compiler.flags.include) do
			S.c.addArg(w:format(v))
		end
	end
end

--- Define a macro
-- Define a macro during compliation.
--
-- @param map A table of key/value pairs to be defined during compilation.
function S.c.define ( map )
	if type(map) ~= "table" then
		dir = {tostring(map)}
	end

	for k, v in pairs(map) do
		if type(value) ~= "string" then
			value = ""
		else
			value = "="..value
		end
		for l, w in pairs(P.S.c.compiler.flags.define) do
			S.c.addArg(w:format(v))
		end
	end
end

--- Link a library.
--
-- This just calls S.ld.addLib() with the linker being used by S.c.
--
-- @param dir a string or list of strings as the name of the libraries.
function S.c.addLib ( lib )
	local ln = S.ld.swapState(state.linker)

	S.ld.addLib(lib)

	state.linker = S.ld.swapState(ln)
end

--- Compile a source into an object.
--
-- @param obj The place to put the resulting object file.
-- @prarm src The file to compile.
-- @ headers A list of headers that are needed.
function S.c.compileObject ( obj, src, headers )
	obj = C.path(obj)
	src = C.path(src)
	headers = T.List(headers):map(C.path)

	local compiler = P.S.c.compiler

	local oldarguments = state.arguments
	state.arguments = T.List()

	S.c.addArg(compiler.name)
	S.c.addArg(compiler.flags.compile)

	S.c.addArg(oldarguments)

	if S.c.debug then                      -- Add the debug flag.
		S.c.addArg(compiler.flags.debug)
	end
	if S.c.profile then                    -- Add the profile flag.
		S.c.addArg(compiler.flags.profile)
	end
	local o = compiler.flags.optimize[S.c.optimization] -- Set the optimization
	if o then                                           -- level.                                --
		S.c.addArg(o)                                   --
	end                                                 --

	for i in T.List(compiler.flags.output):iter() do -- Add the desired output
		S.c.addArg(i:format(obj))                    -- file to the command
	end                                              -- line.

	S.c.addArg(src)

	C.addGenerator({obj}, headers, state.arguments, {
		description = "Compiling "..obj
	})

	state.arguments = oldarguments;
end

--- Compile an Executable
-- Compiles and links a list of files into executables.
--
-- @param out The file to be created.  ".exe" will be appended if compiling on
--	Windows.
-- @param sources A list of sources (bot header and source files) that will be
--	used when compiling the executable.
function S.c.compile ( out, sources )
	out = C.path(out)
	sources = T.List(sources):map(C.path)

	local ln = S.ld.swapState(state.linker) -- Use our linker

	local projectRoot = C.path("<") -- Cache this.
	local outRoot     = C.path(">") --

	local h, s = T.List(), T.List()
	for source in sources:iter() do
		if source:match("[Hh]") then
			h:append(source)
		else
			s:append(source)
		end
	end

	local objects = T.List()

	for source in s:iter() do
		source = C.path(source)

		local object = nil; -- Get path to put object file.

		if source:sub(0, #projectRoot) == projectRoot then
			object = C.path(">"..source:sub(#projectRoot)..".o")
		elseif source:sub(0, #outRoot) == outRoot then
			object = C.path(source..".o") -- Already in the out dir.
		else
			object = C.path("@"..source:sub(#projectRoot)..".o") -- Put inside
			                                                     -- the build
		end                                                      -- dir.

		S.c.compileObject(object, source, h)
		objects:append(object)
	end

	S.ld.link(out, objects)

	state.arguments = oldarguments;
	state.linker = S.ld.swapState(ln) -- Put their linker back.
end

--- Create a header file with definitions.
-- Creates a header/source pair with definitions. Currently all values are
-- treated as strings with type  <span class="code">const char*</span>.
--
-- @param header Where to put the genereated header.  This is treated as a
--	BuildGen path.
-- @param header Where to put the genereated source file.  This is treated as a
--	BuildGen path.
-- @param An object consisting of key/value pairs where the key will be the
--	variable name and the value will be the value.
function S.c.generateHeader ( head, src, definitions )
	local generatorScript = S.lualibsRoot .. "c/generateHeader.lua"

	head = C.path(head)
	src  = C.path(src)

	cmd = T.List()
	cmd:append "*lua"
	cmd:append(generatorScript)
	cmd:append(head)
	cmd:append(src)

	for k,v in pairs(definitions) do
		cmd:append(k.."="..v)
	end

	C.addGenerator({head, src}, {}, cmd, {
		description = "Generating "..head
	})
end

end
