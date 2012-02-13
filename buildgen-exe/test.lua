#! /usr/bin/env lua

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
