dofile(S.lualibsRoot.."debug/dumper.lua")
function dump ( v )
	print(DataDumper(v, ""))
end
