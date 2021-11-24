--- Render Object
local module = {}
-- module.RandomCircle = function ()
--     local x, y, final, time = math.random(100, 1400), math.random(100, 800), math.random(70, 200), math.random(150, 300)
--     local rbase, step, color = 20, 2, RGB(math.random(0, 255), math.random(0, 255), math.random(0, 255))
--     if final > 130 then
--         step = step + 1
--     end
--     coroutine.yield()
    
--     local r = rbase
--     for frame = 1, time, 1 do
--         Set.SetLineColor(color)
--         Act.Circle(x, y, r)
--         r = r + step
--         coroutine.yield()
--     end
-- end

module.IntoSignIn = function ()
    coroutine.yield()

    for i = 1, 300, 1 do
        Set.SetLineColor(0xFFFFFF)
        Set.SetTextStyle(100, 70, "Terminal")
        Act.Xyout(290, 290, "Made in C++ & Lua")
        Set.SetTextStyle(20, 14, "Terminal")
        Act.Xyout(950, 390, "(C)Copyleft Sliver")
       
        coroutine.yield()
    end
end

return module