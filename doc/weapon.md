# 武器
武器模块位于 `resource / weapon.lua` 中  
弹药命中地形将会产生爆炸, 除了会基于玩家与爆点的距离造成不同的伤害比例, 还会造成挖坑效果.  
伤害计算范围和挖坑效果的计算是互相独立的, 互不影响  
其中, 伤害计算为圆形, 分为内外两层, 内层为爆心, 受到全额伤害, 外层为爆沿, 受到衰减比例的影响, 不同武器的衰减比例各不相同, 建议为 `70% - 90%`   
挖坑效果计算参见 `TrapEffect`  

用于描述一个武器的 表(table) 需要包含如下字段
- Desciption string 描述
用于显示武器的描述

- BaseAttack int 基准攻击力     
处于爆炸范围内的人物将会受到基于该数值的伤害 

- DecayRate double: [0, 1] 爆沿伤害衰减比例    
与爆点距离大于爆心半径小于该值的敌人将会受到 `DecayRate * BaseAttack` 的伤害

- StrikeRate double: [0, 1] 暴击率  
暴击将会造成 `150%` 伤害

- CenterRadius int 爆心半径   
与爆点距离小于该值的敌人将会受到 `100%` 于攻击力的伤害

- EdgeRadius 爆沿半径  
与爆点距离大于爆心半径小于该值的敌人将会受到 `DecayRate * BaseAttack` 的伤害

- TrapEffect 挖坑效果计算公式  
挖坑效果计算是一个 lua 函数, 该函数输入为弧度, 输出为该弧度方向上的破坏半径, 即 `坐标轴正方向水平向右` 的 `极坐标系` 下的挖坑效果边缘公式  
例如, 下面这个函数将会造成一个半径等于 100 的圆的挖坑效果  
~~~ lua
    ...
    TrapEffect = function(rad) 
        return 100
    end,
    ...
~~~   

这个函数将会造成一个椭圆形状的挖坑效果, 其中 e 为离心率, a为长轴  
~~~ lua
    ...
    TrapEffect = function(rad)
        -- 椭圆极坐标方程 
        return a * (1 - e * e) / (1 - e * math.cos(rad)) 
    end
    ...
~~~ 

代码模板
- 为了方便修改配置文件, TrapEffect 函数将会有第二个参数 args, 调用时将会传入 weapon 表中 Args 字段作为第二个参数   

如下是 `Weapon.lua` 中 一个武器的定义, 爆炸效果为椭圆
~~~ lua
    ["Weapon name"] = {
        BaseAttack      = 100,
        DecayRate       = 0.8,
        StrikeRate      = 0.2,
        CenterRadius    = 30,
        EdgeRadius      = 100,

        Description = "input weapon desciption here",
        
        Args = {
            e = 0.75,
            a = 130,
        },

        TrapEffect = function (rad, args)
            return args.a * (1 - math.pow(args.e, 2)) / (1 - args.e * math.cos(rad))
        end
    }

    --[[ 如果爆炸效果为一个圆, 那么后两个字段则改成
        Args = {
            r = 100,
        },

        TrapEffect = function (rad, args) 
            return args.r
        end
    ]]

    --[[ 爆炸效果为六边形

    ]] 
~~~

## 挖坑效果详解  
- 以爆心为坐标原点, 向右为正方向, 顺时针为正角建立极坐标系   
- 每隔 `15°`, 调用一次武器的 TrappEffect 函数, 得到 13 个点  
- 将这些点依次用直线连接, 服务端清除内部的碰撞模型, 客户端额外遮盖对应像素点
