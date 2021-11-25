local module = 
{
    MainMenu = function ()
        coroutine.yield()

        while true do  -- show every 0.2s 
            Set.SetLineColor(0xFFFFFF)
            Set.SetTextStyle(100, 70, "Terminal")
            Act.Xyout(500, 100, "Main Menu")
            Set.SetTextStyle(20, 14, "Terminal")
            Act.Xyout(625, 250, "enter space to start")
            Act.Xyout(345, 400, "8fd95c31662f6211778e625351fa676576844e1c897fff0c20600e4e484f1a88ab4f605f53771f")

            coroutine.yield()
        end
    end,

    DebugShow = function (str)
        local debuginfo = str 
        coroutine.yield()
    
        while true do
            Set.SetTextStyle(30, 20, "Terminal")
            Act.Xyout(0, 30, debuginfo)
            coroutine.yield()
        end
    end,

    --- @arg count: rooms count 
    --- @arg list: table of rooms, 
    DisplayRoomList = function (count, list)
        local list, count = list, count 
        coroutine.yield()

        while true do
            Set.SetLineColor(0xFFFFFF)  -- white 
            Set.SetTextStyle(20, 14, "Terminal")
            if count == 0 then
                Act.Xyout(580, 250, "( empty room list in server )")
            else 
                local count = 0
                for _, value in pairs(list) do
                    local str = ""
                    str = str..tostring(value).."\t"  
                    Act.Xyout(625, 250 + 30 * count, str)            
                end
            end
            coroutine.yield()
        end
    end, 

}

return module 