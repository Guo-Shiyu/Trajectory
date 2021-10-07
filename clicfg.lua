
--- Config
local module = 
{
    Client = {
        ResourcePath = "D:\\GitBranch\\GitHub\\Trajectory\\resource",
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
        LoginServerAddr = "172.81.247.7", 
        TargetPort  = 24680,
        
        Protocal = 
        {
            Beat = [[{"Type":"Beat"}]],
        }
    },

    UserIO = 
    {

    }
}

print("Client config load success")

return module