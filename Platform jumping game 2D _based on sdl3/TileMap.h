#pragma once
#include <vector>
#include <string>
#include <SDL3/SDL.h>
#include "Tile.h"

class TileMap {
public:
	struct CollisionResult {
		SDL_FRect hitbox; // 碰撞后的碰撞盒位置和大小
		float velocityX; // 碰撞后的水平速度
		float velocityY; // 碰撞后的垂直速度
		//bool hitLeft; // 是否碰撞到左侧
		//bool hitRight; // 是否碰撞到右侧
		//bool hitTop; // 是否碰撞到顶部
		//bool hitBottom; // 是否碰撞到底部
		bool isLanded; // 是否着陆，表示玩家是否站在地面上
	};
	explicit TileMap() = default;
	~TileMap() = default;
	void update(float dt) noexcept;
	void render() const noexcept;
	void setTile(const std::vector<std::vector<Tile>>& tiles) noexcept { tiles_ = tiles; }
	CollisionResult tileCollision(const SDL_FRect& hitBox, float velocityX, float velocityY, bool isLanded, float dt) const noexcept;
	void calculateTileTypeToTextureIndexMapping() noexcept; // 计算瓦片类型到纹理索引的映射关系
private:
	std::vector<std::vector<Tile>> tiles_ = {}; // 2D瓦片地图数据，tiles_[y][x]表示第y行第x列的瓦片
	std::vector<std::vector<uint8_t>> tileTypeToTextureIndex_ = {}; // 瓦片类型到纹理索引的映射
};