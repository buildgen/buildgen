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

S.import "ld"

local function setup () -- So that we can hide our locals.

if not P.S.c.compiler then
	local compilers = {
		{	name = "gcc", -- Name of the executable
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
		error("Error: No C compiler found.", 0)
	else
		P.S.c.compiler = compiler
	end
end

S.c.debug = false
if D.debug then S.c.debug = true end

S.c.optimization = "regular"
if D.debug then S.c.optimization = "none" end

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
		S.c.addArg({"-I", C.path(v)})
	end
end

S.c.addLib = S.ld.addLib

function S.c.compile ( out, sources )
	sources = List(sources)
	local compiler = P.S.c.compiler

	out = C.path(out)

	local toLink = List()

	for source in iter(sources) do
		source = C.path(source)

		if stringx.endswith(source, ".c") or stringx.endswith(source, ".C") then
			-- Get path to put object file.
			local object = C.path("@"..source:sub(#lfs.currentdir()+2, -3)..".o")

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

			cmd:extend(arguments)
			cmd:append(source)

			C.addGenerator({object}, sources, cmd, {
				description = "Generating "..object
			})
			toLink:append(object)
		end
	end

	S.ld.link(out, toLink)
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

	C.addGenerator({name}, {}, cmd, {
		description = "Generating '"..name.."'..."
	})
end

end
setup()
setup=nil
