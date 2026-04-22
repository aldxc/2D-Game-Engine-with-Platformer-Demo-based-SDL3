#include <algorithm>
#include <cmath>
#include "Physics.h"
#include "PhysicsCollMap.h"

enum class TileColl : uint8_t { NONE = 0, HALF = 1, FULL = 2, CLIMBABLE = 3 };

bool Physics::init(float gravity) noexcept{
	gravity_ = gravity;

	return true;
}

void Physics::registerRigidBody(RigidBody& body) noexcept{
	auto it = std::find_if(rigidBodies_.begin(), rigidBodies_.end(), [&body](const std::reference_wrapper<RigidBody>& rb) {
		return &rb.get() == &body;
	});
	if(it != rigidBodies_.end()) {
		return; // 已经注册过了，直接返回
	}
	rigidBodies_.push_back(body);
}

void Physics::unregisterRigidBody(RigidBody& body) noexcept{
	auto it = std::find_if(rigidBodies_.begin(), rigidBodies_.end(), [&body](const std::reference_wrapper<RigidBody>& rb) {
		return &rb.get() == &body;
	});
	if (it != rigidBodies_.end()) {
		rigidBodies_.erase(it);
	}
}

void Physics::update(double dt) noexcept{
	for (RigidBody& body : rigidBodies_) {
		// 只有在空中时才应用重力，着陆后重力不再影响速度，保持玩家在地面上的稳定状态
		// 速度积分
		body.velocity.setY(body.velocity.y() + (body.isLanded ? 0 : gravity_ * body.gravityScale * dt) + body.acceleration.y() * dt); 
		body.velocity.setX(body.velocity.x() + body.acceleration.x() * dt); 

		// 速度限制，防止物体移动过快导致穿透问题
		body.velocity.setX(std::clamp(body.velocity.x(), -body.maxSpeed, body.maxSpeed)); 
		body.velocity.setY(std::clamp(body.velocity.y(), -body.maxSpeed, body.maxFallSpeed));

		// 重置加速度，等待下一帧根据输入重新设置
		body.acceleration.set(0, 0);
	}
}

