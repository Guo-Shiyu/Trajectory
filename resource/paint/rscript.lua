
--- render
local module = {

    --- fn for sketchers 
    StepRender = function ()
        if #TaskQue > 0 then 
            for k, v in pairs(TaskQue) do
                coroutine.resume(v)
                if coroutine.status(v) == "dead" then 
                    table.remove(TaskQue, k)
                end
            end
        end
    end,
    
    ClearRender = function ()
        for k, _ in pairs(TaskQue) do
            TaskQue[k] = nil 
        end
    end,

    ShowLog = function ()
        local LogArgs = {
            Hight   = 16,
            Width   = 9,
            Font    = "Termianl",
            GetLog  = function (id)
                if id == ThreadId.U then
                    local info = LogTable[id] or ""
                    local len = string.len(info)
                    if len > 10 then
                        info = string.sub(info, len - 10, len)
                    end
                    return info
                else
                    return LogTable[id] or "(no log)"  
                end
            end
        }

        Set.SetTextStyle(LogArgs.Hight, LogArgs.Width, LogArgs.Font)
        Act.Xyout(0,        0,                      "Cli:"..LogArgs.GetLog(ThreadId.C))
        Act.Xyout(1400,     0,                      "Key:"..LogArgs.GetLog(ThreadId.U))
        Act.Xyout(0,        900 - LogArgs.Hight,    "Net:"..LogArgs.GetLog(ThreadId.N))
    end,

    ClearTask = function ()
        print("lua: ClearTask called")
    end,

    ClearObject = function ()
        print("lua: ClearObject called")
    end,

    UploadLog = function (logs)
        for _, v in pairs(ThreadId) do 
            if logs[v] ~= nil then 
                LogTable[v] = logs[v]
            end
        end
        print("lua: UploadLog called")
    end,

    UpdateTask = function (queue)
        for k, v in ipairs(queue) do 
            print("Key: "..tostring(k).." Value: "..type(v));
            table.insert(TaskQue, v)
            --table.remove(queue, k) -- clear taskque in cache by cpp
        end
        print("lua: UpdateTask called")
        print("Cache's TaskQue len: "..#queue)
    end,

    --- fn for cache
    AddRenderTask = function (index, ...)
        print("lua: index: "..index)
        local task = coroutine.create(Ani[index])
        local result, why = coroutine.resume(task, ...)
        print("lua: AddRenderTask Called result: "..tostring(result).." why: "..tostring(why))
        table.insert(TaskCache, task)
    end,

    AddRenderLog = function (id, logstr)
        LogCache[id] = logstr
        print("lua: AddRenderLog called")
    end
}

return module