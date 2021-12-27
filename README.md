# Trajectory 弹道轨迹
 2019 SCU-CS 研究与开发实战课程作业   
 一个模仿 __4399弹弹堂__ 与 __TNT弹道轨迹__ 的2D游戏, 使用 `EasyX` 与 `Lua`  完成  
 除了以上两个, 还要感谢以下开源库的作者, 没有他们, 这个游戏只会存在于想象里
 - [json](https://github.com/nlohmann/json) : JSON for modern C++
 - [libhv](https://github.com/ithewei/libhv) : event-loop with non-blocking IO and timer 
 - [sol2](https://github.com/ThePhD/sol2) : C++ binding for Lua
 
 # 进度记录  
 交作业用  
 20-08-30:   
 + 立项

 20-09-01:   
 + 开发环境配置完成, 外部依赖可以正常使用  
 
 20-09-09:   
 + 类结构基本搭建完成 
 + 确定客户端三线程工作模型
 + 草案，客户端各个线程的状态机
 
 20-09-13:  
 + 测试，状态机实现方式, 同时实现了部分状态机逻辑
 + 测试，在脚本层实现绘图动画
 + 完成，协程绘图单元测试 
 + 完成，封装了日志模块
 + 草案，线程间通信接口及参数传递方式
 + 草案，配置文件与脚本加载机制
 + 草案，地图文件脚本

 20-09-29:    
 + 重构，从类中进一步抽象出接口统一行为，简洁了代码
 + 重写，完善了线程间通信接口，以一种侵入式的方式（？）
 + 完成，基于 lua 协程的生成器 的绘图方式
 + 完成，渲染线程即时日志上线 
 + 完成，配置文件与脚本加载机制
 + 草案，client-server 通信协议草案 
 + 草案，武器脚本
 + 草案，新版地图脚本
 + 草案，战斗服务器物理引擎

 20-10-07
 + 重构，渲染线程即时日志现在是基于线程间通信接口完成的, 接口更加简洁清晰了
 + 完善，渲染线程工作逻辑，现在涉及 data race 的代码都被封装在了类内部，解决了渲染线程对 lua 虚拟机争用频繁加锁的问题（虽然不影响绘图性能）
 + 完善，渲染线程 userio，netio 即时日志部分
 + 完善，状态机的逻辑
 + 调整，配置文件的内容
 + 调整，部分 userio 线程的逻辑
 + 草案，登录服务器架构

 20-10-09  
 + 测试， 客户端与服务端通信

 ... 由于课程设计太多， 后续更新略了， 最终提交的课程报告位于 doc/README.md 中

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
        |- sercfg.lua        战斗服务器配置文件
        |- clicfg.json       客户端配置文件
 ~~~


# Architect 
## 组成 
 由三个部分组成
 - Client.exe  客户端    
 部署在玩家电脑上, 采集用户输入和绘制画面
 - BattleServer.exe 战斗服务器  
 ~~因为云服务器负载快满了~~ 部署在玩家电脑上 (windows), 单独运行, 负责战斗房间中的弹道/伤害计算, 实现物理效果
 - LoginServer.a 登陆服务器  
 ~~不得不~~ 部署在服务器上, 用于游戏大厅中玩家的登录/加入房间, 以及分配战斗服务器

 ## 通信
 服务器与客户端间的通信格式均为 `JSON`  
 各个字段定义参见 `PROTO.md`

 

 ## 客户端服务端公用部分  
 包括 状态机 (state.h) 日志 (log.h) 两部分  
 定义在 `header` 目录下对应的 .h 文件中  
 
 ## 状态机 state machine
 ~~模板加宏威力无穷~~  
 状态机定义分三部分: 
 - StateBase 状态模板类, 类似接口  
 模板参数 class C : 表明这是 class C 的一个可能状态.  
 提供三个必须实现的接口   
 ~~~ C++
    virtual void into(C* )  = 0;    //进入状态时
    virtual void on(C* )    = 0;    //处于状态中
    virtual void off(C* )   = 0;    //离开状态时
 ~~~

 - StateMachine 状态机模板类
 模板参数 class T : 表明这个类拥有一个状态机, 及多个可能的状态  
 内部包含有 指向 T 实例的指针, 记录当前状态及之前状态的指针(用于状态翻转), 以及指向全局状态的指针(暂时没用到).  
 每个对象用于实现功能的 public 成员函数不直接调用, 在状态机的 into / on / off 三个函数中调用, 能够做到状态驱动, 使代码逻辑清晰, 便于维护.    
 配合客户端中消息分发机制 (Message Dispatch), 能够实现状态的灵活改变以及事件驱动.

 - 状态机生成宏
 填写宏参数自动生成状态, 减少重复代码. 但由于预处理器只是单趟编译, 这部分代码并不优雅.  
 主要用法参见 header/client/state.h  
 ~~C/C++的宏太弱了, 想要过程宏~~

 ## 日志 log
 日志定义在 header/log.h 的 namespace Logger 下, 分为三部分:
 - 日志记录 (logger)
 暂时采用有限长度的队列 (std::queue\<std::string\>) 来记录最后生成的数条记录, 可以通过配置文件更改, 默认为 128.  
 现在看来已经能够满足生产需求, 没有必要更换其他更加复杂的日志策略.
 
 - 日志生成 (log generator)
 通过 std::format/std::source_location (感谢C++20), 可变参数宏, 变参模板 实现日志生成, 暂时不支持日志格式通过配置文件定义. 其默认格式如下:
 ~~~ C++
    //固定生成的部分
    "-- {:2}:{:2}:{:2}:{:4}\t-{:13} :{:3} :{:15}:"    
        hour, min, sec, msec, 文件名, 行号, 函数名 
    //随后会换行并且缩进一个指标符, 在info:后输出自由定义的部分
    "--info:" + ... 
    //... 部分的格式及参数占位由 clog 宏中参数 fmt 控制
 ~~~

 - 日志记录 (log dump)  
 将 logger 中缓存的数据按一次性输出到 stdout / 文件 中
 暂时没有实现

 此外, 客户端还仿照游戏 《饥荒》实现了 develop 模式下输出到屏幕的即时日志
 

 # 客户端
 客户端需要三个线程
 - 绘制线程 R (Renderer)
 - 操作线程 U (UserIO)
 - 通信线程 N (NetIO)

 除此之外还, 消息分发 (msgdispatch.h), 
 
 ## 绘制线程 R   
  绘制分为三级 `动作(Act)`, `任务(Task)`, `对象(Paintobj)`  
  - Act  
  动作由 C++ 通过 sol 注册到 Lua 中名为 Act 的全局表中, 均为 EasyX 的基本绘制函数, 如 `circle`, `rectangle` 等   
  详见 threads.cpp/Render/regist_act   
  
  - Task  
  任务在 Lua 脚本层定义, 用于绘制动画/特效效果. Task 是一个协程函数. 一个任务中包含任务触发的坐标, 持续时长, 以及多个用于绘制图像的 Act.  
  Task 在 C++ 通过 Render::submit 接口调用, 根据参数将不同 Kind 的绘制任务添加到名为 TaskQue 的全局表中, 作为 Lua 的绘制任务队列. Lua 层每一帧依次执行任务队列中的任务并将 status 为 dead 的协程对象剔除任务队列. 
  其他线程可以通过 MsgDispatch 来调用 Render::on_msg 接口, 实现跨线程注册绘制任务, 用于实现事件驱动效果.  
  Task 定义详见 resource/Paint/corender.lua

  - Paintobj  
  对象在 Lua 脚本层定义, 是固定显示在界面上的元素, 如力度条/角度指示器等. 由于其相对固定, 因此不通过绘制任务注册, 单独维护在 Lua 名为 PaintAble 的全局表中.

  特别的, 由于地图作为背景固定且占用内存较大, 对查询修改性能要求较高, 因此在 C++ 层面的 GameInfo 中维护并作绘制. 

  协程使用参考 [Lua5.4 Reference Manual](https://www.lua.org/manual/5.4/manual.html)

  ## 操作线程 U
  操作线程不断地读取 EasyX 提供的接口 getmessage 获取用户的键鼠操作, 并根据当前游戏状态决定的过滤条件来对操作进行过滤, 不符合过滤条件的操作直接被丢弃, 符合条件的输入将被判定有效并作进一步处理.  

  操作线程内部维护一个状态机和过滤器 (filiter), 分别用于实现对用户操作的粗过滤和细过滤.  
  
  操作线程状态机的状态由全局状态机 (state::client::*)的状态决定, 分为三种, 用于粗过滤用户输入
  - OnlyKey     
  仅允许键盘输入
  - OnlyMouse   
  仅允许鼠标输入
  - Both  
  键鼠操作均可  

  细过滤器为定义在 threads.h 中, 类型定义为: 
  ``` C++
    using InputFiliter = std::function<bool(ExMessage&&)>;
  ```
  
  一般的, 在非 battle 状态下, 细过滤器由状态机控制, 在 battle 状态下, 细过滤器受到 网络线程 通过 MsgDispather 来调用 UserIO 的 on_msg 接口控制, 达到同一个回合不同阶段对输入逻辑的判定不同的效果.

  对于用户的输入, 例如角度/力度/技能等, 将经过粗糙的处理发送给战斗服务器, 由服务器进行计算, 用于防止~~并不存在的~~作弊.

  ## 网络线程 N
  网络线程用于发送数据包到服务器, 并接受服务器的数据包, 并通过 MsgDispatcher 调用其他线程的接口来实现事件驱动.

  网络线程内部维护一个状态机, 分别是
  - ToLoginServ  
  连接到登陆服务器
  - ToBattleServ   
  连接到战斗服务器  
  - Offline  
  离线  
  
  其中, 前两种状态的转换受到全局状态机的控制, 且无论是哪种状态, 都通过 TCP 协议与远端链接, 且通信格式均为 JSON. 不使用 UDP 协议是因为这个游戏对实时性要求不高, 且对通信的稳定性有要求, 因而选择自带重传的 TCP 协议.

  hv::TcpClient 的用法, 参考 [libhv使用教程](https://blog.csdn.net/GG_SiMiDa/article/details/113733758) , 以及 [muduo网络库](https://github.com/chenshuo/muduo-tutorial)  





 
 # 开发环境搭建(不完整)
 按照依次按照如下步骤安装， 默认使用 VS2019 
 - 使用官网自动安装程序安装 EazyX  
 [EazyX 官网地址](https://easyx.cn/)  
 [EazyX 文档地址](https://docs.easyx.cn/zh-cn/intro)   
 - 下载 Lua 源代码, 并将其中的.h 文件放置到 VS2019 的 `include` 目录下  
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
 注: 可以通过在 VS2019 的项目设置中选择 C/C++ 头文件包含目录中指定 EasyX 与 Lua 源码所在文件夹, 从而无需污染 VS2019 的库目录, 同时将在 链接器 -> 输入 中添加 编译后的 lua54.lib / easyx.lib 也可起到同样效果.  
 动态链接也是同理 ~~(懂得都懂)~~ 


 - 从 github 上 clone 该项目到本地  
 使用 `.sln` 文件打开解决方案, 确保 `Client` 与 `BattleServer` 两个项目中的 `项目->属性` 页中  
 `c/c++ -> 预处理器 -> 输入` 中存在 `HV_STATICLIB` 宏 (引入第三方库)  
 `c/c++ -> 链接器 -> 输入 -> 附加依赖项` 中包含了 `Trajectory\lib` 目录下的所有 `lib` 文件  
 ~~`c/c++ -> 链接器 -> 输入 -> 忽略指定默认库`  中包含 `msvcrt.lib`~~

到这里, 开发环境搭建完成, 编译运行测试程序来测试第三方库能否正常使用