void Physics::resolveCollisions(const std::vector<std::vector<physicalCollMap>>& collmap, double dt, float epsilon) noexcept{
	if(collmap.empty() || collmap[0].empty()) {
		SDL_Log("Warning: Collision map is empty, skipping collision resolution.");
		// 碰撞地图无效，直接返回
		return; 
	}

	for (RigidBody& body : rigidBodies_) {
		// 这里可以添加物体之间的碰撞检测和响应逻辑，例如玩家与地图的碰撞检测等
		const float tileSize = collmap[0][0].size; 
		// 计算物体的下一个状态，初始为当前状态，根据速度积分进行预测
		RigidBody nextState = body;

		// 边界检查函数，确保访问collmap时不会越界
		auto isInBounds = [&collmap](int32_t row, int32_t col) noexcept -> bool {
			return row >= 0 && row < static_cast<int32_t>(collmap.size()) && col >= 0 && col < static_cast<int32_t>(collmap[0].size());
			};

		// 获取瓦片矩形的函数，根据行列索引计算瓦片在世界坐标中的位置和大小
		auto getTileRect = [tileSize](int32_t row, int32_t col) noexcept -> Rect {
			return Rect{
				col * tileSize,
				row * tileSize,
				tileSize,
				tileSize
			};
			};

		// 先处理 X 轴：只处理 FULL，HALF 不参与水平阻挡 full - 2, half - 1, none - 0
		nextState.hitBox.setX(nextState.hitBox.x() + nextState.velocity.x() * dt);

		int32_t rowStart = static_cast<int32_t>(nextState.hitBox.y() / tileSize);
		int32_t rowEnd = static_cast<int32_t>((nextState.hitBox.y() + nextState.hitBox.h() - epsilon) / tileSize);
		int32_t colStart = static_cast<int32_t>(nextState.hitBox.x() / tileSize);
		int32_t colEnd = static_cast<int32_t>((nextState.hitBox.x() + nextState.hitBox.w() - epsilon) / tileSize);

		// 索引使用int32_t 防止越界是强转为size_t补码后变成一个很大的正数，导致访问collmap越界崩溃
		for (int32_t row = rowStart; row <= rowEnd; ++row) {
			for (int32_t col = colStart; col <= colEnd; ++col) {
				if (!isInBounds(row, col)) {
					continue;
				}

				if (static_cast<TileColl>(collmap[row][col].coll) != TileColl::FULL) { 
					continue;
				}

				const SDL_FRect tileRect = getTileRect(row, col);
				if (!isColliding(nextState.hitBox, tileRect)) {
					continue;
				}

				if (nextState.velocity.x() > 0.0f) {
					nextState.hitBox.setX(tileRect.x - nextState.hitBox.w());
				}
				else if (nextState.velocity.x() < 0.0f) {
					nextState.hitBox.setX(tileRect.x + tileRect.w);
				}

				nextState.velocity.setX(0.0f);
			}
		}

		// 再处理 Y 轴：FULL 正常阻挡，HALF 仅在下落且从上方穿过平台顶面时生效
		// 记录物体顶部底部的初始位置
		const float previousTop = nextState.hitBox.y(); 
		const float previousBottom = previousTop + nextState.hitBox.h(); 

		nextState.hitBox.setY(previousTop + nextState.velocity.y() * dt);
		nextState.isLanded = false;

		// 计算物体在垂直方向上的扫掠区域，考虑物体的移动范围，确保在高速移动时也能正确检测到碰撞
		const float currentTop = nextState.hitBox.y();
		const float currentBottom = currentTop + nextState.hitBox.h();

		// 扫掠区域的上下边界
		const float sweepTop = std::min(previousTop, currentTop);
		const float sweepBottom = std::max(previousBottom, currentBottom);

		rowStart = static_cast<int32_t>(std::floor((sweepTop - epsilon) / tileSize));
		rowEnd = static_cast<int32_t>(std::floor((sweepBottom + epsilon) / tileSize));
		colStart = static_cast<int32_t>(std::floor(nextState.hitBox.x() / tileSize));
		colEnd = static_cast<int32_t>(std::floor((nextState.hitBox.x() + nextState.hitBox.w() - epsilon) / tileSize));

		for (int32_t row = rowStart; row <= rowEnd; ++row) {
			for (int32_t col = colStart; col <= colEnd; ++col) {
				if (!isInBounds(row, col)) {
					continue;
				}

				const uint8_t collision = collmap[row][col].coll;
				const SDL_FRect tileRect = getTileRect(row, col);

				// 计算物体与瓦片在水平轴上的重叠情况，确保只有在水平上有重叠时才考虑垂直方向的碰撞响应
				const bool overlapX =
					nextState.hitBox.x() < tileRect.x + tileRect.w &&
					nextState.hitBox.x() + nextState.hitBox.w() > tileRect.x;

				if (static_cast<TileColl>(collision) == TileColl::FULL) {
					const bool touchingOrCrossingTop =
						nextState.velocity.y() >= 0.0f &&
						overlapX &&
						previousBottom <= tileRect.y + epsilon &&
						currentBottom >= tileRect.y - epsilon;

					if (touchingOrCrossingTop) {
						nextState.hitBox.setY(tileRect.y - nextState.hitBox.h());
						nextState.velocity.setY(0.0f);
						nextState.isLanded = true;
						continue;
					}

					if (!Physics::isColliding(nextState.hitBox, tileRect)) {
						continue;
					}

					if (nextState.velocity.y() > 0.0f) {
						nextState.hitBox.setY(tileRect.y - nextState.hitBox.h());
						nextState.velocity.setY(0.0f);
						nextState.isLanded = true;
					}
					else if (nextState.velocity.y() < 0.0f) {
						nextState.hitBox.setY(tileRect.y + tileRect.h);
						nextState.velocity.setY(0.0f);
					}
				}
				else if (static_cast<TileColl>(collision) == TileColl::HALF) {
					if (body.wantsDropDown) {
						continue;
					}

					// 只有当物体在下落且从上方穿过平台顶面时才考虑半碰撞平台的碰撞响应
					const bool touchingOrCrossingPlatformTop =
						nextState.velocity.y() >= 0.0f &&
						overlapX &&
						previousBottom <= tileRect.y + epsilon &&
						currentBottom >= tileRect.y - epsilon; 

					if (!touchingOrCrossingPlatformTop) {
						continue;
					}

					nextState.hitBox.setY(tileRect.y - nextState.hitBox.h());
					nextState.velocity.setY(0.0f);
					nextState.isLanded = true;
				}
			}
		}

		// 边界处理：如果玩家碰到地图边界，强制调整位置和速度
		if (nextState.hitBox.x() < 0.0f) {
			nextState.hitBox.setX(0.0f);
			nextState.velocity.setX(0.0f);
		}
		else if (nextState.hitBox.x() + nextState.hitBox.w() > collmap[0].size() * tileSize) {
			nextState.hitBox.setX(collmap[0].size() * tileSize - nextState.hitBox.w());
			nextState.velocity.setX(0.0f);
		}
		// y轴上边界处理
		if (nextState.hitBox.y() < 0.0f) {
			nextState.hitBox.setY(0.0f);
			nextState.velocity.setY(0.0f);
		}

		nextState.wantsDropDown = false; // 每帧重置希望下落标志，等待下一帧根据输入重新设置

		body = nextState; // 更新物体状态
	}
}

