
--- global Clicfg => clicfg.lua

--- Ensure
local module = {
    RenderEnsure = function ()
        print("lua: render ensure called")
        print("Type of Notify is:")
    end,

    UioEnsure = function ()
        print("lua: UioEnsure called")
    end
    
}

return module