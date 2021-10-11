
--- Config
local module = 
{
    Client = {
        ResourcePath = "D:\\GitBranch\\GitHub\\Trajectory\\resource",
        SelfDescriptor = {
            Uid   =   "nil",
            Name  =   "hiahiahia",
            Player    =
            {
                Weapon    =   1,
                Style     =
                {
                    FaceColor =  321,
                    HelmetColor   =  321,
                    FigureColor   =   321
                }
            }
        },
    },
    
    Render = 
    {
        MainScreen = 
        {
            Hight = 900,
            Width = 1500,
            Fps   = 100,
        },
        
        StartWithLog = true,
    },
    
    NetIO = 
    {
        LoginServerAddr = "172.30.35.108", 
        TargetPort  = 24680,
        
        
        Protocal = 
        {
            Hello   = [[{"Type":"Hello"}]],
            Beat    = [[{"Type":"Beat"}]],
        }
    },

    UserIO = 
    {
           KeySet = "Default",
    }
}

-- 序列化tablle表--將表转化成string
function Serialize(obj)
    local lua = ""
    local t = type(obj)
    if t == "number" then
        lua = lua .. obj
    elseif t == "boolean" then
        lua = lua .. tostring(obj)
    elseif t == "string" then
        lua = lua .. string.format("%q", obj)
    elseif t == "table" then
        lua = lua .. "{\n\t"
        for k, v in pairs(obj) do
            lua = lua .. "[" .. Serialize(k) .. "]=" .. Serialize(v) .. ",\n"
        end
        local metatable = getmetatable(obj)
        if metatable ~= nil and type(metatable.__index) == "table" then
            for k, v in pairs(metatable.__index) do
                lua = lua .. "[" .. Serialize(k) .. "]=" .. Serialize(v) .. ",\n"
            end
        end
        lua = lua .. "}"
    elseif t == "nil" then
        return nil
    else
        return "-nil-" 
        --error("can not Serialize a " .. t .. " type.")
    end
    return lua
end

function ConfigDump()
    print(" Config Dump: ")
    -- print(Serialize(module))
end

return module