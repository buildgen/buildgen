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

local function setup () -- So that we can hide our locals.

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
				max     = "-O3",     -- Highest possoble (possibly exparemental)
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

S.c.optitization = "regular"
if D.debug then S.c.optitization = "none" end

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
	out = C.path(out)

	local cmd = List()
	cmd:append(compiler.name)

	for i in iter(compiler.flags.output) do -- Add the desired output file to
		cmd:append(i:format(out))           -- the command line.
	end                                     --

	local o = compiler.flags.optimize[S.c.optitization]
	if o then
		cmd:append(o)
	end

	for a in iter(arguments) do cmd:append(v) end

	for k,v in pairs(sources) do
		sources[k] = C.path(v)
		cmd:append(sources[k])
	end

	C.addGenerator({out}, sources, cmd)
end

end
setup()
setup=nil
