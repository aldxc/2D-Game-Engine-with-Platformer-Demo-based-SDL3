# Platform jumping game 2D _based on sdl3

```mermaid
flowchart TD
    %% 深色背景样式
    classDef app fill:#1A237E,stroke:#64B5F6,stroke-width:2px,color:#FFFFFF
    classDef engine fill:#1B5E20,stroke:#81C784,stroke-width:2px,color:#FFFFFF
    classDef base fill:#E65100,stroke:#FFB74D,stroke-width:2px,color:#FFFFFF
    classDef third fill:#4A148C,stroke:#CE93D8,stroke-width:2px,color:#FFFFFF

    subgraph L1 [?? 应用层 - 具体游戏逻辑]
        direction LR
        Game["游戏逻辑<br/>角色控制/玩法"]
        EnemyMgr["EnemyManager<br/>敌人生成/回收"]
    end

    subgraph L2 [?? 引擎核心层 - 通用框架与系统]
        direction LR
        Core["主循环<br/>while(acc>=dt)物理推进"]
        Renderer["渲染系统<br/>三纹理集分层更新"]
        Physics["物理系统<br/>刚体/碰撞/射线"]
        Event["事件队列<br/>每帧统一分发"]
        Input["输入系统<br/>键盘缓存轮询"]
        Resource["资源管理<br/>纹理/文件IO"]
        Audio["音频系统<br/>音效/背景音乐播放"]
        FSM["状态机<br/>通用流程/场景切换"]
        UI["UI管理器<br/>通用界面/按钮管理"]
    end

    subgraph L3 [?? 基础库层 - 通用工具与组件]
        direction LR
        Math["数学库<br/>向量/矩阵"]
        Timer["计时器"]
        Pool["对象池<br/>敌人实例所有权"]
        TMX["TMX解析<br/>Base64/Zlib"]
        Animation["动画机<br/>帧序列控制"]
        Camera["摄像机<br/>视口跟随"]
    end

    subgraph L4 [?? 第三方依赖]
        direction LR
        SDL3["SDL3<br/>跨平台窗口/渲染/音频"]
        Tmxlite["Tmxlite<br/>TMX解析库"]
    end

    %% 层间垂直依赖
    L1 --> L2 --> L3 --> L4

    %% 关键数据流
    Core -.->|固定步长| Physics
    Core -.->|可变帧率| Renderer

    Game -.->|轮询| Input
    FSM -.->|轮询| Input
    UI -.->|轮询| Input

    Event -.->|通知| FSM
    Event -.->|通知| UI
    Event -.->|通知| Audio

    Physics -.->|读取位置| Animation
    Animation -->|提交纹理+位置| Renderer

    EnemyMgr -->|持有| Pool
    EnemyMgr -.->|使用| Animation
    EnemyMgr -.->|使用| Physics

    Resource -->|加载纹理/音频| Renderer
    Resource -->|加载音频| Audio
    TMX -.->|调用| Tmxlite
    Camera -->|设置视口| Renderer

    %% 应用样式
    class Game,EnemyMgr app
    class Core,Renderer,Physics,Event,Input,Resource,Audio,FSM,UI engine
    class Math,Timer,Pool,TMX,Animation,Camera base
    class SDL3,Tmxlite third

    %% 子图背景色
    style L1 fill:#0D1B2A,stroke:#64B5F6,stroke-width:2px,color:#FFFFFF
    style L2 fill:#1B2A1E,stroke:#81C784,stroke-width:2px,color:#FFFFFF
    style L3 fill:#2E1C0A,stroke:#FFB74D,stroke-width:2px,color:#FFFFFF
    style L4 fill:#1E0F2E,stroke:#CE93D8,stroke-width:2px,color:#FFFFFF


resource 只保存纯数据，不存储任何其他自定义类型，
tmxToPngSrcRect tmx - Pngsrc tmx映射png纹理时存储的为xy的相对坐标（不包含w、h），取出后处理时应当乘以对应width和height映射出对应rectangle
取数据时碰撞约定：0 - 无碰撞，1 - 半碰撞，2 - 全碰撞（仅上半部分碰撞） ...
约定所有数据以整型存储，使用时类内自行判断枚举类型

暂未将tmx转为bin格式，直接在运行时解析tmx文件，后续可以考虑将tmx转为bin格式以提升加载效率

暂未将组件抽象为component system，后续可以考虑将组件抽象为component system以提升代码的可维护性和扩展性

引擎组件需包含init接口供初始化，destroy接口供销毁

对象池：对象池用于管理游戏中的对象，避免频繁的创建和销毁对象带来的性能问题。对象池内部拥有对象所有权，外部通过获取对象引用或指针来使用对象，对象池负责对象的生命周期管理。
对象池内部维护一个对象队列，当需要一个对象时，从队列中获取一个对象并返回给外部使用；当对象不再需要时，外部将对象归还给对象池，对象池将其放回队列中以供下次使用。
使用对象池都的对象都需要实现init和reset接口供对象初始化和重置状态，防止脏数据。
对象池为饿汉模式，游戏开始时就创建好预定义数量对象，可进行扩容。
外部可获取对象管理器中的所有活跃对象数组供其它系统交互修改，但不可修改该数组大小、不可添加删除数组中元素。

事件系统：后续添加延迟订阅/退订（标记为脏后下次update再处理），防止退订后事件仍生效；防止同一事件在同一帧内无限传播（设置最大深度）

状态机：后续将状态切换为唯一主动作，UI状态由主状态机切换时伴随状态机生成实例后切换对应UI状态。

致谢：感谢以下资源和工具的支持：