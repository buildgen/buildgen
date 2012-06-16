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
if S.os.toolset == "cygwin" then S.cpp.profile = false end -- Doesn't work.  Linker errors.

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

	S.cpp.debugOveride        = data.debugOveride
	S.cpp.optimizationOveride = data.optimizationOveride
	S.cpp.profileOveride      = data.profileOveride
end

local compilers = T.Map{
	["g++"] = {
		exe = "g++", -- Name of the executable
		flags = {
			compile  = {"-c"},
			output   = {"-o", "%s"}, -- the option to set the output file name.
			debug    = "-g",         -- the option to enable debug mode.
			profile  = "-p",         -- the option to enable profiling.
			define   = {"-D%s"},     -- the option to define a macro.
			include  = {"-I", "%s"}, -- the option to add an include directory.
			optimize = {             -- Flags for different levels of optimization.
				none    = {},
				quick   = "-O",
				regular = "-O2",     -- Default optimazation.
				full    = "-O3",
				max     = { -- Highest possoble (possibly exparemental)
					"-O3",
					"-fexpensive-optimizations",
					"-fomit-frame-pointer"
				},
			}
		}
	},
	["clang"] = {
		exe = "clang", -- Name of the executable
		flags = {
			compile  = {"-c"},
			output   = {"-o", "%s"}, -- the option to set the output file name.
			debug    = {"-g"},         -- the option to enable debug mode.
			profile  = {"-p"},         -- the option to enable profiling.
			define   = {"-D%s"},     -- the option to define a macro.
			include  = {"-I", "%s"}, -- the option to add an include directory.
			optimize = {             -- Flags for different levels of optimization.
				none    = {},
				quick   = "-O",
				regular = "-O2",     -- Default optimazation.
				full    = "-O3",
				max     = { -- Highest possoble (possibly exparemental)
					"-O3",
					"-fexpensive-optimizations",
					"-fomit-frame-pointer"
				},
			}
		}
	},
}

--- Check to see if a compiler is available.
--
-- Checks to see that a compiler is available and that BuildGen knows how
-- to use it.
--
-- @param name The name of the compiler (often the name of the executable).
-- @returns ``true`` if the compiler can be used otherwise ``false``.
function S.cpp.hasCompiler ( name )
	T.utils.assert_string(1, name)

	if compilers[name] == nil then return false end

	local c = compilers[name]
	if not S.findExecutable(c.exe) then return false end

	return true
end

--- Select which compiler to use.
--
-- This function selects the C++ compiler to use.  You should first check if
-- the compiler is avaiable with ``S.cpp.hasCompiler()``.
--
-- @param name The name of the compiler (often the name of the executable).
function S.cpp.useCompiler ( name )
	T.utils.assert_arg(1, name, "string",
	                  S.cpp.hasCompiler, "Unknown compiler",
	                  2)

	local c = compilers[name]

	local function makeList ( path )
		local c = c; -- The parent table.
		local i;     -- The index in c.
		for e in path:split("."):iter() do -- Recursively get table.
			if i then c = c[i] end
			i = e
		end

		if type(c[i]) == "table" then
			c[i] = T.List(c[i])
		else
			c[i] = T.List{c[i]}
		end

		return c[i]
	end

	c.exe = S.findExecutable(c.exe)
	makeList "flags.compile"
	makeList "flags.output"
	makeList "flags.debug"
	makeList "flags.profile"
	makeList "flags.define"
	makeList "flags.include"
	makeList "flags.optimize.none"
	makeList "flags.optimize.quick"
	makeList "flags.optimize.regular"
	makeList "flags.optimize.full"
	makeList "flags.optimize.max"

	P.S.cpp.compiler = c
end

if not P.S.cpp.compiler then
	for n in compilers:iter() do     -- Find the a compiler that they have
		if S.cpp.hasCompiler(n) then -- installed on thier system.
			S.cpp.useCompiler(n)
			break
		end
	end

	if not P.S.cpp.compiler then
		error("Error: No C++ compiler found.", 0)
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
		map = {tostring(map)}
	end

	for k, v in pairs(map) do
		if type(v) ~= "string" then
			v = ""
		else
			v = "="..v
		end
		for l, w in pairs(P.S.cpp.compiler.flags.define) do
			S.cpp.addArg(w:format(k..v))
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
-- @param src The file to compile.
-- @param headers A list of headers that are needed.
-- @param obj The place to put the resulting object file.
function S.cpp.compileObject ( src, headers, obj )
	T.utils.assert_string(1, src)
	T.utils.assert_arg(2, headers, "table")
	T.utils.assert_string(3, obj)

	obj = C.path(obj)
	src = C.path(src)
	headers = T.List(headers):map(C.path)
	headers:append(src)

	local compiler = P.S.cpp.compiler

	local oldarguments = state.arguments
	state.arguments = T.List()

	S.cpp.addArg(compiler.exe)
	S.cpp.addArg(compiler.flags.compile)

	S.cpp.addArg(oldarguments)

	local debug = S.cpp.debugOveride
	if debug == nil then debug = S.cpp.debug end
	if debug then -- Add the debug flag.
		S.cpp.addArg(compiler.flags.debug)
		S.cpp.define{DEBUG=true}
	else                   -- Add the debug flag.
		S.cpp.define{NDEBUG=true}
	end
	local profile = S.cpp.profileOveride
	if profile == nil then profile = S.cpp.profile end
	if profile then -- Add the profile flag.
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
-- @param sources A list of sources (both header and source files) that will be
--	used when compiling the executable.
-- @param out The file to be created.  ".exe" will be appended if compiling on
--	Windows.
-- @returns The actual output name used.
function S.cpp.compile ( sources, out )
	T.utils.assert_arg(1, sources, "table")
	T.utils.assert_string(2, out)

	sources = T.List(sources):map(C.path)
	out = C.path(out)

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

		local object = C.path("@"..source..".o")

		S.cpp.compileObject(source, h, object)
		objects:append(object)
	end

	S.ld.link(objects, out)

	state.linker = S.ld.swapState(ln) -- Put their linker back.

	return out
end

--- Compile a Shared Library
-- Compiles and links a list of files into a shared library.
--
-- @param sources A list of sources (both header and source files) that will be
--	used when compiling the executable.
-- @param out The file to be created.  This is only the basename.  For example,
--   a path ending with "foo" will be called "libfoo.so" on Posix platforms.
-- @returns The actual output name used.
function S.cpp.compileShared ( sources, out )
	T.utils.assert_arg(1, sources, "table")
	T.utils.assert_string(2, out)

	sources = T.List(sources):map(C.path)
	out = C.path(out)

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

		local object = C.path("@"..source..".o")

		S.cpp.compileObject(source, h, object)
		objects:append(object)
	end

	S.ld.linkShared(objects, out)

	state.linker = S.ld.swapState(ln) -- Put their linker back.

	return out
end

S.cpp.swapState(S.cpp.newState())

end