bool Physics::LineOfSight(const SDL_FRect& start, const SDL_FRect& end, const std::vector<std::vector<physicalCollMap>>& collmap) noexcept{
	// 射线检测
	if (collmap.empty() || collmap[0].empty()) return false;
	const float tileSize = static_cast<float>(collmap[0][0].size);
	const int32_t mapRows = static_cast<int32_t>(collmap.size());
	const int32_t mapCols = static_cast<int32_t>(collmap[0].size());

	// 起点终点的中心转为网格索引
	float sx = start.x + start.w * 0.5f;
	float sy = start.y + start.h * 0.5f;
	float ex = end.x + end.w * 0.5f;
	float ey = end.y + end.h * 0.5f;

	int32_t x0 = static_cast<int32_t>(sx / tileSize);
	int32_t y0 = static_cast<int32_t>(sy / tileSize);
	int32_t x1 = static_cast<int32_t>(ex / tileSize);
	int32_t y1 = static_cast<int32_t>(ey / tileSize);

	// 边界检查
	x0 = std::clamp(x0, 0, mapCols - 1);
	y0 = std::clamp(y0, 0, mapRows - 1);
	x1 = std::clamp(x1, 0, mapCols - 1);
	y1 = std::clamp(y1, 0, mapRows - 1);

	// 射线方向
	float dx = ex - sx;
	float dy = ey - sy;

	// 步进方向
	int32_t stepX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
	int32_t stepY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

	// tDelta
	float tDeltaX = (dx != 0) ? std::abs(tileSize / dx) : INFINITY;
	float tDeltaY = (dy != 0) ? std::abs(tileSize / dy) : INFINITY;

	// tMax — 到达第一个网格边界
	float tMaxX, tMaxY;
	if (dx > 0) {
		tMaxX = ((x0 + 1) * tileSize - sx) / dx;
	}
	else if (dx < 0) {
		tMaxX = (x0 * tileSize - sx) / dx;
	}
	else {
		tMaxX = INFINITY;
	}

	if (dy > 0) {
		tMaxY = ((y0 + 1) * tileSize - sy) / dy;
	}
	else if (dy < 0) {
		tMaxY = (y0 * tileSize - sy) / dy;
	}
	else {
		tMaxY = INFINITY;
	}

	// 步进
	int32_t x = x0, y = y0;
	int32_t maxSteps = mapRows + mapCols;  // 安全上限，防止死循环
	while (maxSteps-- > 0) {
		// 越界检查
		if (x < 0 || x >= mapCols || y < 0 || y >= mapRows) {
			return false;
		}

		// 检查当前格子（跳过起点）
		if (!(x == x0 && y == y0)) {
			if (static_cast<TileColl>(collmap[y][x].coll) == TileColl::FULL) {
				return false;  // 被墙阻挡
			}
		}

		// 到达终点
		if (x == x1 && y == y1) break;

		// 步进
		if (tMaxX < tMaxY) {
			tMaxX += tDeltaX;
			x += stepX;
		}
		else {
			tMaxY += tDeltaY;
			y += stepY;
		}
	}

	// 没有被墙阻挡
	return true; 
}

bool Physics::hasGroundAhead(const SDL_FRect& hitBox, bool facingRight, const std::vector<std::vector<physicalCollMap>>& collmap) noexcept{
	if (collmap.empty() || collmap[0].empty()) {
		return false;
	}

	const float tileSize = static_cast<float>(collmap[0][0].size);

	const float probeX = facingRight
		? (hitBox.x + hitBox.w + 2.0f)
		: (hitBox.x - 2.0f);
	const float probeY = hitBox.y + hitBox.h + 2.0f;

	const int col = static_cast<int>(std::floor(probeX / tileSize));
	const int row = static_cast<int>(std::floor(probeY / tileSize));

	if (row < 0 || row >= static_cast<int>(collmap.size()) ||
		col < 0 || col >= static_cast<int>(collmap[0].size())) {
		return false;
	}

	const auto coll = static_cast<TileColl>(collmap[row][col].coll);
	return coll == TileColl::FULL || coll == TileColl::HALF;
}

bool Physics::hasWallAhead(const SDL_FRect& hitBox, bool facingRight, const std::vector<std::vector<physicalCollMap>>& collmap) noexcept{
	if (collmap.empty() || collmap[0].empty()) {
		return false;
	}

	const float tileSize = static_cast<float>(collmap[0][0].size);

	const float probeX = facingRight
		? (hitBox.x + hitBox.w + 2.0f)
		: (hitBox.x - 2.0f);

	const float topY = hitBox.y + 2.0f;
	const float bottomY = hitBox.y + hitBox.h - 2.0f;

	const int col = static_cast<int>(std::floor(probeX / tileSize));
	const int rowStart = static_cast<int>(std::floor(topY / tileSize));
	const int rowEnd = static_cast<int>(std::floor(bottomY / tileSize));

	if (col < 0 || col >= static_cast<int>(collmap[0].size())) {
		return true;
	}

	for (int row = rowStart; row <= rowEnd; ++row) {
		if (row < 0 || row >= static_cast<int>(collmap.size())) {
			continue;
		}

		const auto coll = static_cast<TileColl>(collmap[row][col].coll);
		if (coll == TileColl::FULL) {
			return true;
		}
	}

	return false;
}
