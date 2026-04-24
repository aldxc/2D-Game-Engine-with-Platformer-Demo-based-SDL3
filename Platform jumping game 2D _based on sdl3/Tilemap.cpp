#include <cmath>
#include "TileMap.h"
#include "Config.h"
#include "render/Renderer.h"
#include "render/Camera.h"
#include "resource/Resource.h"

TileMap::TileMap(Renderer& renderer, Resource& rM)noexcept : m_renderer(renderer){
	m_tilesTexture = rM.loadTexture("resource/sheet.png", m_renderer.getSDLRenderer());
}

void TileMap::update(float dt) noexcept {
	// 这里可以添加地图更新逻辑，例如动画、动态元素等
	updateTileAnimations(dt);
	// 清理已经激活的地图对象
	cleanUpActiveObject(); 
}

void TileMap::render(const Camera& camera) const noexcept {

	m_renderer.renderFillRect(SDL_FRect{ 0, 0,Config::LOGIC_WIDTH, Config::LOGIC_HEIGHT }, SDL_Color({ 173, 216, 230, 255 }));

	if (m_tiles.empty() || m_tiles[0].empty()) {
		return;
	}

	// 只渲染视口
	Rect view = camera.getViewport();
	int colStart = static_cast<int>(std::floor(view.x() / Config::TILE_SIZE));
	int colEnd = static_cast<int>(std::ceil((view.x() + view.w()) / Config::TILE_SIZE));
	int rowStart = static_cast<int>(std::floor(view.y() / Config::TILE_SIZE));
	int rowEnd = static_cast<int>(std::ceil((view.y() + view.h()) / Config::TILE_SIZE));


	for (int i = rowStart; i <= rowEnd; ++i) {
		for(int j = colStart; j <= colEnd; ++j) {
			// 渲染视口内的瓦片
			if (i < 0 || i >= static_cast<int>(m_tiles.size()) || j < 0 || j >= static_cast<int>(m_tiles[0].size())) {
				continue;
			}
			const Tile& tile = m_tiles[i][j];
			if (tile.flags == 0) continue;
			Rect srcRect = { static_cast<float>(tile.srcX), static_cast<float>(tile.srcY), Config::TILE_SRC_WIDTH, Config::TILE_SRC_HEIGHT };

			const float worldX = static_cast<float>(j) * Config::TILE_SIZE;
			const float worldY = static_cast<float>(i) * Config::TILE_SIZE;

			Rect dstRect = {
				std::round(worldX - view.x()),
				std::round(worldY - view.y()),
				Config::TILE_SIZE,
				Config::TILE_SIZE
			};
			m_renderer.renderTexture(
				m_tilesTexture.get(),
				srcRect,
				dstRect
			);
		}
	}
}

void TileMap::setData(const std::vector<std::vector<Tile>>& tilesMap, const std::vector<Resource::ObjectDate>& objs) noexcept {
	m_tiles = tilesMap;
	m_mapObjects = objs;
	// 根据瓦片数据生成物理碰撞地图，后续增加更多的碰撞属性，例如半碰撞、可攀爬等
	m_physicalCollisionMap.clear();
	m_physicalCollisionMap.reserve(m_tiles.size());
	for (const auto& row : m_tiles) {
		std::vector<physicalCollMap> collisionRow;
		collisionRow.reserve(row.size());
		for (const auto& tile : row) {
			collisionRow.push_back(physicalCollMap{ Config::TILE_SIZE, tile.collision });
		}
		m_physicalCollisionMap.push_back(collisionRow);
	}
}

bool TileMap::isCanClimb(const SDL_FRect& hitBox) const noexcept{
	if (m_tiles.empty() || m_tiles[0].empty()) {
		return false;
	}

	// 检查玩家碰撞盒中心点所在的瓦片是否具有可攀爬属性
	const float centerX = hitBox.x + hitBox.w / 2.0f;
	const float centerY = hitBox.y + hitBox.h / 2.0f;
	// 加上一定的偏移量，确保在玩家站在可攀爬瓦片边缘时也能正确检测到
	const float offset = 5.0f; // 偏移容差
	const int colStart = static_cast<int>((centerX - offset) / Config::TILE_SIZE);
	const int colEnd = static_cast<int>((centerX + offset) / Config::TILE_SIZE);
	const int rowStart = static_cast<int>((centerY - offset) / Config::TILE_SIZE);
	const int rowEnd = static_cast<int>((centerY + offset) / Config::TILE_SIZE);

	for (int row = rowStart; row <= rowEnd; ++row) {
		for (int col = colStart; col <= colEnd; ++col) {
			if (row < 0 || row >= static_cast<int>(m_tiles.size()) || col < 0 || col >= static_cast<int>(m_tiles[row].size())) {
				continue;
			}

			if (static_cast<CollisionType>(m_tiles[row][col].collision) == CollisionType::CLIMBABLE) {
				return true;
			}
		}
	}

	return false;
}

