#pragma once
#include <vector>
#include <string>
#include <array>
#include <SDL3/SDL.h>
#include <memory>
#include "Tile.h"
#include "render/Camera.h"
#include "physics/Physics.h"

class Resource;
class Renderer;

class TileMap {
public:
	struct CollisionResult {
		SDL_FRect hitbox; // 碰撞后的碰撞盒位置和大小
		float velocityX; // 碰撞后的水平速度
		float velocityY; // 碰撞后的垂直速度
		bool isLanded; // 是否着陆，表示玩家是否站在地面上
	};
	explicit TileMap(Renderer& renderer, Resource& rM) noexcept;
	~TileMap() noexcept = default;
	void update(float dt) noexcept;
	void render(const Camera& camera) const noexcept;
	void setTiles(const std::vector<std::vector<Tile>>& tilesMap) noexcept;
	//CollisionResult tileCollision(const SDL_FRect& hitBox, float velocityX, float velocityY, bool isLanded, float dt) const noexcept; // 基于地图数据的碰撞检测

	int getMapWidth() const noexcept { return tiles_.empty() ? 0 : static_cast<int>(tiles_[0].size()); }
	int getMapHeight() const noexcept { return static_cast<int>(tiles_.size()); }

	const std::vector<std::vector<Physics::physicalCollMap>>& getPhysicalCollisionMap() const noexcept { return physicalCollisionMap_; }

	bool isCanClimb(const SDL_FRect& hitBox) const noexcept;
private:
	Renderer& renderer_; 

	std::vector<std::vector<Tile>> tiles_ = {}; // 地图数据
	//后续抽象出碰撞层、渲染层，碰撞层专门负责提供给物理系统进行碰撞检测，渲染层专门负责地图的渲染
	std::vector<std::vector<Physics::physicalCollMap>> physicalCollisionMap_ = {}; // 碰撞层数据，提供给物理系统进行碰撞检测，后续增加更多的碰撞属性，例如半碰撞、可攀爬等
	std::shared_ptr<SDL_Texture> tilesTexture_ = nullptr; 
};