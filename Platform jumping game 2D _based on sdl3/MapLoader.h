#pragma once
#include <memory>
#include <string>
#include <cstdint>
#include "resource/Resource.h"

#include "TileMap.h"

namespace Maploader {
	static std::unique_ptr<TileMap> loadMap(const std::string& filePath, Resource& rm) {
		// 这里可以添加实际的地图加载逻辑，例如从文件中读取地图数据并解析成TileMap对象
		// 目前只是返回一个空的TileMap实例作为示例
		std::vector<std::vector<uint64_t>> data;
		rm.loadLevel(filePath, data); // 从文件加载地图数据，后续可以根据实际需求定义文件格式和解析逻辑

		auto tileMap = std::make_unique<TileMap>();
		std::vector<std::vector<Tile>> tiles;
		tiles.resize(data.size(), std::vector<Tile>(data[0].size(), Tile{ TileType::EMPTY, CollisionType::NONE, 0 }));

		for(int i = 0; i < data.size(); ++i) {
			for(int j = 0; j < data[i].size(); ++j) {
				uint64_t tileValue = data[i][j];
				TileType type = static_cast<TileType>((tileValue >> 0) & 0xFF); // 根据最低8位判断瓦片类型，后续可以根据需要定义更多的瓦片类型和对应的数值
				CollisionType collision = static_cast<CollisionType>((tileValue >> 8) & 0xFF);// 根据第8-15位判断碰撞类型，后续可以根据需要定义更多的碰撞类型和对应的数值
				uint32_t flags = static_cast<uint32_t>((tileValue >> 16) & 0xFFFFFFFF); // 根据第16-47位判断其他属性标志位，后续可以根据需要定义具体的标志位含义和对应的数值
				tiles[i][j] = Tile{ type, collision, flags }; // 创建Tile对象
			}
		}

		tileMap->setTile(tiles);
		tileMap->calculateTileTypeToTextureIndexMapping(); // 计算瓦片类型到纹理索引的映射关系

		//生成一个测试地图，32列18行瓦片，底部一行是完全碰撞的陆地，其他都是空地
		//std::vector<std::vector<Tile>> testTiles;
		//testTiles.resize(18, std::vector<Tile>(32, Tile{ TileType::EMPTY, CollisionType::NONE, 0 }));
		//for (int i = 0; i < 32; ++i) {
		//	testTiles[17][i] = Tile{ TileType::LAND, CollisionType::FULL, 0 };
		//}
		//rm.saveLevel("resource/level2.bin", Resource::LevelData{ 32, 18, testTiles }); // 将测试地图保存到文件，后续可以删除这行代码
		//for (auto& row : tiles) {
		//	for (auto& tile : row) {
		//		if (tile.collision == CollisionType::NONE) {
		//			tile.type = TileType::EMPTY;
		//		}
		//		else {
		//			tile.type = TileType::LAND;
		//		}
		//	}
		//}
		//rm.saveLevel("resource/level1.bin", Resource::LevelData{ 32, 18, tiles }); // 将测试地图保存到文件，后续可以删除这行代码


		return tileMap;
	}
}