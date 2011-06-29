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


--- The Standard library namespace.
S.imported = true

require "lfs"

if not S.prefix then S.prefix = "/usr/local/" end

local shutdownFunctions = {}
function S._doShutdownFunctions ( )
	for k,v in pairs(shutdownFunctions) do v() end

	shutdownFunctions = {}
end

function S.registerShutdown ( func )
	assert(type(func) == "function", "S.registerShutdown expects a function.")

	shutdownFunctions[#shutdownFunctions+1] = func
end

function S.addToDefault ( path )
	D.addDependancy(">all", path)
end

function S.install ( path, to )
	if string.find(to, "/", 1, true) ~= 1 then
		to = S.prefix..to
	end
	local apath = D.path(path)

	if lfs.attributes(apath, "mode") == "directory" then
		for file in lfs.dir(apath) do
			if file ~= "." and file ~= ".." then
				local dirname = apath
				local i = string.find(string.reverse(dirname), "/", 1, true)
				if i == #dirname then -- Trailing slash
					dirname = string.sub(dirname, 1, -i+1)
					i = string.find(string.reverse(dirname), "/", 1, true)
				end
				if i then
					dirname = string.sub(dirname, -i+1)
				end

				S.install(path..'/'..file, to.."/"..dirname.."/")
			end
		end
	else
		local apath = D.path(path)
		local i = string.find(string.reverse(apath), "/", 1, true)
		if i then
			to = to.."/"..string.sub(apath, -i)
		end

		D.addGenerator({to}, {path}, {"*install", "-D", D.path(path), D.path(to)})
		D.addDependancy(">install", to)
    end
end
