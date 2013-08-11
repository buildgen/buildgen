--- The S Namespace
-- The standard library base BuildGen functions.  These are only available after
-- importing "stdlib" (`S.import"stdlib"`) or any other library.
-- @module stdlib

-- Copyright 2011-2012 Kevin Cox

--[[ --------------------------------------------------------------------------]
[                                                                              ]
[  This software is provided 'as-is', without any express or implied           ]
[  warranty. In no event will the authors be held liable for any damages       ]
[  arising from the use of this software.                                      ]
[                                                                              ]
[  Permission is granted to anyone to use this software for any purpose,       ]
[  including commercial applications, and to alter it and redistribute it      ]
[  freely, subject to the following restrictions:                              ]
[                                                                              ]
[  1. The origin of this software must not be misrepresented; you must not     ]
[     claim that you wrote the original software. If you use this software in  ]
[     a product, an acknowledgment in the product documentation would be       ]
[     appreciated but is not required.                                         ]
[                                                                              ]
[  2. Altered source versions must be plainly marked as such, and must not be  ]
[     misrepresented as being the original software.                           ]
[                                                                              ]
[  3. This notice may not be removed or altered from any source distribution.  ]                                                           *
[                                                                              ]
[-----------------------------------------------------------------------------]]

P.S.stdlib = P.S.stdlib or {}

--- Resolve a path passed as a define.
--
-- Replaces the value in the D prefix with an abosolute path.  The path is
-- treated as relitive to the directory form which BuildGen was called.  This
-- should be used whenever trating a value defined on the command line as a
-- path.  If this is not used relitave paths will not be relitive to the
-- direcotory the caller expects.
--
-- @tparam string path The key of the defined value.
-- @param default The value to be put if the value is not defined.  Defaults to
-- `nil`.
function D.resolvePath ( path , default )
	T.utils.assert_string(1, path)

	if not D[path] then
		D[path] = default
	else
		D[path] = T.path.expanduser(D[path])

		if not T.path.isabs(D[path]) then
			D[path] = ">"..D[path]
		end
	end

	D[path] = C.path(D[path])
end

if not S.prefix then
	if S.os.compliance == "win32" then
		D.resolvePath("prefix", "C:/Program Files/")
	else
		D.resolvePath("prefix", "/usr/local/")
	end

	--- The install prefix.
	--
	-- This is the base directory where files will be installed.
	S.prefix = D.prefix
end

--- Add a Target to the Default Build
--
-- Adds `path` to the default build.  This means that if the builder doesn't
-- specify what they want build this will be built.
--
-- @tparam string path The path of the target.
function S.addToDefault ( path )
	T.utils.assert_string(1, path)

	S.addToTarget("all", path)
end

--- Add a Magic Target to the Default Build
--
-- Adds `targ` to the default build.  This means that if the builder doesn't
-- specify what they want build this will be built.
--
-- @tparam string targ The target.
function S.addMagicToDefault ( targ )
	T.utils.assert_string(1, targ)

	S.addTargetToTarget("all", targ)
end

--- Add a path to a magic target.
--
-- @tparam string target The name of the magic target.
-- @tparam string path The path to add.  This is a BuildGen path.
function S.addToTarget ( target, path )
	T.utils.assert_string(1, target)
	T.utils.assert_string(2, path)

	C.addDependancy(target, path, { magic = true })
end

--- Add a target to a magic target.
--
-- @tparam string target The name of the magic target.
-- @tparam string deptarget The path to add.  This is a BuildGen path.
function S.addTargetToTarget ( target, deptarget )
	T.utils.assert_string(1, target)
	T.utils.assert_string(2, deptarget)

	C.addDependancy(target, deptarget, { magic = true, magicsrc = true })
end

--- Find the path to an executable.
--
-- Look for the program in the location(s) appropriate to the system.  For
-- example in UNIX-style os's it will search your path.
--
-- @tparam string name The name of the executable.
-- @treturn string The path of the executable or `false` if not found.
function S.findExecutable ( name )
	T.utils.assert_string(1, name)

	if T.path.isabs(name) then -- Absolute path.
		if T.path.isfile(name) then
			return name
		else
			return false
		end
	end

	for d in S.path:iter() do
		local p = T.path.join(d, name);

		if T.path.isfile(p) then
			return p
		end

		if S.style == "win32" then
			for e in S.pathext:iter() do
				local f = T.path.join(p, e);
				if T.path.isfile(p) then
					return p
				end
			end
		end
	end

	return false
