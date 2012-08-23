--- The C Library
-- @module S.c

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

S.c = {}

S.import "ld"

if not P.S.c then P.S.c = {} end

do -- So that we can hide our locals.
local state = {}

--- The optimization level.
--
-- A string value representing the level of optimization to use when building
-- the project. Possible values are:
--
-- - none - Perform no optimization.
-- - quick - Perform light optimization.
-- - regular - Perform regular optimization.
-- - full - Fully optimize the executable.
-- - max - Optimize as much as possible (possibly experimental optimizations).
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
		includes = T.OrderedMap(),
		defines  = T.Map(),

		arguments = T.List(),

		debug        = nil,
		optimization = nil,
		profile      = nil,

		linker    = S.ld.newState(),
	}

	return data
end

--- Stashes the current state.
-- Returns the current state and loads a new state.  This is equivilent to
-- `S.c.swapState(S.c.newState())`.
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
	S.c.loadState(new)
	return old
end

--- Load a state
-- Loads the state data
--
-- @param data The state to load.
function S.c.loadState ( data )
	state = data
end

S.c.swapState(S.c.newState())

local compilers = T.OrderedMap()
compilers:set("gcc",	{
	exe = "gcc", -- Name of the executable
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
})

--- Check to see if a compiler is available.
--
-- Checks to see that a compiler is available and that BuildGen knows how
-- to use it.
--
-- @tparam string name The name of the compiler (often the name of the executable).
-- @treturn boolean `true` if the compiler can be used otherwise `false`.
function S.c.hasCompiler ( name )
	if name == nil then return false end
	T.utils.assert_string(1, name)

	if compilers[name] == nil then return false end

	local c = compilers[name]
	if not S.findExecutable(c.exe) then return false end

	return true
end

