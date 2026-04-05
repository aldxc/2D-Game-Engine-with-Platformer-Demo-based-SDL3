#pragma once
#include <memory>
#include <string>
#include "TileMap.h"

class Resource;

namespace Maploader {
	static std::unique_ptr<TileMap> loadMap(const std::string& filePath, Resource& rm) {
		// 这里可以添加实际的地图加载逻辑，例如从文件中读取地图数据并解析成TileMap对象
		// 目前只是返回一个空的TileMap实例作为示例
		return std::make_unique<TileMap>();
	}
}