print("-- check info from lua:\n")

function Check(tablename)
	local count = 0;
	for key, value in pairs(_G[tablename]) do
		print(" - key:"..key.." \t type(value): "..type(value))
		count = count + 1
	end
	print(" >> sum of "..tablename.." is :"..count.."\n")
end

function CheckAll(...)
	local table = {...}
	for _, value in pairs(table) do 
		Check(value)
	end
end

CheckAll("Color", "Ani", "Object", "Set", "Act", "Render")

print("-- check end")