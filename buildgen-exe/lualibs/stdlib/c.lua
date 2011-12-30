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


S.c = {}

S.import "ld"

if not P.S.c then P.S.c = {} end

do -- So that we can hide our locals.
local state = {}

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

function S.c.stashState ( )
	return S.c.swapState(S.c.newState())
end

function S.c.swapState ( new )
	local old = state

	old.debug        = S.c.debug
	old.optimization = S.c.optimization
	old.profile      = S.c.profile

	S.c.loadState(new)

	return old
end

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
	for c in iter(compilers) do          -- Find the first compiler they have
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

function S.c.addArg ( arg )
	if type(arg) ~= "table" then
		arg = {tostring(arg)}
	end

	for k, v in pairs(arg) do state.arguments:append(v) end
end

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

function S.c.addLib ( lib )
	local ln = S.ld.swapState(state.linker)

	S.ld.addLib(lib)

	state.linker = S.ld.swapState(ln)
end

function S.c.compile ( out, sources )
	local ln = S.ld.swapState(state.linker) -- Use our linker.

	sources = T.List(sources)
	local compiler = P.S.c.compiler

	local projectRoot = C.path("<") -- Cache this.
	local outRoot     = C.path(">") --

	out = C.path(out)

	local h, s = T.List(), T.List()
	for source in iter(sources) do
		if source:match("[Hh]") then
			h:append(source)
		else
			s:append(source)
		end
	end

	local oldarguments = state.arguments
	state.arguments = T.List()

	S.c.addArg(compiler.name)
	S.c.addArg(compiler.flags.compile)

	S.c.addArg(oldarguments)

	if S.c.debug then                     -- Add the debug flag.
		S.c.addArg(compiler.flags.debug)
	end
	if S.c.profile then                    -- Add the profile flag.
		S.c.addArg(compiler.flags.profile)
	end
	local o = compiler.flags.optimize[S.c.optimization] -- Set the optimization
	if o then                                             -- level.                                --
		S.c.addArg(o)                                   --
	end

	local length = #state.arguments
	local toLink = T.List()

	for source in iter(sources) do
		source = C.path(source)

		-- Get path to put object file.
		local object = nil;

		if source:sub(0, #projectRoot) == projectRoot then
			object = C.path(">"..source:sub(#projectRoot)..".o")
		elseif source:sub(0, #outRoot) == outRoot then
			object = C.path(source..".o") -- Already in the out dir.
		else
			object = C.path("@"..source:sub(#projectRoot)..".o") -- Put inside
			                                                     -- the build
		end

		for i in iter(compiler.flags.output) do -- Add the desired output file to
			S.c.addArg(i:format(object))      -- the command line.
		end                                               -- dir.

		S.c.addArg(source)

		h:append(source) -- Prepare the dependancies.

		C.addGenerator({object}, h, state.arguments, {
			description = "Compiling "..object
		})
		toLink:append(object)

		state.arguments:chop(length) -- Remove the source from the command.
		h:pop() -- Remove the source from the headers.
	end

	S.ld.link(out, toLink)

	state.arguments = oldarguments;
	state.linker = S.ld.swapState(ln) -- Put thier linker back.
end

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
		description = "Generating '"..head.."'..."
	})
end

end
