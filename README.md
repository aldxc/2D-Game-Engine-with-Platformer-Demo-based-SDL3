# Platform jumping game 2D _based on sdl3

resource 只保存纯数据，不存储任何其他自定义类型，
tmxToPngSrcRect tmx - Pngsrc tmx映射png纹理时存储的为xy的相对坐标（不包含w、h），取出后处理时应当乘以对应width和height映射出对应rectangle
取数据时碰撞约定：0 - 无碰撞，1 - 半碰撞，2 - 全碰撞（仅上半部分碰撞） ...
约定所有数据以整型存储，使用时类内自行判断枚举类型

暂未将tmx转为bin格式，直接在运行时解析tmx文件，后续可以考虑将tmx转为bin格式以提升加载效率

暂未将组件抽象为component system，后续可以考虑将组件抽象为component system以提升代码的可维护性和扩展性

引擎组件需包含init接口供初始化，destroy接口供销毁