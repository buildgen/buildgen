-- Copyright 2011 Kevin Cox

--[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]
[                                                                              ]
[  Permission is hereby granted, free of charge, to any person obtaining a     ]
[  copy of this software and associated documentation files (the "Software"),  ]
[   to deal in the Software without restriction, including without limitation  ]
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
[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]


S.c = {}

S.c.flags = ""
S.c.flag = {}

S.c.flag.debug = "-g" --@todo Set these when this library is loaded ( or generate this library with the proper flags )
S.c.flag.profile = "-g"
S.c.flag.optomize = "-O"

S.c.arguments = {}

function S.c.addArg ( arg )
	if type(arg) ~= "table" then
		arg = {tostring(arg)}
	end

	for k, v in pairs(arg) do S.c.arguments[#S.c.arguments+1] = v end
end

function S.c.addInclude ( dir )
	if type(dir) ~= "table" then
		dir = {tostring(dir)}
	end

	for k, v in pairs(dir) do S.c.addArg({"-I", D.path(v)}) end
end

function S.c.compile ( out, sources )
	local cmd = {"*gcc", "-o", D.path(out) }

	for k, v in pairs(S.c.arguments) do cmd[#cmd+1] = v end

	for k,v in pairs(sources) do
		sources[k] = D.path(v)
		cmd[#cmd+1] = sources[k]
	end

	D.addGenerator({D.path(out)}, sources, cmd)
end
