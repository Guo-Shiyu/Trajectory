
--- Render Ani Candidate
local module = {};

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

module.Recounter = function (second)
    local sec = second 
    coroutine.yield()

    local toshow = sec
    for i = 1, 100 * sec, 1 do
        Set.SetLineColor(0xFFFFFF)
        Set.SetTextStyle(50, 50, "Terminal")
        Act.Xyout(200, 200, tostring(toshow))
        if i % 100 == 0  then
            toshow  = toshow - 1
        end
        coroutine.yield()
    end
end

module.IsMyRound = function()
    coroutine.yield()

    for i = 1, 100, 1 do
        Set.SetLineColor(0xFFFFFF)
        Set.SetTextStyle(100, 100, "Terminal")
        Act.Xyout(300, 300, "It is your turn !!!")
        coroutine.yield()
    end
end

return module
