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

T = require "lfs"
require "pl"
stringx.import()

S.version = _s_version
S.os = _s_os

if not S.prefix then
	if S.os == "windows" then
		D.resolvePath("prefix", "C:/Program Files/")
	else
		D.resolvePath("prefix", "/usr/local/")
	end

	S.prefix = D.prefix
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
	S.import "util"
	if string.find(to, "/", 1, true) ~= 1 then
		to = S.prefix..to.."/"
	end
	local apath = C.path(path)

	if lfs.attributes(apath, "mode") == "directory" then
		local i = string.find(string.sub(string.reverse(apath), 2), "/", 1, true)
		if i then
			dirname = string.sub(apath, -i)
		end

		for root, dirs, files in dir.walk(apath) do
			for f in iter(files) do
				t = to..dirname..string.sub(root, #apath).."/"..f

				S.util.install(root.."/"..f, t)
				C.addDependancy("install", t, { magic = true })
			end
		end
	else
		local i = string.find(string.reverse(apath), "/", 1, true)
		if i then
			to = to.."/"..string.sub(apath, -i)
		end

		S.util.install(path, to)
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

