
--- Config
local module = 
{
    Client = 
    {
        ResourcePath = "D:\\GitBranch\\GitHub\\Trajectory\\resource",
        SelfDescriptor = 
        {
            --Uid   =   "nil",
            Name  =   "h123言",
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
        LoginServerAddr = "172.31.162.129", 
        TargetPort  = 24680,
        
    },
}

return module