#include "Resource.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <fstream>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>

bool Resource::init() noexcept {

	return true;
}

bool Resource::loadLevel(const std::string& filePath, std::vector<std::vector<uint64_t>>& Tiles) const noexcept {
	std::ifstream ifs(filePath, std::ios::binary);
	if(!ifs.is_open()) {
		SDL_Log("Failed to open file for reading: %s", filePath.c_str());
		return false;
	}
	uint64_t width = 0, height = 0;
	ifs.read(reinterpret_cast<char*>(&width), sizeof(width));
	ifs.read(reinterpret_cast<char*>(&height), sizeof(height));
	Tiles.resize(height, std::vector<uint64_t>(width));
	
	//读取瓦片数据
	for(auto& tile : Tiles) {
		for(auto& t : tile) {
			ifs.read(reinterpret_cast<char*>(&t), sizeof(t));
		}
	}

	return true;
}

bool Resource::saveLevel(const std::string& filePath, const LevelData& level) const noexcept {
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
			ofs.write(reinterpret_cast<const char*>(&t), sizeof(t));
		}
	}
	SDL_Log("Level saved successfully to: %s", filePath.c_str());
	return true;
}

bool Resource::loadMap(const std::string& filePath, std::vector<std::vector<uint64_t>>& Tiles) noexcept{
	if (filePath.size() >= 4 && filePath.compare(filePath.size() - 4, 4, ".tmx") == 0) {
		return loadTmxMap(filePath, Tiles);
	}
	else if (filePath.size() >= 4 && filePath.compare(filePath.size() - 4, 4, ".bin") == 0) {
		// 直接从二进制文件加载地图数据
	}
	SDL_Log("Unsupported map format: %s", filePath.c_str());
	return false;

}


bool Resource::loadTmxMap(const std::string& filePath, std::vector<std::vector<uint64_t>>& mapTiles) noexcept { 
	if(!mapData_.load(filePath)) {
		SDL_Log("Failed to load tmx map: %s", filePath.c_str());
		return false;
	}
	auto col = mapData_.getTileCount().x; // 获取地图宽度
	auto row = mapData_.getTileCount().y; // 获取地图高度
	tmxToPngSrcRectAndColl(); // 将tmx地图数据转换成png纹理的源矩形和碰撞信息，存储在tileTypeToSrcRectXY_和tileTypeToCollision_中，后续可以根据需要增加其他属性的转换和存储
	mapTiles.assign(row, std::vector<uint64_t>(col, 0));

	auto& layers = mapData_.getLayers();
	for (auto& layer : layers) {
		if (layer->getType() == tmx::Layer::Type::Tile) { //瓦片层
			if (layer->getName() == "land") {
				auto* tileLayer = dynamic_cast<tmx::TileLayer*>(layer.get());
				if (!tileLayer) {
					SDL_Log("Failed to cast layer to TileLayer: %s", filePath.c_str());
					continue;
				}
				const auto& tiles = tileLayer->getTiles();

				if (tiles.size() < static_cast<size_t>(row * col)) {
					SDL_Log("Tile layer data size is invalid: %s", filePath.c_str());
					return false;
				}

				// layer 的瓦片数据是一个一维数组，按照行优先顺序存储，需要转换成二维数组
				// 有限地图中layer 与 mapData_.getTileCount()中的宽高一致，无需考虑无限地图中的chunk数据
				//int width = tileLayer->getSize().x;
				//int height = tileLayer->getSize().y;
				for (int i = 0; i < row; ++i) {
					for (int j = 0; j < col; ++j) {
						const int index = i * col + j;
						const auto GID = tiles[index].ID; // 获取瓦片的全局ID
						if (GID == 0) {
							continue; // GID为0表示该位置没有瓦片，跳过
						}
						mapTiles[i][j] |= (tileTypeToSrcRectXY_[GID] | (static_cast<uint64_t>(1) << 24)); // 根据GID获取对应的源矩形坐标和碰撞信息，并存储在Tiles二维数组中
					}
				}
			}
			else if (layer->getName() == "collision") {
				auto* tileLayer = dynamic_cast<tmx::TileLayer*>(layer.get());
				if (!tileLayer) {
					SDL_Log("Failed to cast layer to TileLayer: %s", filePath.c_str());
					continue;
				}
				const auto& tiles = tileLayer->getTiles();

				if (tiles.size() < static_cast<size_t>(row * col)) {
					SDL_Log("Tile layer data size is invalid: %s", filePath.c_str());
					return false;
				}
				
				for (int i = 0; i < row; ++i) {
					for (int j = 0; j < col; ++j) {
						const int index = i * col + j;
						const auto GID = tiles[index].ID; // 获取瓦片的全局ID
						if (GID == 0) {
							continue; // GID为0表示该位置没有瓦片，跳过
						}
						mapTiles[i][j] |= (static_cast<uint64_t>(tileTypeToCollision_[GID]) << 16); // 根据GID获取对应的源矩形坐标和碰撞信息，并存储在Tiles二维数组中
					}
				}
			}
		}else if(layer->getType() == tmx::Layer::Type::Object) { //对象层
			if(layer->getName() == "object") {
				auto* objectGroup = dynamic_cast<tmx::ObjectGroup*>(layer.get());
				const auto& objects = objectGroup->getObjects();
				for(const auto& obj : objects) {
					// 处理对象层中的对象数据，根据需要存储在Tiles二维数组中
					// 例如，可以根据对象的类型、位置等信息设置对应的属性位
					// 这里假设对象的类型存储在obj.getClass()中，位置存储在obj.getPosition()中
					// 可以根据实际需求进行调整
					//std::vector
					//if(obj.getClass() == "spawn") {
					//	if(obj.getName() )
					//}
				}
			}
		}
	}

	return true;
}

