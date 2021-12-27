
--- Config
local module = 
{
    Client = 
    {
        ResourcePath = "D:\\GitBranch\\GitHub\\Trajectory\\resource",
        SelfDescriptor = 
        {
            Name    =   "h123言",
            Weapon  =   "C",
            Style   =
            {
                FaceColor   =  321,
                HelmetColor =  321,
                FigureColor =  321
            }
        },
    },
    
    Render = 
    {
        MainScreen = 
        {
            Hight = 952,
            Width = 1540,
            Fps   = 100,
        },
        
        StartWithLog = true,
    },
    
    NetIO = 
    {
        LoginServerAddr = "172.21.66.215", 
        TargetPort  = 24680,
        
        BattlePort = 24676,
    },
}

return module