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
// Auto generated test

#define DEBUG
]]

for f in dir.getfiles(src, "*.hpp"):iter() do
	out:write('#include "'..f..'"\n')
end

out:write([[

int main ( int argc, char **argv )
{
	files = new Files("]]..path.abspath(src)..[[", argv[0]);

	///// DO THE TESTS

]])

for f in dir.getfiles(src, "*.cpp"):iter() do
	local src = io.open(f)
	for l in src:lines() do
		local name = l:match"void[%s]*(_TEST_[%a%d_]*)[%s]%([%s]void[%s]%)"
		if name then
			out:write("	"..name.."();\n")
		end
	end
end

out:write[[

	return 0;
}
]]
