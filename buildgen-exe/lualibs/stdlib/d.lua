--- The D Library
-- @module S.d

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

S.d = {}
if not P.S.d then P.S.d = {} end

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
S.d.optimization = "regular"
if D.debug then S.d.optimization = "none" end

--- Whether to profile.
-- If true profiling code will be present in the resulting executable. This
-- value defaults to true if D.debug is set otherwise false.
S.d.profile = false
if D.debug then S.d.profile = true end

--- Whether to produce debugging symbols.
-- If true debugging symbols will be produced in the resulting executable. This
-- value defaults to true if D.debug is set otherwise false.
S.d.debug = false
if D.debug then S.d.debug = true end

--- Whether to produce unittests.
-- If true unittests will be compiled into the executable. This
-- value defaults to true if D.debug is set otherwise false.
S.d.unittest = false
if D.debug then S.d.unittest = true end

--- Enable compiler warnings.
-- If true warnings will be enabled.
S.d.warnings = false
if D.debug then S.d.warnings = true end

--- Create new D state
-- Creates and returns an opaque state.  This state is a table and is therefore
-- passed by refrence.
--
-- @return The newly created state.
function S.d.newState ( )
	local data = {
		arguments     = T.List(),
		linkArguments = T.List(),
	}

	local s = S.d.swapState(data)

	S.d.loadState(s)

	return data
end

--- Stashes the current state.
-- Returns the current state and loads a new state.  This is equivilent to
-- S.d.swapState(S.d.newState()).
--
-- @return The old state.
function S.d.stashState ( )
	return S.d.swapState(S.d.newState())
end

--- Swap the state
-- Swaps new with the current state.
--
-- @param new The new state to load.
-- @return The old state.
function S.d.swapState ( new )
	local old = state

	old.debugOveride        = S.d.debugOveride
	old.optimizationOveride = S.d.optimizationOveride
	old.profileOveride      = S.d.profileOveride
	old.unittestOveride     = S.d.unittestOveride

	S.d.loadState(new)

	return old
end

--- Load a state
-- Loads the state data
--
-- @param data The state to load.
function S.d.loadState ( data )
	state = data

	S.d.debugOveride        = data.debugOveride
	S.d.optimizationOveride = data.optimizationOveride
	S.d.profileOveride      = data.profileOveride
	S.d.unittestOveride     = data.unittestOveride
end

