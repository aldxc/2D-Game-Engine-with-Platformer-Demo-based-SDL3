#pragma once
#include <cstdint>

struct physicalCollMap {
	uint8_t size; // 瓦片大小全部相同，正方形瓦片，size表示瓦片的边长，单位为像素
	uint8_t coll; // 碰撞类型
};