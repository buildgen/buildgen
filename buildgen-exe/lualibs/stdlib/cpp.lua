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


S.cpp = {}
if not P.S.cpp then P.S.cpp = {} end

S.import "ld"

do -- So that we can hide our locals.
local state = {}

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
S.cpp.optimization = "regular"
if D.debug then S.cpp.optimization = "none" end

--- Whether to profile.
--
-- If true profiling code will be present in the resulting executable. This
-- value defaults to true if D.debug is set otherwise false.
S.cpp.profile = false
if D.debug then S.cpp.profile = true end

--- Whether to produce debugging symbols.
-- If true debugging symbols will be produced in the resulting executable. This
-- value defaults to true if D.debug is set otherwise false.
S.cpp.debug = false
if D.debug then S.cpp.debug = true end

--- Create new cpp state
-- Creates and returns an opaque state.  This state is a table and is therefore
-- passed by refrence.
--
-- @return The newly created state.
function S.cpp.newState ( )
	local data = {
		arguments = T.List(),
		linker    = S.ld.newState(),
	}

	local s = S.cpp.swapState(data)

	S.cpp.addLib "stdc++"

	S.cpp.loadState(s)

	return data
end

--- Stashes the current state.
-- Returns the current state and loads a new state.  This is equivilent to
-- S.cpp.swapState(S.cpp.newState()).
--
-- @return The old state.
function S.cpp.stashState ( )
	return S.cpp.swapState(S.cpp.newState())
end

--- Swap the state
-- Swaps new with the current state.
--
-- @param new The new state to load.
-- @return The old state.
function S.cpp.swapState ( new )
	local old = state

	old.debug        = S.cpp.debugOveride
	old.optimization = S.cpp.optimizationOveride
	old.profile      = S.cpp.profileOveride

	S.cpp.loadState(new)

	return old
end

--- Load a state
-- Loads the state data
--
-- @param data The state to load.
function S.cpp.loadState ( data )
	state = data

	S.cpp.debug        = data.debug
	S.cpp.optimization = data.optimization
	S.cpp.profile      = data.profile
end

if not P.S.cpp.compiler then
	local compilers = {
		{	name = "g++", -- Name of the executable
			flags = {
				compile  = "-c",
				output   = {"-o", "%s"}, -- the option to set the output file name.
				debug    = "-g",         -- the option to enable debug mode.
				profile  = "-p",         -- the option to enable profiling.
				define   = {"-D%s=%s"},  -- the option to define a macro.
				include  = {"-I", "%s"}, -- the option to add an include directory.
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
	compilers = T.List(compilers) -- turn tabe into a penlight 'list'

	local compiler;
	for c in compilers:iter() do          -- Find the first compiler they have
		if S.findExecutable(c.name) then -- installed on thier system.
			compiler = c
			compiler.name = S.findExecutable(compiler.name)

			break
		end
	end

	if compiler == nil then
		error("Error: No C++ compiler found.", 0)
	else
		P.S.cpp.compiler = compiler
	end
end

-- Overide the default optimization level.
S.cpp.optimizationOveride = S.cpp.optimizationOveride

-- Overide the default profile setting.
S.cpp.profileOveride = S.cpp.profileOveride

-- Overide the default profile setting.
S.cpp.debugOveride = S.cpp.debugOveride

--- Add an argrment.
-- Add an argument to the compiler command line.  Please try to avoid using this
-- as it is not portable across compilers.  Please use the other functions that
-- modify the command line (Such as S.coo.optimization and S.cpp.define()) as they
-- are localized to the compiler being used.
--
-- @param args a string or list of strings to be added to the compiler command
--	line.
function S.cpp.addArg ( arg )
	if type(arg) ~= "table" then
		arg = {tostring(arg)}
	end

	for k, v in pairs(arg) do state.arguments:append(v) end
end

--- Add an include directory
--
-- @param dir an string or list of strings.  These will be treated as BuildGen
--	paths.
function S.cpp.addInclude ( dir )
	if type(dir) ~= "table" then
		dir = T.List{tostring(dir)}
	end

	for v in dir:iter() do
		v = C.path(v)
		for l, w in pairs(P.S.cpp.compiler.flags.include) do
			S.cpp.addArg(w:format(v))
		end
	end
end

--- Define a macro
-- Define a macro during compliation.
--
-- @param map A table of key/value pairs to be defined during compilation.
function S.cpp.define ( map )
	if type(map) ~= "table" then
		dir = {tostring(map)}
	end

	for k, v in pairs(map) do
		if type(v) ~= "string" then
			v = ""
		end
		for l, w in pairs(P.S.cpp.compiler.flags.define) do
			S.cpp.addArg(w:format(k, v))
		end
	end
end

--- Link a library.
--
-- This just calls S.ld.addLib() with the linker being used by S.c.
--
-- @param dir a string or list of strings as the name of the libraries.
function S.cpp.addLib ( lib )
	local ln = S.ld.swapState(state.linker)

	S.ld.addLib(lib)

	state.linker = S.ld.swapState(ln)
end

--- Compile a source into an object.
--
-- @param obj The place to put the resulting object file.
-- @prarm src The file to compile.
-- @ headers A list of headers that are needed.
function S.cpp.compileObject ( obj, src, headers )
	obj = C.path(obj)
	src = C.path(src)
	headers = T.List(headers):map(C.path)
	headers:append(src)

	local compiler = P.S.cpp.compiler

	local oldarguments = state.arguments
	state.arguments = T.List()

	S.cpp.addArg(compiler.name)
	S.cpp.addArg(compiler.flags.compile)

	S.cpp.addArg(oldarguments)

	local debug = S.cpp.debugOveride
	if debug == nil then debug = S.cpp.debug end
	if debug then                      -- Add the debug flag.
		S.cpp.addArg(compiler.flags.debug)
		S.cpp.define{DEBUG=true}
	else
		S.cpp.define{NDEBUG=true}
	end
	local profile = S.cpp.profileOveride
	if profile == nil then profile = S.cpp.profile end
	if profile then                    -- Add the profile flag.
		S.cpp.addArg(compiler.flags.profile)
	end
	local optimization = S.cpp.optimizationOveride
	if optimization == nil then optimization = S.cpp.optimization end
	local o = compiler.flags.optimize[optimization] -- Set the optimization
	if o then                                       -- level.                                --
		S.cpp.addArg(o)                             --
	end                                             --

	for i in T.List(compiler.flags.output):iter() do -- Add the desired output file to
		S.cpp.addArg(i:format(obj))                  -- the command line.
	end                                              --

	S.cpp.addArg(src)

	C.addGenerator(headers, state.arguments, {obj}, {
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
function S.cpp.compile ( out, sources )
	out = C.path(out)
	sources = T.List(sources):map(C.path)

	local ln = S.ld.swapState(state.linker) -- Use our linker

	local projectRoot = C.path("<") -- Cache this.
	local outRoot     = C.path(">") --

	local h, s = T.List(), T.List()
	for source in sources:iter() do
		if source:match("\.[^\.]*[Hh][^\.]*$") then
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

		S.cpp.compileObject(object, source, h)
		objects:append(object)
	end

	S.ld.link(out, objects)

	state.linker = S.ld.swapState(ln) -- Put their linker back.
end

S.cpp.swapState(S.cpp.newState())

end