--- Select which compiler to use.
--
-- This function selects the C compiler to use.  You should first check if
-- the compiler is avaiable with `S.c.hasCompiler()`.
--
-- @tparam string name The name of the compiler (often the name of the executable).
function S.c.useCompiler ( name )
	T.utils.assert_arg(1, name, "string",
	                  S.c.hasCompiler, "Unknown compiler",
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

	P.S.c.compiler = c
end

if not P.S.c.compiler then
	if D["c.compiler"] then
		if S.c.hasCompiler(D["c.compiler"]) then
			S.c.useCompiler(D["c.compiler"])
		else
			error("Error: requested compiler "..D["c.compiler"].." not found", 0)
		end
	end

	if not P.S.c.compiler then
		if S.c.hasCompiler(D["c.preferedCompiler"]) then
			S.c.useCompiler(D["c.preferedCompiler"])
		end
	end

	if not P.S.c.compiler then
		for n in T.OrderedMap.iter(compilers) do -- Find the a compiler that
			if S.c.hasCompiler(n) then           -- they have installed on their
				S.c.useCompiler(n)               -- system.
				break
			end
		end
	end

	if not P.S.c.compiler then
		error("Error: No C compiler found.", 0)
	end
end

--- Overide the default optimization level.
function S.c.optimizationOveride ( level )
	if level ~= nil then
		T.utils.assert_arg(1, level, "string",
		                   validOptimization, "Unknown optimization level.",
		                   2)
	end

	state.optimization = level
end

--- Overide the default profile setting.
function S.c.profileOveride ( profile )
	if level ~= nil then
		T.utils.assert_arg(1, profile, "boolean")
	end

	state.profile = profile
end

--- Overide the default profile setting.
function S.c.debugOveride ( debug )
	if level ~= nil then
		T.utils.assert_arg(1, debug, "boolean")
	end

	state.debug = debug
end

--- Add an argrment.
-- Add an argument to the compiler command line.  Please try to avoid using this
-- as it is not portable across compilers.  Please use the other functions that
-- modify the command line (Such as S.c.optimization and S.c.define()) as they
-- are localized to the compiler being used.
--
-- @tparam {string,...} args values to be added to the compiler command line.
function S.c.addArg ( args )
	if type(args) ~= "table" then
		args = tostring(args)
	else
		args = T.List(args)
	end

	state.arguments:extend(args)
end

--- Add an include directory
--
-- @tparam {string,...} dir These will be treated as BuildGen paths.
function S.c.addInclude ( dir )
	if type(dir) == "table" then
		for d in T.List(dir):iter() do
			state.includes:set(C.path(d), true)
		end
	else
		T.utils.assert_string(1, dir)
		state.includes:set(C.path(dir), true)
	end
end
local function includeArgs ()
	local a = T.List()
	for v in state.includes:keys():iter() do
		a:extend(P.S.c.compiler.flags.include:map():format(v))
	end

	return a
end

--- Define a macro
-- Define a macro during compliation.
--
-- @tparam {[string]=string,...} map A table of key/value pairs to be defined
-- during compilation.
function S.c.define ( map )
	T.utils.assert_arg(1, map, "table")

	T.Map.update(state.defines, map)
end
--- Undefine a macro
-- Stop a macro from being defined.
--
-- @tparam {string,...} ids A list of macro identifiers to be undefined.
function S.c.undefine ( ids )
	if type(ids) == "table" then
		for id in T.List(ids):iter() do
			T.Map.set(state.defines, id, nil)
		end
	else
		T.utils.assert_arg(1, ids, "string")
		T.Map.set(state.defines, ids, nil)
	end
end

local function defineArgs ()
	local a = T.List()
	for k, v in T.Map.iter(state.defines) do
		if type(v) ~= "string" then
			v = ""
		else
			v = "="..v
		end

		a:extend(P.S.c.compiler.flags.define:map():format(k..v))
	end

	return a
end

--- Link a library.
--
-- This just calls `S.ld.addLib()` with the linker being used by S.c.
--
-- @tparam {string,...} dir names of the libraries.
function S.c.addLib ( lib )
	local ln = S.ld.swapState(state.linker)

	S.ld.addLib(lib)

	state.linker = S.ld.swapState(ln)
end

--- Compile a source into an object.
--
-- @tparam string src The file to compile.
-- @tparam {string,...} headers A list of headers that are needed.
-- @tparam string obj The place to put the resulting object file.
function S.c.compileObject (src, headers, obj)
	T.utils.assert_string(1, src)
	T.utils.assert_arg(2, headers, "table")
	T.utils.assert_string(3, obj)

	obj = C.path(obj)
	src = C.path(src)
	headers = T.List(headers):map(C.path)
	headers:append(src)

	local compiler = P.S.c.compiler

	local cmd = T.List{compiler.exe}
	cmd:extend(compiler.flags.compile)

	local debug = state.debug
	if debug == nil then debug = S.c.debug end
	if debug then -- Add the debug flag.
		cmd:extend(compiler.flags.debug)
		S.c.define{DEBUG=true}
		S.c.undefine("NDEBUG")
	else                   -- Add the debug flag.
		S.c.define{NDEBUG=true}
		S.c.undefine("DEBUG")
	end
	local profile = state.profile
	if profile == nil then profile = S.c.profile end
	if profile then -- Add the profile flag.
		cmd:extend(compiler.flags.profile)
	end
	local optimization = state.optimization
	if optimization == nil then optimization = S.c.optimization end
	cmd:extend(compiler.flags.optimize[optimization]) -- Set the optimization level.

	cmd:extend(includeArgs())
	cmd:extend(defineArgs())
	cmd:extend(state.arguments)

	-- Add the desired output file to the command line.
	cmd:extend(compiler.flags.output:map():format(obj))

	cmd:append(src)

	C.addGenerator(headers, cmd, {obj}, {
		description = "Compiling "..obj
	})
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
-- @tparam {string,...} sources A list of sources (both header and source files)
-- that will be used when compiling the executable.
-- @tparam string out The file to be created.  ".exe" will be appended if
-- compiling on Windows.
-- @treturn string The actual path of the created executable.
function S.c.compile ( sources, out )
	T.utils.assert_arg(1, sources, "table")
	T.utils.assert_string(2, out)

	return compile(S.ld.link, sources, out)
end

--- Compile a Static Library
-- Compiles and links a list of files into a static library.
--
-- @tparam {string,...} sources A list of sources (both header and source files)
-- that will be used when compiling the library.
-- @param out The file to be created.  This is just the basename,
-- @treturn string The actual path used for the output executable.
function S.c.compileStatic ( sources, out )
	T.utils.assert_arg(1, sources, "table")
	T.utils.assert_string(2, out)

	return compile(S.ld.linkStatic, sources, out)
end

--- Compile a Shared Library
-- Compiles and links a list of files into a shared library.
--
-- @tparam {string,...} sources A list of sources (both header and source files)
-- that will be used when compiling the library.
-- @tparam string out The file to be created.  This is just the basename,
-- @treturn string The actual path used for the output executable.
function S.c.compileShared ( sources, out )
	T.utils.assert_arg(1, sources, "table")
	T.utils.assert_string(2, out)

	return compile(S.ld.linkShared, sources, out)
end

--- Find Include Directory
-- Find the directory that holds the given file.
--
-- @tparam string file The suffux to search for (ex: `test.h`, `test/test.h`)
-- @return The directory or false.
function S.c.findIncludeDir ( file )
	local f = S.findFile(file, {"/usr/include","/usr/local/include"})
	if f then
		return T.path.dirname(f)
	end

	return false
end

local generatorScript = C.path("c/generateHeader.lua")

--- Create a header file with definitions.
-- Creates a header/source pair with definitions. Currently all values are
-- treated as strings with type  `const char*`.
--
-- @tparam string head Where to put the genereated header.  This is treated
-- as a BuildGen path.
-- @tparam string src Where to put the genereated source file.  This is
-- treated as a BuildGen path.
-- @tparam {[string]=string,...} definitions An object consisting of key/value
-- pairs where the key will be the variable name and the value will be the value.
function S.c.generateHeader ( head, src, definitions )
	T.utils.assert_string(1, head)
	T.utils.assert_string(2, src)
	T.utils.assert_arg(2, definitions, "table")

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
