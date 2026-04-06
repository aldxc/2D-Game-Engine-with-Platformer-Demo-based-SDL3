#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <SDL3/SDL.h>
#include "Tile.h"

//最好单例
class Resource {
private:
	Resource() = default;
	~Resource() = default;
public:

	struct LevelData {
		std::uint64_t width = 0;
		std::uint64_t height = 0;
		std::vector<std::vector<Tile>> tiles = {};
	};

	//禁用拷贝与移动
	Resource(const Resource&) = delete;
	Resource& operator=(const Resource&) = delete;
	Resource(Resource&&) = delete;
	Resource& operator=(Resource&&) = delete;

	static Resource& getInstance() {
		static Resource instance;
		return instance;
	}
	bool init() noexcept; // 初始化资源，如加载纹理、字体等

	// 地图 level 文件读写
	bool loadLevel(const std::string& filePath, std::vector<std::vector<Tile>>& Tiles) const;
	bool saveLevel(const std::string& filePath, const LevelData& level) const;

	//	瓦片纹理读取
	std::shared_ptr<SDL_Texture> loadTexture(const std::string& filePath, SDL_Renderer* renderer);
private:
	std::unordered_map<std::string, std::shared_ptr<SDL_Texture>> textureCache_; // 资源缓存，可以根据实际需求定义具体的资源类型和管理方式
};