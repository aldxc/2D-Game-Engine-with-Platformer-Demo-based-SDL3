#include "TileMap.h"
#include "Config.h"
#include "render/Renderer.h"
#include "resource/Resource.h"

void TileMap::update(float dt) noexcept {
	// 这里可以添加地图更新逻辑，例如动画、动态元素等
}

void TileMap::render() const noexcept {

	auto texture = Resource::getInstance().loadTexture("resource/sheet.png", Renderer::getInstance().getSDLRenderer());
	const auto& bkrect = Config::BK_SRC;
	SDL_FRect bksrcRect = { bkrect[0], bkrect[1], bkrect[2], bkrect[3] };
	Renderer::getInstance().renderTexture(texture.get(), bksrcRect, SDL_FRect{ 0,0,Config::LOGIC_WIDTH, Config::LOGIC_HEIGHT });

	if(tiles_.empty() || tiles_[0].empty()) {
		return; // 如果地图数据为空，直接返回
	}
	
	for (int i = 0; i < tiles_.size(); ++i) {
		for (int j = 0; j < tiles_[0].size(); ++j) {
			if (tiles_[i][j].type == TileType::LAND) {
				const auto& rect = Config::TILE_LAND_SRC[tileTypeToTextureIndex_[i][j]];
				SDL_FRect srcRect = { rect[0], rect[1], rect[2], rect[3] };
				Renderer::getInstance().renderTexture(texture.get(), srcRect, SDL_FRect{ float(j * 32), float(i * 32), 32, 32 });
			}
		}
	}
}

TileMap::CollisionResult TileMap::tileCollision(const SDL_FRect& srcBox, float velocityX, float velocityY, bool isLanded, float dt) const noexcept {
	constexpr float tileSize = Config::TILE_SIZE;
	constexpr float epsilon = Config::EPSILON;

	CollisionResult result{
		srcBox,
		velocityX,
		velocityY,
		false
	};

	if (tiles_.empty() || tiles_[0].empty()) {
		result.isLanded = isLanded;
		return result;
	}

	auto isInBounds = [this](int row, int col) noexcept -> bool {
		return row >= 0 && row < static_cast<int>(tiles_.size()) && col >= 0 && col < static_cast<int>(tiles_[0].size());
	};

	auto getTileRect = [tileSize](int row, int col) noexcept -> SDL_FRect {
		return SDL_FRect{
			col * tileSize,
			row * tileSize,
			tileSize,
			tileSize
		};
	};

	// 先处理 X 轴：只处理 FULL，HALF 不参与水平阻挡
	result.hitbox.x += result.velocityX * dt;

	int rowStart = static_cast<int>(result.hitbox.y / tileSize);
	int rowEnd = static_cast<int>((result.hitbox.y + result.hitbox.h - epsilon) / tileSize);
	int colStart = static_cast<int>(result.hitbox.x / tileSize);
	int colEnd = static_cast<int>((result.hitbox.x + result.hitbox.w - epsilon) / tileSize);

	for (int row = rowStart; row <= rowEnd; ++row) {
		for (int col = colStart; col <= colEnd; ++col) {
			if (!isInBounds(row, col)) {
				continue;
			}

			if (tiles_[row][col].collision != CollisionType::FULL) {
				continue;
			}

			const SDL_FRect tileRect = getTileRect(row, col);
			if (!SDL_HasRectIntersectionFloat(&result.hitbox, &tileRect)) {
				continue;
			}

			if (result.velocityX > 0.0f) {
				result.hitbox.x = tileRect.x - result.hitbox.w;
			} else if (result.velocityX < 0.0f) {
				result.hitbox.x = tileRect.x + tileRect.w;
			}

			result.velocityX = 0.0f;
		}
	}

	// 再处理 Y 轴：FULL 正常阻挡，HALF 仅在下落且从上方穿过平台顶面时生效
	const float previousBottom = result.hitbox.y + result.hitbox.h;
	result.hitbox.y += result.velocityY * dt;
	result.isLanded = false;

	rowStart = static_cast<int>(result.hitbox.y / tileSize);
	rowEnd = static_cast<int>((result.hitbox.y + result.hitbox.h - epsilon) / tileSize);
	colStart = static_cast<int>(result.hitbox.x / tileSize);
	colEnd = static_cast<int>((result.hitbox.x + result.hitbox.w - epsilon) / tileSize);

	for (int row = rowStart; row <= rowEnd; ++row) {
		for (int col = colStart; col <= colEnd; ++col) {
			if (!isInBounds(row, col)) {
				continue;
			}

			const CollisionType collision = tiles_[row][col].collision;
			const SDL_FRect tileRect = getTileRect(row, col);

			if (collision == CollisionType::FULL) {
				if (!SDL_HasRectIntersectionFloat(&result.hitbox, &tileRect)) {
					continue;
				}

				if (result.velocityY > 0.0f) {
					result.hitbox.y = tileRect.y - result.hitbox.h;
					result.isLanded = true;
				} else if (result.velocityY < 0.0f) {
					result.hitbox.y = tileRect.y + tileRect.h;
				}

				result.velocityY = 0.0f;
			} else if (collision == CollisionType::HALF) {
				if (result.velocityY <= 0.0f) {
					continue;
				}

				const bool overlapX = result.hitbox.x < tileRect.x + tileRect.w && result.hitbox.x + result.hitbox.w > tileRect.x;

				const float currentBottom = result.hitbox.y + result.hitbox.h;
				const bool crossedPlatformTop = previousBottom <= tileRect.y + epsilon && currentBottom >= tileRect.y;

				if (!overlapX || !crossedPlatformTop) {
					continue;
				}

				result.hitbox.y = tileRect.y - result.hitbox.h;
				result.velocityY = 0.0f;
				result.isLanded = true;
			}
		}
	}

	return result;
}

void TileMap::calculateTileTypeToTextureIndexMapping() noexcept{
	auto isLandTile = [&](int row, int col) {
		if (row < 0 || row >= static_cast<int>(tiles_.size())) {
			return false;
		}
		if (col < 0 || col >= static_cast<int>(tiles_[row].size())) {
			return false;
		}
		return tiles_[row][col].type == TileType::LAND;
		};

	const int rows = tiles_.size(), cols = tiles_.empty() ? 0 : tiles_[0].size();
	tileTypeToTextureIndex_.assign(rows, std::vector<uint8_t>(cols, 0));
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			//根据瓦片位置周围的瓦片类型计算出当前瓦片的纹理索引
			//land
			const bool left = isLandTile(i, j - 1); // true 表示左侧是陆地瓦片，false 表示左侧不是陆地瓦片
			const bool right = isLandTile(i, j + 1); // true 表示右侧是陆地瓦片，false 表示右侧不是陆地瓦片
			const bool up = isLandTile(i - 1, j); // true 表示上方是陆地瓦片，false 表示上方不是陆地瓦片
			if (!up) {
				if (!left && right) {
					tileTypeToTextureIndex_[i][j] = 0;
				}else if (left && !right) {
					tileTypeToTextureIndex_[i][j] = 2;
				}
				else if (left && right) {
					tileTypeToTextureIndex_[i][j] = 1;
				}
				else if (!left && !right) {
					tileTypeToTextureIndex_[i][j] = 6;
				}
			} else {
				if (!left && right) {
					tileTypeToTextureIndex_[i][j] = 3;
				}else if (left && !right) {
					tileTypeToTextureIndex_[i][j] = 5;
				}else if(left && right) {
					tileTypeToTextureIndex_[i][j] = 4;
				}else if (!left && !right) {
					tileTypeToTextureIndex_[i][j] = 7;
				}
			}
		}
	}
}

