#pragma once
#include <vector>
#include <string>
#include <array>
#include <SDL3/SDL.h>
#include "Tile.h"
#include "render/Camera.h"


class TileMap {
public:
	struct CollisionResult {
		SDL_FRect hitbox; // 碰撞后的碰撞盒位置和大小
		float velocityX; // 碰撞后的水平速度
		float velocityY; // 碰撞后的垂直速度
		bool isLanded; // 是否着陆，表示玩家是否站在地面上
	};
	explicit TileMap() = default;
	~TileMap() = default;
	void update(float dt) noexcept;
	void render(const Camera& camera) const noexcept;
	void setTiles(const std::vector<std::vector<Tile>>& tilesMap) noexcept { tiles_ = tilesMap; }
	CollisionResult tileCollision(const SDL_FRect& hitBox, float velocityX, float velocityY, bool isLanded, float dt) const noexcept; // 基于地图数据的碰撞检测

	int getMapWidth() const noexcept { return tiles_.empty() ? 0 : static_cast<int>(tiles_[0].size()); }
	int getMapHeight() const noexcept { return static_cast<int>(tiles_.size()); }
private:
	std::vector<std::vector<Tile>> tiles_ = {}; // tmx地图数据

};