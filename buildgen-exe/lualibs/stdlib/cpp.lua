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


S.cpp = {}
if not P.S.cpp then P.S.cpp = {} end

S.import "ld"

local function setup () -- So that we can hide our locals.

if not P.S.cpp.compiler then
	local compilers = {
		{	name = "g++", -- Name of the executable
			flags = {
				compile  = "-c",
				output   = {"-o", "%s"}, -- the option to set the output file name.
				debug    = "-g",         -- the option to enable debug mode.
				profile  = "-p",         -- the option to enable profiling.
				link     = {"-l", "%s"}, -- the option to link a library.
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
	List(compilers) -- turn tabe into a penlight 'list'

	local compiler;
	for c in iter(compilers) do          -- Find the first compiler they have
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

S.cpp.debug = false
if D.debug then S.cpp.debug = true end

S.cpp.optimization = "regular"
if D.debug then S.cpp.optimization = "none" end

S.cpp.profile = false
if D.debug then S.cpp.profile = true end

local arguments = List()

function S.cpp.addArg ( arg )
	if type(arg) ~= "table" then
		arg = {tostring(arg)}
	end

	for k, v in pairs(arg) do arguments:append(v) end
end

function S.cpp.addInclude ( dir )
	if type(dir) ~= "table" then
		dir = {tostring(dir)}
	end

	for k, v in pairs(dir) do
		S.cpp.addArg({"-I", C.path(v)})
	end
end

S.cpp.addLib = S.ld.addLib
S.ld.addLib "stdc++"

function S.cpp.compile ( out, sources )
	sources = List(sources)
	local compiler = P.S.cpp.compiler

	out = C.path(out)

	local projectRoot = C.path("<") -- Cache this.
	local outRoot     = C.path(">") --

	local toLink = List()

	for source in iter(sources) do
		source = C.path(source)

		if stringx.endswith(source, ".cpp") or
		   stringx.endswith(source, ".CPP") or
		   stringx.endswith(source, ".c++") or
		   stringx.endswith(source, ".CC") or
		   stringx.endswith(source, ".cxx") or
		   stringx.endswith(source, ".CXX")  then
			-- Get path to put object file.

			local object = nil;

			if source:sub(0, #projectRoot) == projectRoot then
				object = C.path(">"..source:sub(#projectRoot)..".o")
			elseif source:sub(0, #outRoot) == outRoot then
				object = C.path(source..".o") -- Already in the out dir.
			else
				object = C.path("@"..source:sub(#projectRoot)..".o") -- Put inside
				                                                     -- the build
			end                                                      -- dir.

			local cmd = List()
			cmd:append(compiler.name)

			if type(compiler.flags.compile) == "table" then
				cmd:extend(compiler.flags.compile)
			else
				cmd:append(compiler.flags.compile)
			end

			for i in iter(compiler.flags.output) do -- Add the desired output file to
				cmd:append(i:format(object))           -- the command line.
			end                                     --

			if S.cpp.debug then                       -- Add the debug flag.
				if type(compiler.flags.debug) == "table" then
					cmd:extend(compiler.flags.debug)
				else
					cmd:append(compiler.flags.debug)
				end
			end
			if S.cpp.profile then                     -- Add the profile flag.
				if type(compiler.flags.profile) == "table" then
					cmd:extend(compiler.flags.profile)
				else
					cmd:append(compiler.flags.profile)
				end
			end

			local o = compiler.flags.optimize[S.cpp.optimization] -- Set the optimization
			if o then                                             -- level.
				if type(o) == "table" then                        --
					cmd:extend(o)                                 --
				else                                              --
					cmd:append(o)                                 --
				end                                               --
			end                                                   --

			cmd:extend(arguments)
			cmd:append(source)

			C.addGenerator({object}, sources, cmd, {
				description = "Compiling "..object
			})
			toLink:append(object)
		end
	end

	S.ld.link(out, toLink)
end

function S.cpp.stash ( )
	old = {
		args = arguments,
		link = nil,

		debug        = S.cpp.debug,
		optimization = S.cpp.optimization,
		profile      = S.cpp.profile,
	}

	S.cpp.load(S.cpp.newState())

	return old
end

function S.cpp.newState ( )
	data = {
		args = {},
		link = nil,
	}

	data.debug = false
	if D.debug then data.debug = true end

	data.optimization = "regular"
	if D.debug then data.optimization = "none" end

	data.profile = false
	if D.debug then data.profile = true end

	return data
end

function S.cpp.load ( data )
	arguments = data.args
	old = {
		args = arguments,
		link = nil,

		debug        = S.cpp.debug,
		optimization = S.cpp.optimization,
		profile      = S.cpp.profile,
	}
	S.cpp.debug        = data.debug
	S.cpp.optimization = data.optimization
	S.cpp.profile      = data.profile
end


end
setup()
setup=nil
