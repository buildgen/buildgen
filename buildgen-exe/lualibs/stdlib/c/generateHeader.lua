#! /usr/bin/env lua

-- Copyright 2011-2012 Kevin Cox

--[[---------------------------------------------------------------------------]
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

require "lfs"
require "pl"
stringx.import()

headerName = arg[1]
headerDontIncludeName = headerName:upper():gsub("[^%w]", "_")
srcName = arg[2]

dir.makepath(path.dirname(headerName))
dir.makepath(path.dirname(srcName))

header = assert(io.open(headerName, "w"))
src    = assert(io.open(srcName, "w"))

header:write([[
/***** Generated by BuildGen S.c.generateHeader *****/
#ifndef ]] .. headerDontIncludeName .. [[

#define ]] .. headerDontIncludeName .. [[


]])
src:write([[
/***** Generated by BuildGen S.c.generateHeader *****/


]])

local i = 2
while #arg > i do
	i = i+1

	local arg = arg[i]
	local key, value

	local b = arg:find("=") -- Find the end of the key.
	key, value = arg:sub(0, b-1), arg:sub(b+1) -- Split into the key and value

	if not key:match("^[%a_][%w_]*$") then
		error('S.c.generateHeader given an invalid key "'..key..'"')
	end

	value = value:gsub('.', {
	                          ["\\"] = "\\".."\\",
	                          ['"' ] = '\\"',
	                          ["\n"] = "\\n"
	                        })

	header:write("extern const char *"..key..';\n')
	src:write("const char *"..key..' = "'..value..'";\n')
end

header:write([[

#endif /* ]] .. headerDontIncludeName .. [[ */
]])
