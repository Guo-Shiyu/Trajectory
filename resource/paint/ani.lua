
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

local function Round(x, y, rin, width)
    Act.Circle(x, y, rin) -- outer circle 
    if width > 2 then 
        Act.FillCircle(x, y, rin - width) -- inner circle 
    end 
end

module.OpenAnimation = function ()
    -- some helper func 
    local coroRound = function (x, y)
        local x, y = x, y
        local baseRad, baseWid= 10, 50
        coroutine.yield()

        for i = 1, 100, 1 do
            Round(x, y, baseRad, baseWid)
            if i % 3 == 0 then
                baseWid = baseWid - 2
                baseRad = baseRad + 3
            end
            coroutine.yield()
        end
    end
    
    
    local coroArray = {}
    coroutine.yield()
    
    for frame = 1, 500, 1 do
        if frame % 15 == 0 then
            local act = coroutine.create(coroRound)
            coroutine.resume(act, math.random(1500), math.random(900))
            table.insert(coroArray, act)
        end

        for index, value in ipairs(coroArray) do
            if coroutine.status(value) ~= "dead" then
                coroutine.resume(value)
            end
        end
        coroutine.yield()
    end
end

module.IntoSignIn = function ()
    coroutine.yield()
    for i = 1, 300, 1 do
        Set.SetTextStyle(100, 70, "Terminal")
        Act.Xyout(300, 430, "Trajectory")
        coroutine.yield()
    end
end

return module
