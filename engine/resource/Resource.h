#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <tmxlite/Map.hpp>
#include "core/Rect.h"

class Resource {
public:
	Resource() noexcept = default;
	~Resource() noexcept = default;

	struct LevelData {
		std::uint64_t width = 0;
		std::uint64_t height = 0;
		std::vector<std::vector<uint64_t>> tiles = {};
	};

	struct ObjectDate {
		std::string data;
		// 对象的位置和大小
		Rect rect; 
		bool isAcitive = false;
		// 存储对象的动画帧数据，每帧包含一个源矩形位置和一个持续时间，持续时间存储在uint32_t中，单位为毫秒
		std::vector<std::pair<uint16_t, uint32_t>> animationFrames; 
	};

	//禁用拷贝与移动
	Resource(const Resource&) = delete;
	Resource& operator=(const Resource&) = delete;
	Resource(Resource&&) = delete;
	Resource& operator=(Resource&&) = delete;

	// 初始化资源
	bool init() noexcept; 

	// 地图 level 文件读写
	bool loadLevel(const std::string& filePath, std::vector<std::vector<uint64_t>>& Tiles) const noexcept;
	bool saveLevel(const std::string& filePath, const LevelData& level) const noexcept;
	bool loadMap(const std::string& filePath, std::vector<std::vector<uint64_t>>& Tiles, std::vector<ObjectDate>& objectDate) noexcept;


	//	瓦片纹理读取
	std::shared_ptr<SDL_Texture> loadTexture(const std::string& filePath, SDL_Renderer* renderer) noexcept;

	// 游戏数据读写
	void saveGameData(const std::string& filePath, const std::vector<int>& data) noexcept; 
	void saveGameData(const std::string& filePath, const uint32_t date) noexcept; 
	void loadGameData(const std::string& filePath, std::vector<int>& data) noexcept; 

	// 加载音频
	std::shared_ptr<MIX_Audio> loadAudio(const std::string& filePath, MIX_Mixer* mixer) noexcept; 
private:
	// 地图加载
	bool loadTmxMap(const std::string& filePath, std::vector<std::vector<uint64_t>>& Tiles, std::vector<ObjectDate>& objectDate) noexcept; 
	// 将tmx地图数据转换成png纹理的源矩形和碰撞 // 只保存x y
	void tmxToPngSrcRectAndColl(std::unordered_map<uint32_t, std::vector<uint64_t>>& tileIDToAnimationFrames) noexcept; 
private:
	// 资源缓存
	std::unordered_map<std::string, std::shared_ptr<SDL_Texture>> textureCache_; 
	// tmx地图数据，后续将tmx转换成bin存储
	tmx::Map mapData_; 
	// GID - 源矩形左上角坐标的映射关系 
	std::unordered_map<uint32_t, uint16_t> tileTypeToSrcRectXY_; 
	//uint64_t的低16位存储srcX和srcY坐标（每个坐标8位）
	std::unordered_map<uint32_t, uint8_t> tileTypeToCollision_; 
	// 音频缓存
	std::unordered_map<std::string, std::shared_ptr<MIX_Audio>> audioCache_;
};