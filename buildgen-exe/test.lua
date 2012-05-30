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

require "pl"
stringx.import()

if not arg[1] then
	print "No input directory"
	return 3
end
local src = arg[1]

if not arg[2] then
	print "No output file"
	return 3
end
local out = io.open(arg[2], "w")

out:write[[
// Auto generated tests

#define DEBUG
#define TEST

]]

for f in dir.getfiles(src, "*.hpp"):iter() do
	out:write('#include "'..f..'"\n')
end

out:write([[

int main ( int argc, char **argv )
{
	Files *files = new Files("]]..path.abspath(src)..[[", argv[0]);

	///// DO THE TESTS

]])

argTypes = {
	Files = "files",
}

for f in dir.getfiles(src, "*.cpp"):iter() do
	local src = io.open(f)
	for l in src:lines() do
		local name, args = l:match"void[%s]*(_TEST_[%a%d_]*)[%s]%(([^%)]*)%)"
		if name then
			local cargs = ""

			if not args:match "[%s]*void[%s]*" then
				for s in args:gmatch("[^,]*") do
					if s == "" then break end

					local type = s:match"^[%s]*([A-Za-z_][A-Za-z0-9_]*)[%s]*%*[%s]*[A-Za-z_][A-Z0-9a-z_]*"

					local var = argTypes[type]
					if not type then
						print("Type '"..s.."' unknown in void '"..name.." ("..args..")'")
						os.exit(1)
					else
						cargs = (cargs ~= "" and cargs..", " or "")..var
					end
				end
			end

			out:write("	"..name.."("..cargs..");\n")
		end
	end
end

out:write[[

	return 0;
}
]]