if not P.S.d.compiler then
	local compilers = {
		{	name = "dmd", -- Name of the executable
			flags = {
				compile    = {"-c"},--, "-output-bc"},
				link       = {},
				linkShared = "-shared",
				linkStatic = "-lib",
				output     = {"-of%s"}, -- the option to set the output file name.
				debug      = "-g",          -- the option to enable debug mode.
				unittest   = "-unittest",   -- the option to enable unittest.
				profile    = {},            -- the option to enable profiling.
				warnings   = {"-w"},        -- Enable compiler warnings.
				define     = {"-D%s=%s"},   -- the option to define a macro.
				lib        = {"-l%s"},      -- the option to link a library.
				include    = {"-I%s"},  -- the option to add an include directory.
				optimize   = {              -- Flags for different levels of optimization.
					none    = {},
					quick   = {},
					regular = "-O",     -- Default optimazation.
					full    = "-O",
					max     = {"-O"}      -- Highest possoble (possibly exparemental)
				}
			}
		},
		{	name = "ldc2", -- Name of the executable
			flags = {
				compile    = {"-c"},--, "-output-bc"},
				link       = {},
				linkShared = "-shared",
				linkStatic = "-lib",
				output     = {"-of", "%s"}, -- the option to set the output file name.
				debug      = "-g",          -- the option to enable debug mode.
				unittest   = "-unittest",   -- the option to enable unittest.
				profile    = {},            -- the option to enable profiling.
				warnings   = {"-w"},        -- Enable compiler warnings.
				define     = {"-D%s=%s"},   -- the option to define a macro.
				lib        = {"-l%s"},      -- the option to link a library.
				include    = {"-I", "%s"},  -- the option to add an include directory.
				optimize   = {              -- Flags for different levels of optimization.
					none    = "-O0",
					quick   = "-O",
					regular = "-O",     -- Default optimazation.
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
		error("Error: No D compiler found.", 0)
	else
		P.S.d.compiler = compiler
	end
end

--- Overide the default optimization level.
S.d.optimizationOveride = S.d.optimizationOveride

--- Overide the default profile setting.
S.d.profileOveride = S.d.profileOveride

--- Overide the default profile setting.
S.d.debugOveride = S.d.debugOveride

--- Overide the default unittest setting.
S.d.unittestOveride = S.d.unittestOveride

--- Add an argrment to the linker.
-- Add an argument to the linker command line.  Please try to avoid using this
-- as it is not portable across compilers.  Please use the other functions that
-- modify the command line (Such as S.d.optimization and S.d.define()) as they
-- are localized to the compiler being used.
--
-- @tparam {string,...} args arguments to be added to the linker command line.
function S.d.addLinkArg ( arg )
	if type(arg) ~= "table" then
		arg = {tostring(arg)}
	end

	state.linkArguments:extend(arg)
end

--- Add an argrment.
-- Add an argument to the compiler command line.  Please try to avoid using this
-- as it is not portable across compilers.  Please use the other functions that
-- modify the command line (Such as S.d.optimization and S.d.define()) as they
-- are localized to the compiler being used.
--
-- @tparam {string,...} arguments to be added to the compiler command line.
function S.d.addArg ( arg )
	if type(arg) ~= "table" then
		arg = {tostring(arg)}
	end

	for k, v in pairs(arg) do state.arguments:append(v) end
end

--- Add an include directory
--
-- @tparam {string,...} dir Directories to be added.  These will be treated as
-- BuildGen paths.
function S.d.addInclude ( dir )
	if type(dir) ~= "table" then
		dir = T.List{tostring(dir)}
	end

	for v in dir:iter() do
		v = C.path(v)
		for l, w in pairs(P.S.d.compiler.flags.include) do
			S.d.addArg(w:format(v))
		end
	end
end

--- Define a macro
-- Define a macro during compliation.
--
-- @tparam {[string]=string,...} map A table of key/value pairs to be defined
-- during compilation.
function S.d.define ( map )
	for k, v in pairs(map) do
		if type(v) ~= "string" then
			v = ""
		end
		for l, w in pairs(P.S.d.compiler.flags.define) do
			S.d.addArg(w:format(k, v))
		end
	end
end

--- Link a library.
--
-- This just calls `S.ld.addLib()` with the linker being used by `S.d`.
--
-- @param dir a string or list of strings as the name of the libraries.
function S.d.addLib ( lib )
	if type(lib) ~= "table" then
		lib = {tostring(lib)}
	end

	for k, v in pairs(lib) do
		if type(v) ~= "string" then
			v = ""
		end
		for l, w in pairs(P.S.d.compiler.flags.lib) do
			S.d.addLinkArg(w:format(v))
		end
	end
end

--- Compile a source into an object.
--
-- @tparam string src The source file to compile.
-- @tparam {string,...} depends A list of files the object depends on
-- (specificly templates).
-- @tparam string obj The place to put the resulting object file.
function S.d.compileObject ( src, depends, obj )
	T.utils.assert_string(1, src)
	T.utils.assert_arg(2, depends, "table")
	T.utils.assert_string(3, obj)

	src = C.path(src)
	depends = T.List(depends):map(C.path):append(src)
	obj = C.path(obj)

	local compiler = P.S.d.compiler

	local oldarguments = state.arguments
	state.arguments = T.List()

	S.d.addArg(compiler.name)
	S.d.addArg(compiler.flags.compile)

	S.d.addArg(oldarguments)

	local warnings = S.d.warningsOveride
	if warnings == nil then warnings = S.d.warnings end
	if warnings then                      -- Add the warning flag
		S.d.addArg(compiler.flags.warnings)
	end
	local debug = S.d.debugOveride
	if debug == nil then debug = S.d.debug end
	if debug then                      -- Add the debug flag.
		S.d.addArg(compiler.flags.debug)
		--S.d.define{DEBUG=true}
	else                   -- Add the debug flag.
		--S.d.define{NDEBUG=true}
	end
	local unittest = S.d.unittestOveride
	if unittest == nil then unittest = S.d.unittest end
	if unittest then                      -- Add the unittest flag.
		S.d.addArg(compiler.flags.unittest)
	end
	local profile = S.d.profileOveride
	if profile == nil then profile = S.d.profile end
	if profile then                    -- Add the profile flag.
		S.d.addArg(compiler.flags.profile)
	end
	local optimization = S.d.optimizationOveride
	if optimization == nil then optimization = S.d.optimization end
	local o = compiler.flags.optimize[optimization] -- Set the optimization
	if o then                                       -- level.                                --
		S.d.addArg(o)                             --
	end                                             --

	for i in T.List(compiler.flags.output):iter() do -- Add the desired output file to
		S.d.addArg(i:format(obj))                    -- the command line.
	end                                              --

	S.d.addArg(src)

	C.addGenerator(depends, state.arguments, {obj}, {
		description = "Compiling "..obj
	})

	state.arguments = oldarguments;

	return obj
end

--- Link into an executable
-- @tparam {string,...} objects A list of objects to link.
-- @tparam string out Where to put the executable.  This is treated as a
-- BuildGen path. ".exe" will be appended on windows.
function S.d.link(objects, out)
	T.utils.assert_arg(1, objects, "table")
	T.utils.assert_string(2, out)

	objects = T.List(objects):map(C.path)
	out = C.path(out)

	local compiler = P.S.d.compiler

	local oldarguments = state.linkArguments
	state.linkArguments = T.List()

	S.d.addLinkArg(compiler.name)
	S.d.addLinkArg(compiler.flags.link)

	S.d.addLinkArg(oldarguments)

	local warnings = S.d.warningsOveride
	if warnings == nil then warnings = S.d.warnings end
	if warnings then                      -- Add the warning flag
		S.d.addLinkArg(compiler.flags.warnings)
	end                                     --

	for i in T.List(compiler.flags.output):iter() do -- Add the desired output file to
		S.d.addLinkArg(i:format(out))                -- the command line.
	end                                              --

	S.d.addLinkArg(objects)

	C.addGenerator(objects, state.linkArguments, {out}, {
		description = "Linking "..out
	})

	state.linkArguments = oldarguments;
end

--- Compile a Library
-- Compiles and links a list of files into a lirary.
--
-- @tparam {string,...} objects A list of objects that will be used to compile
-- the library.
-- @tparam string out The file to be created.  An appropriate extension will be
-- added.
function S.d.linkShared(objects, out)
	T.utils.assert_arg(1, objects, "table")
	T.utils.assert_string(2, out)

	objects = T.List(objects):map(C.path)
	out = C.path(out)

	local compiler = P.S.d.compiler

	local oldarguments = state.linkArguments
	state.linkArguments = T.List()

	S.d.addLinkArg(compiler.name)
	S.d.addLinkArg(compiler.flags.link)

	S.d.addLinkArg(compiler.flags.linkShared)

	S.d.addLinkArg(oldarguments)

	local warnings = S.d.warningsOveride
	if warnings == nil then warnings = S.d.warnings end
	if warnings then                      -- Add the warning flag
		S.d.addLinkArg(compiler.flags.warnings)
	end                                            --

	for i in T.List(compiler.flags.output):iter() do -- Add the desired output file to
		S.d.addLinkArg(i:format(out))                -- the command line.
	end                                              --

	S.d.addLinkArg(objects)

	C.addGenerator(objects, state.linkArguments, {out}, {
		description = "Linking "..out
	})

	state.linkArguments = oldarguments;
end

--- Compile an Executable
-- Compiles and links a list of files into executables.
--
-- @tparam {string,...} sources A list of sources (bot header and source files)
-- that will be used when compiling the executable.
-- @tparam string out The file to be created.  ".exe" will be appended if
-- compiling on Windows.
function S.d.compile ( sources, out )
	T.utils.assert_arg(1, sources, "table")
	T.utils.assert_string(2, out)

	sources = T.List(sources):map(C.path)
	out = C.path(out)

	local projectRoot = C.path("<") -- Cache this.
	local outRoot     = C.path(">") --

	local objects = T.List()

	for source in sources:iter() do
		source = C.path(source)

		local object = C.path("@"..source..".o")

		S.d.compileObject(source, sources, object)
		objects:append(object)
	end

	S.d.link(objects, out)
end

--- Compile a Shared Library
-- Compiles and links a list of files into a shared library.
--
-- @tparam {string,...} sources A list of sources that will be used to compile
-- the library.
-- @tparam string out The file to be created.  An appropriate extension will be added.
function S.d.compileShared ( sources, out )
	T.utils.assert_arg(1, sources, "table")
	T.utils.assert_string(2, out)

	sources = T.List(sources):map(C.path)
	out = C.path(out)

	local projectRoot = C.path("<") -- Cache this.
	local outRoot     = C.path(">") --

	local objects = T.List()

	for source in sources:iter() do
		source = C.path(source)

		local object = nil; -- Get path to put object file.

		local object = C.path("@"..source..".o")

		S.d.compileObject(source, object)
		objects:append(object)
	end

	S.d.linkShared(objects, out)
end

S.d.swapState(S.d.newState())

end
