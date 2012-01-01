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

--- Add A Generator
--
-- @param outputs A list of files that are generated by executing the command.
--	They are treated as BuildGen paths.
-- @param inputs A list of files that the outputs depend on.  They are treated
--	as BuildGen paths.
-- @param command A list of arguments.  The first item in the list (arg 0) is
--	treated as a BuildGen path.
-- @param options A table of options.  The following values are currently
--	supported.
--	<ul><li>
--		description: A description of what the command is doing.
-- </li><ul>
--
-- @class function
-- @name C.addGenerator

--- Add A Dependancy
--
-- Add dependency to target.  This ensures that dependancy exist before target
-- is build and that targert is rebuilt when target is updated.
--
-- @param target A list of files that are generated by executing the command.
--	They are treated as BuildGen paths.
-- @param dependency A list of files that the outputs depend on.  They are treated
--	as BuildGen paths.
-- @param options A table of options.  The following values are currently
--	supported.
--	<ul><li>
--		magic: If true target will be treated as a
--		<a href="https://gitorious.org/buildgen/pages/UserDocs_Targets">magic
--		target</a>.
--	</li><li>
--		magicsrc: If true dependency will be treated as a
--		<a href="https://gitorious.org/buildgen/pages/UserDocs_Targets">magic
--		target</a>.
-- </li><ul>
--
-- @class function
-- @name C.addDependancy

--- Add A Directory
--
-- Add dir to the build.
--
-- @param dir A BuildGen path.
--
-- @class function
-- @name C.addDir

--- Resolve a BuildGen path.
--
-- Returns the absolute path refrenced by the BuildGen path path.
--
-- @param path A BuildGen path.
-- @return A (dumb) normalized absolute pathname.  (Symlinks are not resolved)
--
-- @class function
-- @name C.path

--- Resolve a path passed as a define.
--
-- Replaces the value in the D prefix with an abosolute path.  The path is
-- treated as relitive to the directory form which BuildGen was called.  This
-- should be used whenever trating a value defined on the command line as a
-- path.  If this is not used relitave paths will not be relitive to the
-- direcotory the caller expects.
--
-- @param path The key of the defined value.
-- @param default The value to be put if the value is not defined.  Defaults to
--	nil.
function D.resolvePath ( path , default )
	if not D[path] then
		D[path] = default
	end

	if not D[path] then return end -- If the default was nil.

	if D[path]:find("/", 1, true) > 1 then -- Doesn't start with a slash.
		D[path] = C.path(">"..D[path])
	end
end

dofile(S.lualibsRoot.."stdlib.lua")
