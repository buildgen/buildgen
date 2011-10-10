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
if not P.S.c then P.S.c = {} end

local function setup () -- So that we can hide our locals.

if not P.S.c.compiler then
	local compilers = {
		{ name="gcc", -- Name of the executable
			flags = {
				output   = {"-o", "%s"}, -- the option to set the output file name.
				debug    = "-g",         -- the option to enable debug mode.
				profile  = "-p",         -- the option to enable profiling.
				include  = {"-I", "%s"}, -- the option to add an include directory.
				optimize = {             -- Flags for different levels of optimization.
					none    = "",
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

	P.S.c.compiler = compiler
end

S.c.optimization = "regular"
if D.debug then S.c.optimization = "none" end

S.c.debug = false
if D.debug then S.c.debug = true end

S.c.profile = false
if D.debug then S.c.profile = true end

local arguments = List()

function S.c.addArg ( arg )
	if type(arg) ~= "table" then
		arg = {tostring(arg)}
	end

	for k, v in pairs(arg) do arguments:append(v) end
end

function S.c.addInclude ( dir )
	if type(dir) ~= "table" then
		dir = {tostring(dir)}
	end

	for k, v in pairs(dir) do
		local a =
		S.c.addArg({"-I", C.path(v)})
	end
end

function S.c.compile ( out, sources )
	local compiler
	if P.S.c.compiler == nil then
		error("Error: No C compiler found.", 0)
	else
		compiler = P.S.c.compiler
	end

	out = C.path(out)

	local cmd = List()
	cmd:append(compiler.name)

	for i in iter(compiler.flags.output) do -- Add the desired output file to
		cmd:append(i:format(out))           -- the command line.
	end                                     --

	if S.c.debug then                       -- Add the debug flag.
		if type(compiler.flags.debug) == "table" then
			cmd:extend(compiler.flags.debug)
		else
			cmd:append(compiler.flags.debug)
		end
	end
	if S.c.profile then                     -- Add the profile flag.
		if type(compiler.flags.profile) == "table" then
			cmd:extend(compiler.flags.profile)
		else
			cmd:append(compiler.flags.profile)
		end
	end

	local o = compiler.flags.optimize[S.c.optimization] -- Set the optimization
	if o then                                           -- level.
		if type(o) == "table" then                      --
			cmd:extend(o)                               --
		else                                            --
			cmd:append(o)                               --
		end                                             --
	end                                                 --

	for v in iter(arguments) do cmd:append(v) end

	for k,v in pairs(sources) do
		sources[k] = C.path(v)
		cmd:append(sources[k])
	end

	C.addGenerator({out}, sources, cmd)
end

function S.c.generateHeader ( name, definitions )
	local generatorScript = S.lualibsRoot .. "c/generateHeader.lua"

	cmd = List()
	cmd:append "*lua"
	cmd:append(generatorScript)
	cmd:append(C.path(name))

	for k,v in pairs(definitions) do
		cmd:append(k.."="..v)
	end

	C.addGenerator({name}, {}, cmd)
end

end
setup()
setup=nil
