#include "resource/Resource.h"
#include <SDL3/SDL.h>
#include <fstream>
#include <cstdint>



bool Resource::init() noexcept {

	return true;
}

bool Resource::loadLevel(const std::string& filePath, std::vector<std::vector<Tile>>& Tiles) const{
	std::ifstream ifs(filePath, std::ios::binary);
	if(!ifs.is_open()) {
		SDL_Log("Failed to open file for reading: %s", filePath.c_str());
		return false;
	}
	uint64_t width = 0, height = 0;
	ifs.read(reinterpret_cast<char*>(&width), sizeof(width));
	ifs.read(reinterpret_cast<char*>(&height), sizeof(height));
	Tiles.resize(height, std::vector<Tile>(width));
	//读取地图宽高
	//ifs.read(reinterpret_cast<char*>(&outLevel.width), sizeof(outLevel.width));
	//ifs.read(reinterpret_cast<char*>(&outLevel.height), sizeof(outLevel.height));
	//outLevel.tiles.resize(outLevel.height, std::vector<Tile>(outLevel.width));
	
	//读取瓦片数据
	for(auto& tile : Tiles) {
		for(auto& t : tile) {
			uint64_t packedData = 0;
			ifs.read(reinterpret_cast<char*>(&packedData), sizeof(packedData));
			t.type = static_cast<TileType>((packedData >> 0) & 0xFF);
			t.collision = static_cast<CollisionType>((packedData >> 8) & 0xFF);
			t.flags = static_cast<uint32_t>((packedData >> 16) & 0xFFFFFFFF);
		}
	}

	return true;
}

bool Resource::saveLevel(const std::string& filePath, const LevelData& level) const{
	//以二进制文件方式打开文件进行写入
	std::ofstream ofs(filePath, std::ios::binary);
	if (!ofs.is_open()) {
		SDL_Log("Failed to open file for writing: %s", filePath.c_str());
		return false;
	}
	//写入地图宽高
	ofs.write(reinterpret_cast<const char*>(&level.width), sizeof(level.width));
	ofs.write(reinterpret_cast<const char*>(&level.height), sizeof(level.height));
	
	for (const auto& tile : level.tiles) {
		for (const auto& t : tile) {
			//将Tile的type、collision和flags打包成一个64位整数进行写入，节省空间
			uint64_t packedDate = (static_cast<uint64_t>(t.type) << 0 | static_cast<uint64_t>(t.collision) << 8 | static_cast<uint64_t>(t.flags) << 16);
			ofs.write(reinterpret_cast<const char*>(&packedDate), sizeof(packedDate));
		}
	}
	SDL_Log("Level saved successfully to: %s", filePath.c_str());
	return true;
}
