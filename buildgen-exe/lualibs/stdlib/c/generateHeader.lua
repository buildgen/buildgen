#! /usr/bin/env lua

require "lfs"
require "pl"
stringx.import()

headerName = arg[1]
headerDontIncludeName = headerName:upper():gsub("[^%w]", "_")
srcName = arg[2]

header = assert(io.open(headerName, "w"))
src    = assert(io.open(srcName, "w"))

header:write([[
/***** Generated by BuildGen S.c header generator *****/
#ifndef ]] .. headerDontIncludeName .. [[

#define ]] .. headerDontIncludeName .. [[


]])
src:write([[
/***** Generated by BuildGen S.c header generator *****/


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

	header:write   ("extern const char *"..key..';\n')
	src:write("const char *"..key..' = "'..value..'";\n')
end

header:write([[

#endif /* ]] .. headerDontIncludeName .. [[ */
]])
