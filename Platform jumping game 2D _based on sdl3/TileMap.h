#pragma once
#include <vector>
#include <unordered_map>
#include <SDL3/SDL.h>
#include <memory>
#include "Tile.h"
#include "physics/PhysicsCollMap.h"
#include "Resource/Resource.h"
#include "core/Timer.h"

class Renderer;
class Camera;

class TileMap {
public:
	explicit TileMap(Renderer& renderer, Resource& rM) noexcept;
	~TileMap() noexcept = default;

	void update(float dt) noexcept;
	void render(const Camera& camera) const noexcept;
	void setData(const std::vector<std::vector<Tile>>& tilesMap, const std::vector<Resource::ObjectDate>& objs) noexcept;

	int getMapWidth() const noexcept { return tiles_.empty() ? 0 : static_cast<int>(tiles_[0].size()); }
	int getMapHeight() const noexcept { return static_cast<int>(tiles_.size()); }

	const std::vector<std::vector<physicalCollMap>>& getPhysicalCollisionMap() const noexcept { return physicalCollisionMap_; }

	bool isCanClimb(const SDL_FRect& hitBox) const noexcept;

	std::vector<Resource::ObjectDate>& getObjects() noexcept { return mapObjects_; }
	const std::vector<Resource::ObjectDate>& getObjects() const noexcept { return mapObjects_; }

	// 判断玩家是否处于死亡区域
	bool isInDeadArea(const SDL_FRect& hitBox) const noexcept;

	// 判断玩家是否进入特定类型的触发区域
	bool isInTriggerArea(const SDL_FRect& hitBox) const noexcept;

	// 将已经激活的对象删除
	void cleanUpActiveObject() noexcept;

	void addAnimationFrame(size_t i, size_t j, const std::vector<std::pair<uint16_t, uint32_t>>& frames) noexcept;
	void updateTileAnimations(double dt) noexcept;

	std::unordered_map<size_t, std::pair<std::vector<std::pair<uint16_t, uint32_t>>, Timer>>& getAnimationFrames() noexcept { return animationFrames_; }
	size_t getMapCol() const noexcept { return tiles_.empty() ? 0 : tiles_[0].size(); }

private:
	// 外部依赖
	Renderer& renderer_;

	// 地图渲染数据
	std::vector<std::vector<Tile>> tiles_ = {};
	std::shared_ptr<SDL_Texture> tilesTexture_ = nullptr;

	// 地图对象数据，例如敌人出生点、道具位置等
	std::vector<Resource::ObjectDate> mapObjects_ = {};

	// 碰撞层数据，提供给物理系统进行碰撞检测
	std::vector<std::vector<physicalCollMap>> physicalCollisionMap_ = {};

	// 瓦片动画帧数据
	// key 为瓦片在地图中的位置索引（i * col + j）
	// value 为该位置的动画帧数据和动画计时器
	std::unordered_map<size_t, std::pair<std::vector<std::pair<uint16_t, uint32_t>>, Timer>> animationFrames_ = {};
};