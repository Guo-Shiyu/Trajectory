
--- render
local module = {
    AddRenderTask = function (index, ...)
        local task = coroutine.create(Ani[index])
        coroutine.resume(task, ...)
        table.insert(TaskQue, task)
    end,
    
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
        LogArgs = {
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
        Act.Xyout(1440,     0,                      "Key:"..LogArgs.GetLog(ThreadId.U))
        Act.Xyout(0,        900 - LogArgs.Hight,    "Net:"..LogArgs.GetLog(ThreadId.N))
    end,

    ClearTask = function ()
        print("lua: ClearTask called")
    end,

    ClearObject = function ()
        print("lua: ClearObject called")
    end,

    UploadLog = function ()
        print("lua: UploadTask called")
    end,

    UpdateTask = function ()
        print("lua: UpdateTask called")
    end
}

return module