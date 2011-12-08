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


S.ld = {}
if not P.S.ld then P.S.ld = {} end

local function setup () -- So that we can hide our locals.

if not P.S.ld.linker then
	local linkers = {
		{	name="cc", -- Name of the executable
			flags = {
				link = {}, -- commands that need to be present.
				out  = {"-o", "%s"},
				lib  = {"-l", "%s"}
			}
		},
	}
	List(linkers) -- turn tabe into a penlight 'list'

	local linker;
	for l in iter(linkers) do          -- Find the first compiler they have
		if S.findExecutable(l.name) then -- installed on thier system.
			linker = l
			linker.name = S.findExecutable(linker.name)

			break
		end
	end

	if linker == nil then
		error("Error: No linker found.", 0)
	else
		P.S.ld.linker = linker
	end
end

local arguments = List()
function S.ld.addArg ( arg )
	if type(arg) ~= "table" then
		arg = {tostring(arg)}
	end

	arguments:extend(arg)
end

function S.ld.addLib ( lib )
	if type(lib) ~= "table" then
		lib = {tostring(lib)}
	end

	local args = List()
	for k, v in pairs(lib) do
		for l, w in pairs(P.S.ld.linker.flags.lib) do
			args:append(w:format(v))
		end
	end
	S.ld.addArg(args)
end

function S.ld.link ( out, objects )
	linker = P.S.ld.linker

	local cmd = List()
	cmd:append(linker.name)

	if type(linker.flags.link) == "table" then
		cmd:extend(linker.flags.link)
	else
		cmd:append(linker.flags.link)
	end

	for i in iter(linker.flags.out) do -- Add the desired output file to
		cmd:append(i:format(out))     -- the command line.
	end

	cmd:extend(arguments)
	cmd:extend(objects)

	C.addGenerator({out}, objects, cmd, {
		description = "Linking "..out
	})
end

function S.ld.stashState ( )
	return S.ld.swapState(S.ld.newState())
end

function S.ld.swapState ( new )
	old = {
		args = arguments,
	}

	S.ld.load(new)

	return old
end

function S.ld.newState ( )
	data = {
		args = {},
	}

	return data
end

function S.ld.loadState ( data )
	arguments = data.args
end

end
setup()
setup=nil
