Task = {
	["CircleExplode"] = function (x, y, r)
		local _x, _y, _r = x, y, r
		local expand = 3;
		coroutine.yield()	-- first call
		for i=1, 150 do 
			Circle(_x, _y, _r + i * expand)	
			coroutine.yield()
		end
		return true
	end,
}

function AddRenderTask(index, x, y, r)
	print("add render task success, index:"..index.."|"..x.."|"..y.."|"..r)
	local co = coroutine.create(Task[index])
	TaskQue[#TaskQue + 1] = co 
	coroutine.resume(TaskQue[#TaskQue], x, y, r)
end

function StartRender()
	for k, v in pairs(TaskQue) do 
		coroutine.resume(v)
		if coroutine.status(v) == "dead" then 
			TaskQue[k] = nil 
			print("TaskQue:"..tostring(k).." has been desert, current queue size:"..#TaskQue)
		end
	end
end

function increase(x)
	print("increase has been called, result = ".. (x + 1))
end

function append_suffix(x, i)
	print("append_suffix has been called, result = ".. (x.."|"..tostring(i)))
end