void Resource::tmxToPngSrcRectAndColl() noexcept {
	for (const auto& tileset : mapData_.getTilesets()) {
		// 纹理集名字
		if (tileset.getName() == "sheet") { // sheet 获取瓦片纹理
			int col = tileset.getColumnCount(); // tileset每行几个瓦片
			int tileSize = mapData_.getTileSize().x;
			uint32_t firstGID = tileset.getFirstGID();

			int tileCount = tileset.getTileCount(); // tileset里有多少瓦片

			for (int i = 0; i < tileCount; ++i) {
				uint32_t tileID = firstGID + i;
				uint8_t srcX = (i % col);
				uint8_t srcY = (i / col);
				tileTypeToSrcRectXY_[tileID] = (static_cast<uint16_t>(srcX) | static_cast<uint16_t>(srcY) << 8);
			}
		} else if (tileset.getName() == "logictile") { // logictile 获取瓦片碰撞信息
			uint32_t firstGID = tileset.getFirstGID();

			for (const auto& tile : tileset.getTiles()) {
				uint32_t tileID = firstGID + tile.ID;
				const auto& props = tile.properties;

				for (const auto& prop : props) {
					if (prop.getName() == "collision") {
						uint8_t collisionType = 0;
						if (prop.getType() == tmx::Property::Type::Int) { // 碰撞类型int存储 1 -- half 2 -- full
							int collInt = prop.getIntValue();
							collisionType = collInt; // 存储的int值直接对应碰撞类型
						}
						tileTypeToCollision_[tileID] = collisionType;
					}
				}
			}
		}
	}
}

std::shared_ptr<SDL_Texture> Resource::loadTexture(const std::string& filePath, SDL_Renderer* renderer) noexcept {
	auto it = textureCache_.find(filePath);
	if(it != textureCache_.end()) {
		return it->second; // 如果缓存中已经存在该纹理，直接返回
	}

	SDL_Texture* texture = IMG_LoadTexture(renderer, filePath.c_str());
	if(!texture) {
		SDL_Log("Failed to load texture: %s, error: %s", filePath.c_str(), SDL_GetError());
		return nullptr;
	}

	std::shared_ptr<SDL_Texture> texturePtr(texture, [](SDL_Texture* tex) {
		if(tex) SDL_DestroyTexture(tex);
		});
	textureCache_[filePath] = texturePtr; // 将新加载的纹理添加到缓存中
	SDL_SetTextureScaleMode(texturePtr.get(), SDL_SCALEMODE_NEAREST);//就近采样

	return texturePtr;
}
