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


--- The Standard library namespace.
S.imported = true

require "lfs"
require "pl"
stringx.import()

S.version = _s_version
S.os = _s_os

if not S.prefix then
	if S.os == "windows" then
		S.prefix = "C:/Program Files/"
	else
		S.prefix = "/usr/local/"
	end
end

local shutdownFunctions = {}
function S._doShutdownFunctions ( )
	for k,v in pairs(shutdownFunctions) do v() end

	shutdownFunctions = {}
end

function S.registerShutdown ( func )
	assert(type(func) == "function", "S.registerShutdown expects a function.")
	print "S.shutdown is depreciated."

	shutdownFunctions[#shutdownFunctions+1] = func
end

function S.addToDefault ( path )
	C.addDependancy("all", path, { magic = true })
end

function S.findExecutable ( name )
	if name:find("/", 1, true) == 1 then
		if path.isfile(name) then
			return name
		end

		return false
	end

	for k, v in pairs(S.path) do
		local p = path.join(v, name);
		if path.isfile(p) then
			return p
		end
	end

	return false
end

function S.install ( path, to )
	if string.find(to, "/", 1, true) ~= 1 then
		to = S.prefix..to
	end
	local apath = C.path(path)

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
		local i = string.find(string.reverse(apath), "/", 1, true)
		if i then
			to = to.."/"..string.sub(apath, -i)
		end

		C.addGenerator({to}, {path}, {"*install", "-D", apath, C.path(to)}, {
			description = "Installing "..to
		})
		C.addDependancy("install", to, { magic = true })
    end
end

local function setup ( )

	--- Populate S.path ---
	S.path = {} -- Will hold folders in the operating systems path.

	local ospath = os.getenv("PATH")
	local seperator = ""

	if     S.os == "posix"   then
		seperator = ":"
	elseif S.os == "windows" then
		seperator = ";" --@help Is the right?
	end

	if seperator then
		S.path = ospath:split(seperator)
	end
end
setup()
setup = nil

