#pragma once
#include <memory>
#include <string>
#include <cstdint>
#include "resource/Resource.h"

#include "TileMap.h"

namespace Maploader {
	static std::unique_ptr<TileMap> loadMap(const std::string& filePath, Resource& rm) {
		std::vector<std::vector<uint64_t>> data;
		rm.loadMap(filePath, data); // 从tmx文件加载地图数据，后续增加有限地图和无限地图的区分处理等功能

		auto tileMap = std::make_unique<TileMap>();

		if(data.empty() || data[0].empty()) {
			return tileMap; // 如果地图数据为空，直接返回空的TileMap实例
		}

		std::vector<std::vector<Tile>> tilemap;
		tilemap.resize(data.size(), std::vector<Tile>(data[0].size(), Tile{ 0, 0, CollisionType::NONE, 0 }));
		for(size_t i = 0; i < data.size(); ++i) {
			for(size_t j = 0; j < data[i].size(); ++j) {
				uint64_t tileValue = data[i][j];
				uint16_t srcX = static_cast<uint8_t>((tileValue >> 0) & 0xFF) * Config::TILE_SRC_WIDTH;// 根据最低8位获取瓦片在纹理中的源矩形左上角X坐标，乘以瓦片源矩形的宽度得到实际的像素坐标
				uint16_t srcY = static_cast<uint8_t>((tileValue >> 8) & 0xFF) * Config::TILE_SRC_HEIGHT;// 根据第8-15位获取瓦片在纹理中的源矩形左上角Y坐标，乘以瓦片源矩形的高度得到实际的像素坐标
				uint8_t collisionValue = static_cast<uint8_t>((tileValue >> 16) & 0xFF); // 根据第16-23位获取碰撞类型的数值
				uint16_t flags = static_cast<uint16_t>((tileValue >> 24) & 0xFFFF); // 根据第24-39位获取其他属性标志位的数值，后续可以根据需要定义具体的标志位含义和对应的数值
				CollisionType collision = CollisionType::NONE; // 默认碰撞类型为NONE
				switch (collisionValue) {
					case 0:
						collision = CollisionType::NONE;
						break;
					case 1:
						collision = CollisionType::HALF;
						break;
					case 2:
						collision = CollisionType::FULL;
						break;
					default:
						collision = CollisionType::NONE;
						break;
				}
				tilemap[i][j] = Tile{ srcX, srcY, collision, flags };
			}
		}
		tileMap->setTiles(tilemap);

		return tileMap;
	}
}