bool TileMap::isInDeadArea(const SDL_FRect& hitBox) const noexcept{
	int colStart = static_cast<int>(hitBox.x / Config::TILE_SIZE);
	int colEnd = static_cast<int>((hitBox.x + hitBox.w) / Config::TILE_SIZE);
	int rowStart = static_cast<int>(hitBox.y / Config::TILE_SIZE);
	int rowEnd = static_cast<int>((hitBox.y + hitBox.h) / Config::TILE_SIZE);
	for(int row = rowStart; row <= rowEnd; ++row) {
		for(int col = colStart; col <= colEnd; ++col) {
			if (row < 0 || row >= static_cast<int>(m_tiles.size()) || col < 0 || col >= static_cast<int>(m_tiles[row].size())) {
				continue;
			}
			if (static_cast<CollisionType>(m_tiles[row][col].collision) == CollisionType::DEAD) {
				return true;
			}
		}
	}

	return false;
}

bool TileMap::isInTriggerArea(const SDL_FRect& hitBox) const noexcept{
	int colStart = static_cast<int>(hitBox.x / Config::TILE_SIZE);
	int colEnd = static_cast<int>((hitBox.x + hitBox.w) / Config::TILE_SIZE);
	int rowStart = static_cast<int>(hitBox.y / Config::TILE_SIZE);
	int rowEnd = static_cast<int>((hitBox.y + hitBox.h) / Config::TILE_SIZE);
	for (int row = rowStart; row <= rowEnd; ++row) {
		for (int col = colStart; col <= colEnd; ++col) {
			if (row < 0 || row >= static_cast<int>(m_tiles.size()) || col < 0 || col >= static_cast<int>(m_tiles[row].size())) {
				continue;
			}
			if (static_cast<CollisionType>(m_tiles[row][col].collision) == CollisionType::HITED) {
				return true;
			}
		}
	}
	return false;
}

void TileMap::cleanUpActiveObject() noexcept{
	std::size_t index = 0;
	while (index < m_mapObjects.size()) {
		if (m_mapObjects[index].isAcitive) {
			if (index != m_mapObjects.size() - 1) {
				m_mapObjects[index] = std::move(m_mapObjects.back());
			}
			m_mapObjects.pop_back();
		}
		else {
			++index;
		}
	}
}

void TileMap::addAnimationFrame(size_t i, size_t j, const std::vector<std::pair<uint16_t, uint32_t>>& frames) noexcept{
	size_t row = j / Config::TILE_SIZE;
	size_t col = i / Config::TILE_SIZE;
	size_t line = m_tiles[0].size();
	//m_animationFrames[col * line + row] = std::make_pair(frames, Timer());

	if (i < m_tiles.size() && j < m_tiles[i].size()) { 
		//前八位为x坐标, 后八位为y坐标
		m_tiles[i][j].srcX = (frames.front().second & 0xFF) * Config::TILE_SRC_WIDTH; 
		m_tiles[i][j].srcY = ((frames.front().second >> 8) & 0xFF) * Config::TILE_SRC_HEIGHT; 
	}
}

void TileMap::updateTileAnimations(double dt) noexcept{
	for(auto& [index, animData] : m_animationFrames) {
		animData.second.update(dt); // 更新定时器状态
		if(!animData.second.isActive()) {
			continue; // 如果动画已经结束或者未激活，跳过更新
		}
		size_t currentFrameIndex = 0;
		double accumulatedTime = 0.0;
		for(size_t k = 0; k < animData.first.size(); ++k) {
			accumulatedTime += animData.first[k].second / 1000.0; // 将持续时间从毫秒转换为秒
			if(accumulatedTime > animData.second.getElapsedTime()) {
				currentFrameIndex = k;
				break;
			}
		}

		size_t col = m_tiles[0].size();
		size_t i = index / col;
		size_t j = index % col;
		if (i < m_tiles.size() && j < m_tiles[i].size()) { //前八位为x坐标, 后八位为y坐标
			m_tiles[i][j].srcX = (animData.first[currentFrameIndex].first & 0xFF) * Config::TILE_SRC_WIDTH; // 获取当前帧的源矩形X坐标
			m_tiles[i][j].srcY = ((animData.first[currentFrameIndex].first >> 8) & 0xFF) * Config::TILE_SRC_HEIGHT; // 获取当前帧的源矩形Y坐标
			// 这里可以根据需要添加更多的动画属性更新，例如碰撞属性等
		}
	}
}

