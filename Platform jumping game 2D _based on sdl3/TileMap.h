#pragma once
#include <vector>
#include "Tile.h"

class TileMap {
public:
	struct Tile {
		TileType type; // 瓦片类型，如陆地、空中等
		CollisionType collision; // 碰撞类型，如无碰撞、完全碰撞、半碰撞等
		uint32_t flags; // 其他属性标志位，如是否可破坏、是否有特殊效果等，后续可以根据需要定义具体的标志位含义
	};
	explicit TileMap() = default;
	~TileMap() = default;
	void update(float dt) noexcept;
	void render() const noexcept;
private:
	std::vector<std::vector<Tile>> tiles_; // 2D瓦片地图数据，tiles_[y][x]表示第y行第x列的瓦片
};