end

P.S.stdlib.foundFiles = T.Map()

--- Find a file.
-- This function recursivly searches the given directory to find the specified
-- file.
--
-- @tparam string file The suffux to search for (ex: `test.h`, `test/test.h`)
-- @tparam {string,...} dirs The directories to search in.
-- @return The path to the file or `false` if it was not found.
function S.findFile ( file, dirs )
	if type(file) ~= "table" then
		file = {tostring(file)}
	end
	if type(dirs) ~= "table" then
		dirs = {tostring(dirs)}
	end
	file = T.List(file)
	dirs = T.List(dirs):map(C.path)
	T.utils.assert_arg(1, file, "table")
	T.utils.assert_arg(2, dirs, "table")

	local foundFiles = P.S.stdlib.foundFiles

	----- Check the cache.
	for tf in file:iter() do
		if foundFiles[tf] then
			for f in foundFiles[tf]:iter() do
				if f:endswith(tf) then
					return f
				end
			end
		end
	end

	----- Actually look for it.
	for dir in dirs:iter() do
		for root, _, files in T.dir.walk(dir) do
			for tf in file:iter() do
				for f in files:iter() do
					f = T.path.join(root, f)
				
					if f:endswith(tf) then
						if foundFiles[tf] then
							foundFiles[tf]:append(f)
						else
							foundFiles[tf] = T.List{f}
						end

						return f
					end
				end
			end
		end
	end

	return "false";
end

--- Find a library.
--
-- @tparam string lib The base name of the library.
-- @return The path to the file or `false` if it was not found.
function S.findSharedLibrary ( lib )
	if type(lib) ~= "table" then
		lib = {tostring(lib)}
	end
	T.utils.assert_arg(1, lib, "table")
	lib = T.List(lib)

	local join = T.path.join
	local isfile = T.path.isfile

	local dirs = T.List()
	local templates = T.List()

	if S.os.compliance == "posix" then
		templates:append "lib%s.so"

		dirs:append "/usr/lib/"
		dirs:append "/usr/local/lib/"
		dirs:append "/lib/"
	end

	local names = T.List()
	local orig  = T.List()
	for l in lib:iter() do
		names:extend(templates:map():format(l))
		orig:extend(templates:map():gsub("^.*$", l)) -- Fill it with the name.
	end

	local l = S.findFile(names, dirs)
	if l then
		return l, orig[names:index(T.path.basename(l))]
	end

	return false;
end

--- Add a Target to the Install Target
--
-- Installs `path`.  If `path` is a directory it will be installed
-- recursivly.  Please note that the directory will be installed, not it's
-- contents.  Example: `S.install("foo/", "bar/")` will result in `bar/foo/`,
-- not `bar/{foo_contents}`.
--
-- @tparam string path The path of the target.
-- @tparam string to The directory to install the file.  This is treated as
-- relative to the install prefix (`S.prefix`) if it is not absolute.
-- @tparam number perm The unix permissions (see S.util.install)
-- @treturn {string,...} A List of files that will be installed.
function S.install ( path, to, perm )
	T.utils.assert_string(1, path)
	T.utils.assert_string(2, to)

	S.import "util"

	local apath = C.path(path)
	local bname = T.path.basename(apath)
	to = T.path.join(T.path.join(S.prefix, to or ""), bname)

	local installed = T.List()

	if T.path.isdir(apath) then
		for f in T.List(T.dir.getallfiles(path)):iter() do
			local fto = T.path.join(to, f)
			S.util.install(f, fto, perm)
			C.addDependancy("install", fto, { magic = true })
			installed:append(fto)
		end
	else
		S.util.install(apath, to, perm)
		C.addDependancy("install", to, { magic = true })
		installed:append(to)
	end

	return installed
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
		S.path = T.List(ospath:split(seperator))
	end

	if S.style == "win32" then
		S.pathext =  T.List(os.getenv("PATH"):split(";"))
	end
end

