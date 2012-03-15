-- Copyright 2011-2012 Kevin Cox

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

S.imported = true

--- Import Penlight
T = {}

require "lfs"

T.List = require "pl.List"
T.class = require "pl.class"
T.Map = require "pl.Map"
T.Set = require "pl.Set"

T.OrderedMap = require "pl.OrderedMap"
T.MultiMap = require "pl.MultiMap"
--T.TypedList = require "pl.TypedList"

T.app = require "pl.app"
T.array2d = require "pl.array2d"
T.comprehension = require "pl.comprehension"
T.config = require "pl.config"
T.data = require "pl.data"
T.dir = require "pl.dir"
T.file = require "pl.file"
T.func = require "pl.func"
T.input = require "pl.input"
T.lapp = require "pl.lapp"
T.lexer = require "pl.lexer"
T.luabalanced = require "pl.luabalanced"
T.operator = require "pl.operator"
T.path = require "pl.path"
T.permute = require "pl.permute"
T.pretty = require "pl.pretty"
T.seq = require "pl.seq"
T.sip = require "pl.sip"
T.stringio = require "pl.stringio"
T.stringx = require "pl.stringx"
T.tablex = require "pl.tablex"
T.test = require "pl.test"
T.text = require "pl.text"
T.utils = require "pl.utils"

T.stringx.import()

if not S.prefix then
	if S.os.compliance == "win32" then
		D.resolvePath("prefix", "C:/Program Files/")
	else
		D.resolvePath("prefix", "/usr/local/")
	end

	S.prefix = D.prefix
end

--- Add a Target to the Default Build
--
-- Adds <span class="code">path</span> to the default buid.  This means that if
-- the builder doesn't specify what they want build this will be built.
--
-- @param path The path of the target.
function S.addToDefault ( path )
	S.addToTarget("all", path)
end

--- Add a target to a magic target.
--
-- @param target The name of the magic target.
-- @param path The path to add.  This is a BuildGen path.
function S.addToTarget ( target, path )
	C.addDependancy(target, path, { magic = true })
end

--- Find the path to an executable.
--
-- Look for the program in the location(s) appropriate to the system.  For
-- example in UNIX-style os's it will search your path.
--
-- @param name The path of the executable.
function S.findExecutable ( name )
	if name:find("/", 1, true) == 1 then
		if T.path.isfile(name) then
			return name
		end

		return false
	end

	for k, v in pairs(S.path) do
		local p = T.path.join(v, name);
		if T.path.isfile(p) then
			return p
		end
	end

	return false
end

--- Add a Target to the Install Target
--
-- Installs <span class="code">path</span>.  If <span class="code">path</span>
-- is a directory it will be installed recursivly.  Please note that the
-- directory will be installed, not it's contents.  Example:
-- <span class="code">S.install("foo/", "bar/")</span> will result in
-- <span class="code">bar/foo/</span> not <span class="code">bar/{foo_contents}
-- </span>.
--
-- @param path The path of the target.
-- @param to Where to install the file.  This is treated as relative to the
--	install prefix (S.prefix) unless it is prefixed by a ‘/’ in which case it
--	is treated as an absolute path.
function S.install ( path, to )
	S.import "util"
	if string.find(to, "/", 1, true) ~= 1 then
		to = S.prefix..to.."/"
	end
	local apath = C.path(path)

	if T.path.isdir(apath) then
		local i = string.find(string.sub(string.reverse(apath), 2), "/", 1, true)
		if i then
			dirname = apath:sub(-i)
		end

		for root, dirs, files in T.dir.walk(apath) do
			for f in T.List(files):iter() do
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

do
	--- The system's path
	-- A list of directorys in the systems executable search path.  Please note
	-- that the order is significant and the directories should be searched from
	-- first to last.
	S.path = T.List()

	local ospath = os.getenv("PATH")
	local seperator = ""

	if     S.os.compliance == "posix"   then
		seperator = ":"
	elseif S.os.compliance == "win32" then
		seperator = ";" --@help Is the right?
	end

	if seperator then
		S.path = ospath:split(seperator)
	end
end

