# Trajectory 弹道轨迹
 2019 SCU-CS 研究与开发实战课程作业   
 一个模仿 __4399弹弹堂__ 与 __TNT弹道轨迹__ 的2D游戏, 使用 `EasyX` 与 `Lua`  完成  
 除了以上两个, 还要感谢以下开源库的作者, 没有他们, 这个游戏只会存在于想象里
 - [json](https://github.com/nlohmann/json) : JSON for modern C++
 - [libhv](https://github.com/ithewei/libhv) : event-loop with non-blocking IO and timer
 - [sol2](https://github.com/ThePhD/sol2) : C++ library binding for Lua
 # 进度记录(~~交作业用~~)
 20-08-30: 创建项目  
 20-09-01: 开发环境配置完成, 外部依赖可以正常使用  
 20-09-09: 类结构基本搭建完成, 开始实现逻辑

 # 目录结构
 ~~~
    Trajectory
        |- BattleServer     VS2019 Project
        |- Client           VS2019 Project
        |- header           
            |- battle.h         战斗服务器头文件
            |- client.h         客户端头文件
            |- fsmdef.h         FSM模板定义及状态机生成宏
            |- client
                |- character.h      (GameInfo)定义
                |- interface.h      接口定义
                |- msgdispatch.h    消息分派器, 日志定义
                |- state.h          特化状态机定义
                |- thread.h         R, N, U 工作线程定义
            |- ...              第三方库头文件夹
        |- src
            |- battle           战斗服务器
                |- main.cpp
                |- 
            |- client           客户端
                |- main.cpp 
                |- ...          对应 header / client 下头文件
        |- lib              第三方库依赖
            |- ...
        |- resorce
            |- paint            动画
                |- 
            |- sound            音效
                |-
            |- script           脚本文件
                |-
        |- sercfg.lua       战斗服务器配置文件
        |- clicfg.json       客户端配置文件
                
 ~~~

 # 开发环境搭建
 依次按照如下步骤安装， 默认使用 VS2019 
 - 使用官网自动安装程序安装 EazyX  
 [EazyX 官网地址](https://easyx.cn/)  
 [EazyX 文档地址](https://docs.easyx.cn/zh-cn/intro)   
 - 下载 Lua 源代码, 并放置到 VS2019 的 `include` 目录下  
 [Lua](https://github.com/lua/lua)  
 例如, 我的目录结构是

 ~~~
    ..\Microsoft Visual Studio\2019\Community\VC\Auxiliary\VS\include  
                                                                |- easyx.h
                                                                |- graphics.h                
                                                                |- lua.h
                                                                |- luaxlib.h
                                                                ...
 ~~~  
 
 达到这样的最终效果即可  ~~总不会这也需要教吧~~  
 在任何 cpp 文件中, 这样的语句
 ~~~
    #include <lua.h>
 ~~~
 
 或是
 
 ~~~
    #include <easyx.h>
 ~~~   
 将不会报错  
 - 从 github 上 clone 该项目到本地  
 使用 `.sln` 文件打开解决方案, 确保 `Client` 与 `BattleServer` 两个项目中的 `项目->属性` 页中  
 `c/c++ -> 预处理器 -> 输入` 中存在 `HV_STATICLIB` 宏 (引入第三方库)  
 `c/c++ -> 链接器 -> 输入 -> 附加依赖项` 中包含了 `Trajectory\lib` 目录下的所有 `lib` 文件  
 ~~`c/c++ -> 链接器 -> 输入 -> 忽略指定默认库`  中包含了 `msvcrt.lib`~~

到这里, 开发环境搭建完成, 编译运行测试程序来测试第三方库能否正常使用

# Architect 
## 组成 
 由三个部分组成
 - Client.exe  客户端    
 部署在玩家电脑上, 采集用户输入和绘制画面 ~~主要用来交作业~~
 - BattleServer.exe 战斗服务器  
 ~~因为云服务器负载快满了~~ 部署在玩家电脑上, 单独运行, 负责战斗房间中的弹道/伤害计算   
 - LoginServer.a 登陆服务器  
 ~~不得不~~ 部署在服务器上, 用于游戏大厅中玩家的登录/加入房间, 以及分配战斗服务器

 ## 通信
 服务器与客户端间的通信格式均为 `JSON`  
 各个字段定义参见 `PROTO.md`

 # 实现思路  
 参见 `inspiration.md`
 
 ## 客户端
 客户端需要三个线程
 - 通信线程 N (NetIO)
 - 操作线程 U (UserIO)
 - 绘制线程 R (Render)
 
 ### 绘制线程 R (文中的 `渲染` 也表达绘制图像的意思)  
  
 