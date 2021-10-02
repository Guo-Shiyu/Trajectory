
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
        SelfPort  = 24680,
        TargetHost  = "172.0.0.1", 
        Init = 
        {
            NetCheckSite = "www.baidu.com",
            MaxDisConnectTime = 1000,
        }
    },

    UserIO = 
    {

    }
}

print("Client config load success")

return module