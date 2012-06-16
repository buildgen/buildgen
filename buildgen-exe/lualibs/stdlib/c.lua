-- Copyright 2011-2012 Kevin Cox

--[[---------------------------------------------------------------------------]
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

S.c = {}

S.import "ld"

if not P.S.c then P.S.c = {} end

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
S.c.optimization = "regular"
if D.debug then S.c.optimization = "none" end

--- Whether to profile.
--
-- If true profiling code will be present in the resulting executable. This
-- value defaults to true if D.debug is set otherwise false.
S.c.profile = false
if D.debug then S.c.profile = true end
if S.os.toolset == "cygwin" then S.c.profile = false end -- Doesn't work.  Linker errors.

--- Whether to produce debugging symbols.
-- If true debugging symbols will be produced in the resulting executable. This
-- value defaults to true if D.debug is set otherwise false.
S.c.debug = false
if D.debug then S.c.debug = true end

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

	old.debug        = S.c.debugOveride
	old.optimization = S.c.optimizationOveride
	old.profile      = S.c.profileOveride

	S.c.loadState(new)

	return old
end

--- Load a state
-- Loads the state data
--
-- @param data The state to load.
function S.c.loadState ( data )
	state = data

	S.c.debugOveride        = data.debug
	S.c.optimizationOveride = data.optimization
	S.c.profileOveride      = data.profile
end

S.c.swapState(S.c.newState())

if not P.S.c.compiler then
	local compilers = T.List{
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

-- Overide the default optimization level.
S.c.optimizationOveride = S.c.optimizationOveride

-- Overide the default profile setting.
S.c.profileOveride = S.c.profileOveride

-- Overide the default profile setting.
S.c.debugOveride = S.c.debugOveride

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
		map = {tostring(map)}
	end

	for k, v in pairs(map) do
		if type(v) ~= "string" then
			v = ""
		else
			v = "="..v
		end
		for l, w in pairs(P.L.avr.cpp.compiler.flags.define) do
			L.avr.cpp.addArg(w:format(k..v))
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
-- @prarm src The file to compile.
-- @ headers A list of headers that are needed.
-- @param obj The place to put the resulting object file.
function S.c.compileObject (src, headers, obj)
	T.utils.assert_string(1, src)
	T.utils.assert_arg(2, headers, "table")
	T.utils.assert_string(3, obj)

	obj = C.path(obj)
	src = C.path(src)
	headers = T.List(headers):map(C.path)
	headers:append(src) -- We depend on the source file too.

	local compiler = P.S.c.compiler

	local oldarguments = state.arguments
	state.arguments = T.List()

	S.c.addArg(compiler.name)
	S.c.addArg(compiler.flags.compile)

	S.c.addArg(oldarguments)

	local debug = S.c.debugOveride
	if debug == nil then debug = S.c.debug end
	if debug then                      -- Add the debug flag.
		S.c.addArg(compiler.flags.debug)
	end
	local profile = S.c.profileOveride
	if profile == nil then profile = S.c.profile end
	if profile then                    -- Add the profile flag.
		S.c.addArg(compiler.flags.profile)
	end
	local optimization = S.c.optimizationOveride
	if optimization == nil then optimization = S.c.optimization end
	local o = compiler.flags.optimize[optimization] -- Set the optimization
	if o then                                       -- level.                                --
		S.c.addArg(o)                               --
	end                                             --

	for i in T.List(compiler.flags.output):iter() do -- Add the desired output
		S.c.addArg(i:format(obj))                    -- file to the command
	end                                              -- line.

	S.c.addArg(src)

	C.addGenerator(headers, state.arguments, {obj}, {
		description = "Compiling "..obj
	})

	state.arguments = oldarguments;
end

local function compile ( linkfunc, sources, out )
	out = C.path(out)
	sources = T.List(sources):map(C.path)

	local ln = S.ld.swapState(state.linker) -- Use our linker

	local projectRoot = C.path("<") -- Cache this.
	local outRoot     = C.path(">") --

	local h, s, l = T.List(), T.List(), T.List()
	local objects = T.List()

	for source in sources:iter() do
		if source:match"\.[Hh]$" then
			h:append(source)
		elseif source:match"\.a$" then
			l:append(source)
		elseif source:match"\.o$" then
			objects:append(source)
		else
			s:append(source)
		end
	end

	for source in s:iter() do
		source = C.path(source)

		local object = C.path("@"..source..".o")

		S.c.compileObject(source, h, object)
		objects:append(object)
	end
	objects:extend(l)

	out = linkfunc(objects, out)

	state.linker = S.ld.swapState(ln) -- Put their linker back.

	return out
end

--- Compile an Executable
-- Compiles and links a list of files into executables.
--
-- @param sources A list of sources (bot header and source files) that will be
--   used when compiling the executable.
-- @param out The file to be created.  ".exe" will be appended if compiling on
--   Windows.
-- @returns The actual path of the created executable.
function S.c.compile ( sources, out )
	T.utils.assert_arg(1, sources, "table")
	T.utils.assert_string(2, out)

	return compile(S.ld.link, sources, out)
end

--- Compile a Static Library
-- Compiles and links a list of files into a static library.
--
-- @param sources A list of sources (bot header and source files) that
--   will be used when compiling the library.
-- @param out The file to be created.  This is just the basename,
-- @returns The actual path used for the output executable.
function S.c.compileStatic ( sources, out )
	T.utils.assert_arg(1, sources, "table")
	T.utils.assert_string(2, out)

	return compile(S.ld.linkStatic, sources, out)
end

--- Compile a Shared Library
-- Compiles and links a list of files into a shared library.
--
-- @param sources A list of sources (bot header and source files) that
--   will be used when compiling the library.
-- @param out The file to be created.  This is just the basename,
-- @returns The actual path used for the output executable.
function S.c.compileShared ( sources, out )
	T.utils.assert_arg(1, sources, "table")
	T.utils.assert_string(2, out)

	return compile(S.ld.linkShared, sources, out)
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
	T.utils.assert_string(1, head)
	T.utils.assert_string(2, src)
	T.utils.assert_arg(2, definitions, "table")

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

	C.addGenerator({}, cmd, {head, src}, {
		description = "Generating "..head
	})
end

end
