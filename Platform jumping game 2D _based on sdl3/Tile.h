#pragma once
#include <cstdint>

enum class CollisionType : uint8_t { NONE = 0, HALF = 1, FULL = 2, CLIMBABLE = 3, HITED = 4, DEAD = 5 };

struct Tile { // 依旧可以打包成64位数据
	uint16_t srcX; // 瓦片在纹理中的源矩形左上角X坐标
	uint16_t srcY; // 瓦片在纹理中的源矩形左上角Y坐标
	//CollisionType collision; // 碰撞类型，如无碰撞、完全碰撞、半碰撞等
	uint8_t collision; // 碰撞类型的数值表示，0表示NONE，1表示HALF，2表示FULL，3表示CLIMBABLE
	uint16_t flags; // 其他属性标志位，如是否可破坏、是否有特殊效果等，后续可以根据需要定义具体的标志位含义
};