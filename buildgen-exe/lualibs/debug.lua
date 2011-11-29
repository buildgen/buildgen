dofile(_s_lualibs_root.."debug/dumper.lua")
function dump ( v )
	print(DataDumper(v, ""))
end
