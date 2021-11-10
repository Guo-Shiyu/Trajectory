
function Check(tablename)
	if _G[tablename] ~= nil then 
		local count = 0;
		for key, value in pairs(_G[tablename]) do
			print(" - key:"..key.." \t\t type(value): "..type(value))
			count = count + 1
		end
		print(" >> sum of "..tablename.." is :"..count.."\n")
	end
end

function CheckAll(...)
	local table = {...}
	for _, value in pairs(table) do 
		Check(value)
	end
end

--- sketcher.ensure()
function SketcherEnsure()
	print("-- check info from lua:\n")
	CheckAll("Color", "Ani", "Object", "Set", "Act", "Render", "Schedule")
	print("-- check end")
	return true;
end

--- cache.ensure()
function CacheEnsure()
	print("cache ensure called");
end