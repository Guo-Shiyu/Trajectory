
--- Render Ani Candidate
local module = {};

--- @arg list: table of rooms, 
--- @arg count: rooms count 
module.DisplayRoomList = function (list, count)
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
end

--- an example
module.CircleExpand = function (x, y, color, rbase, rfinal, rate)
    --- args config
    local color = color or Color.Blue
    local rbase, rfinal = rbase or 30, rfinal or 150
    local rate = rate or 2    -- expand rate
    coroutine.yield()   -- first call to store args

    --- draw acts
    for r = rbase, rfinal, rate do
        Set.SetLineColor(color)
        Act.Circle(x, y, r)
        coroutine.yield()      
    end
end

module.OpenAnimation = function (x, y, color, rbase, rfinal, rate)
    --- args config
    local x, y = x or math.random(1500), y or math.random(900)
    local color = color or Color.White
    local rbase, rfinal = rbase or 30, rfinal or 900
    local rate = rate or 2    -- expand rate
    coroutine.yield()   -- first call to store args

    -- --- draw acts
    -- for r = rbase, rfinal, rate do
    --     Set.SetLineColor(color)
    --     Act.Circle(x, y, r)
    --     coroutine.yield() 
    -- end
end

return module
