local module = {} 

module.SkillBoard = function ()
    coroutine.yield() 

    local skills = {"1. someskill", "2. otherskill", "3. anotherskiill"}
    while true do 
        Set.SetLineColor(0xFFFFFF)
        Set.SetTextStyle(30, 0, "Terminal")
        for i = 1, #skills, 1 do
            Act.Xyout(200 + (i - 1) * 100, 700, skills[i])
        end
        coroutine.yield()
    end 
end

return module