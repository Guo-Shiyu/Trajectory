
--- Render Ani Candidate
local module = {};

module.DebugShow = function (str)
    local debuginfo = str 
    coroutine.yield()

    for frame = 1, 100, 1 do
        Set.SetTextStyle(30, 20, "Terminal")
        Act.Xyout(0, 30, debuginfo)
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

    --- draw acts
    for r = rbase, rfinal, rate do
        Set.SetLineColor(color)
        Act.Circle(x, y, r)
        coroutine.yield() 
    end
end

module.RandomCircle = function ()
    local x, y, final, time = math.random(100, 1400), math.random(100, 800), math.random(70, 200), math.random(150, 300)
    local rbase, step, color = 20, 2, RGB(math.random(0, 255), math.random(0, 255), math.random(0, 255))
    if final > 130 then
        step = step + 1
    end
    coroutine.yield()
    
    local r = rbase
    for frame = 1, time, 1 do
        Set.SetLineColor(color)
        Act.Circle(x, y, r)
        r = r + step
        coroutine.yield()
    end
end

module.IntoSignIn = function ()
    coroutine.yield()
    for i = 1, 300, 1 do
        Set.SetLineColor(0xFFFFFF)
        Set.SetTextStyle(100, 70, "Terminal")
        Act.Xyout(290, 290, "Made in C++ & Lua")
        Set.SetTextStyle(20, 14, "Terminal")
        Act.Xyout(950, 390, "(C)Copyleft Sliver")
        if i % 21 == 0 and i < 200 then
            Schedule.Submit("RandomCircle")
        end 
        coroutine.yield()
    end
end

module.MainMenu = function ()
    coroutine.yield()

    while true do
        Set.SetLineColor(0xFFFFFF)
        Set.SetTextStyle(100, 70, "Terminal")
        Act.Xyout(500, 100, "Main Menu")
        Set.SetTextStyle(20, 14, "Terminal")
        Act.Xyout(625, 250, "enter space to start")

        Act.Xyout(345, 400, "8fd95c31662f6211778e625351fa676576844e1c897fff0c20600e4e484f1a88ab4f605f53771f")

        coroutine.yield()
    end
end

return module
