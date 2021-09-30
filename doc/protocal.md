# 通信协议总述
以下协议均为 JSON 格式  

## 用于描述自身信息的 descriptor

- client  
~~~ JSON
{
    "Uid"   :   123456,     // int_hash, created by login server
    "Name"  :   "xxxxx",    // str, input by user
    "Player"    :           // json decided by user 
    {
        "Weapon"    :   1,  // int enum
        "Style"     :       // json
        {
            "FaceColor" :   321,    // RGB value
            "HelmetColor"   :   321,    // RGB value
            "FigureColor"   :   321     // RGB value
        }
    }
}
~~~

- battle server
~~~ JSON
{
    "Name"  :   "official",   // str, create by battle server config file
    "Address"   :   "192.168.233.14",   // str    
}
~~~

- room 房间
~~~ JSON
{
    "Rid"       :   666,    // int_hash, input by user
    "Slogan"    :   "why not have a try, boy?",  // str, input by user
    "OnBattle"  :   false,  // bool, is on battle flag
    "Server"    :           // json   
    {
        // as battle server
    },

    "Players"   :   // array 
    [
        // client descriptors
    ]
}
~~~

- weapon  武器
参见 weapon.md

- area 地图
参见 area.md 

## client <-> login server
 正常情况下二者间通信， 包含三个字段  
- Type  : string        "request" | "order" | "reply" | "return"  
 该字段标记了消息的主类型, 其中 
 + + request    泛指请求数据
 + + order      泛指有效操作
 + + reply      用于回复 request
 + + return     用于回复 order

- SubType   :   int (enum) 
 该字段标记主类型下消息子类型, 均为从 1 开始的枚举值, 各种情况下意义为:
 + + request
 + + + 1    请求大厅中所有房间数据
 + + + 2    请求所有武器数据
 + + + 3    请求地图数据
 + + + 4    待定
 
 + + reply
 各项枚举值与 request 相同 

 + + order
 + + + 1    创建一个新房间
 + + + 2    加入房间
 + + + 3    离开房间
 + + + 4    开始游戏指令

 + + return
 各项枚举值与 order 相同

- Appendix  : json  
 该字段是一个 json , 包含每次通信的参数, json 的格式由 前两个字段决定  
 + + request
 + + + 1    请求大厅中所有房间数据
 ~~~ JSON
 // "Appendix" : 
 {
    "ReqCounter"    :   0,  // int, 本机客户端第 n 次请求房间列表
 }
 ~~~

 + + + 2    请求所有武器数据
 ~~~ JSON
 {
    "LocalMd5" :    123456, // int, 本地缓存武器配置文件的 MD5 校验码, 若无文件则该字段为 0
 }
 ~~~

 + + + 3    请求地图数据
 ~~~ JSON
 {
    "LocalMd5" :    123457, // int, 本地缓存地图文件的 MD5 校验码, 若无文件则该字段为 0
 }
 ~~~

 + + + 4    待定
 
 + + reply
 + + + 1    回复大厅中所有房间数据
 ~~~ JSON
 {
    "HasRoom"   :   true,   // bool, 表明是否存在房间  
    "Rooms" :       // array, exist when HasRoom is true
    [
       // room descriptors
    ], 
    
    "YourId"    :   123456  // int_hash, exist when "ReqCounter" == 0
 }
 ~~~

 + + + 2    回复所有武器数据
 ~~~ JSON
 {
    "Modified"  :   true,   // int, MD5 校验值不同则为 true, 同时后者有意义
    "NewFile"   :   "",     // str, 整个 weapon.lua 文件, 前者 false 时无意义
 }
 ~~~
 
 + + + 3    回复地图数据
 ~~~ JSON
 {
    "Modified"  :   true,   // int, MD5 校验值不同为 true, 同时后者有意义
    "NewFile"   :   "",     // str, 整个 Area.json 文件, 前者 false 时无意义
 }
 ~~~
  
 + + + 4    待定
 
 + + order
 + + + 1    创建一个新房间
 ~~~ JSON
 {
    "RoomInfo"  :       // json
    {
        // room descriptor without "Players" and "OnBattle"
    },

    "Creater"   :       // json
    {
        // client self descriptor
    }
 }
 ~~~
  
 + + + 2    加入房间
 ~~~ JSON
 {
     "Target"   :   666,    // int, target room id
 }
 ~~~
  
 + + + 3    离开房间
 ~~~ JSON
 // "Appendix" : 
 null                       // null
 ~~~
 
 + + + 4    开始游戏指令
 ~~~ JSON
 // "Appendix" : 
 null                       // null
 ~~~  

 特别的, 在以下情况, 由客户端发起一次主动心跳, 此时 仅包含 "Type" 字段, 且其值为 "beat" , 当 login server 收到这样的消息时 充当 echo 服务器返回原文, 客户端收到返回重置心跳计时器.  
 当 login server 60s 未收到客户端的任何消息时, 将视为其断线
 - 客户端第一次上线时
 - 当超过 15s 未互通消息时
 