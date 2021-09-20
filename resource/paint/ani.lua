--- Render Ani Candidate
local module = {

    --- an example
    ["CircleExpand"] = function (x, y, color, rbase, rfinal, rate)
        
        --- args config
        local color = color or Color.Blue
        local rbase, rfinal = rbase or 30, rfinal or 150
        local rate = rate or 2    -- expand pixel
        coroutine.yield()   -- first call to store args

        --- draw acts
        for r = rbase, rfinal, rate do
            Set.SetLineColor(color)
            Act.Circle(x, y, r)
            coroutine.yield()      
        end
    end,

    ["OpenAnimation"] = function (x, y, color, rbase, rfinal, rate)
        
        --- args config
        local color = color or Color.White
        local rbase, rfinal = rbase or 30, rfinal or 150
        local rate = rate or 2    -- expand pixel
        coroutine.yield()   -- first call to store args

        --- draw acts
        for r = rbase, rfinal, rate do
            Set.SetLineColor(color)
            Act.Circle(x, y, r)
            coroutine.yield()      
        end
    end,
}

return module
