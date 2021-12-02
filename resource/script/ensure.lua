--- Ensure
local module = {
    RenderEnsure = function ()
        print("lua: render ensure called")
        
        local logger = function (name, table)
            print("", name.." resources:", type(table))
            for key, value in pairs(table) do
                print(string.format("\t - %20s : %s", tostring(key), tostring(value)))
            end
            print("")
        end

        logger("Animation", Ani)
        logger("Object", Object)
        logger("UI", Ui)
        logger("Menu", Menu)

        print("lua: render ensure called")

    end,

    UioEnsure = function ()
        print("lua: UioEnsure called")
    end
}

return module