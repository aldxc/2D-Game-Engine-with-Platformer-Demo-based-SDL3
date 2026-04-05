#pragma once
#include <memory>
#include <string>
#include "resource/Resource.h"
#include "TileMap.h"

class Resource;

namespace Maploader {
	static std::unique_ptr<TileMap> loadMap(const std::string& filePath, Resource& rm) {
		// 这里可以添加实际的地图加载逻辑，例如从文件中读取地图数据并解析成TileMap对象
		// 目前只是返回一个空的TileMap实例作为示例
		std::vector<std::vector<Tile>> tiles;
		rm.loadLevel(filePath, tiles); // 从文件加载地图数据，后续可以根据实际需求定义文件格式和解析逻辑

		auto tileMap = std::make_unique<TileMap>();
		tileMap->setTile(tiles);

		//生成一个测试地图，32列18行瓦片，底部一行是完全碰撞的陆地，其他都是空地
		//std::vector<std::vector<Tile>> testTiles;
		//testTiles.resize(18, std::vector<Tile>(32, Tile{ TileType::EMPTY, CollisionType::NONE, 0 }));
		//for (int i = 0; i < 32; ++i) {
		//	testTiles[17][i] = Tile{ TileType::LAND, CollisionType::FULL, 0 };
		//}
		//rm.saveLevel("resource/level2.bin", Resource::LevelData{ 32, 18, testTiles }); // 将测试地图保存到文件，后续可以删除这行代码

		return tileMap;
	}
}