#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <SDL3/SDL.h>
#include <tmxlite/Map.hpp>

//最好单例
class Resource {
private:
	Resource() = default;
	~Resource() = default;
public:

	struct LevelData {
		std::uint64_t width = 0;
		std::uint64_t height = 0;
		std::vector<std::vector<uint64_t>> tiles = {};
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
	bool loadLevel(const std::string& filePath, std::vector<std::vector<uint64_t>>& Tiles) const noexcept;
	bool saveLevel(const std::string& filePath, const LevelData& level) const noexcept;
	bool loadMap(const std::string& filePath, std::vector<std::vector<uint64_t>>& Tiles) noexcept; 


	//	瓦片纹理读取
	std::shared_ptr<SDL_Texture> loadTexture(const std::string& filePath, SDL_Renderer* renderer) noexcept;
private:
	bool loadTmxMap(const std::string& filePath, std::vector<std::vector<uint64_t>>& Tiles) noexcept; // 有限地图加载
	void tmxToPngSrcRectAndColl() noexcept; // 将tmx地图数据转换成png纹理的源矩形和碰撞 // 只保存x y

private:
	std::unordered_map<std::string, std::shared_ptr<SDL_Texture>> textureCache_; // 资源缓存，可以根据实际需求定义具体的资源类型和管理方式

	tmx::Map mapData_; // tmx地图数据，后续将tmx转换成bin存储
	std::unordered_map<uint32_t, uint16_t> tileTypeToSrcRectXY_; // GID - 源矩形左上角坐标的映射关系 
	//uint64_t的低16位存储srcX和srcY坐标（每个坐标8位）
	std::unordered_map<uint32_t, uint8_t> tileTypeToCollision_; // GID - 碰撞类型的映射关系，0表示无碰撞，1表示半碰撞，2表示完全碰撞
};