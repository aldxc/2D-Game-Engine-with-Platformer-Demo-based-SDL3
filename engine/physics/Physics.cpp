#include <algorithm>
#include <cmath>
#include "Physics.h"

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

void Physics::update(float dt) noexcept{
	for (RigidBody& body : rigidBodies_) {
		// 只有在空中时才应用重力，着陆后重力不再影响速度，保持玩家在地面上的稳定状态
		body.velocity.setY(body.velocity.getY() + (body.isLanded ? 0 : gravity_ * body.gravityScale * dt) + body.acceleration.getY() * dt); // 速度积分
		body.velocity.setX(body.velocity.getX() + body.acceleration.getX() * dt); // 速度积分

		body.velocity.setX(std::clamp(body.velocity.getX(), -body.maxSpeed, body.maxSpeed)); // 限速
		body.velocity.setY(std::clamp(body.velocity.getY(), -body.maxSpeed, body.maxSpeed)); // 限速

		// 重置加速度，等待下一帧根据输入重新设置
		body.acceleration.set(0, 0);
	}
}

void Physics::resolveCollisions(const std::vector<std::vector<physicalCollMap>>& collmap, float dt, float epsilon) noexcept{
	if(collmap.empty() || collmap[0].empty()) {
		SDL_Log("Warning: Collision map is empty, skipping collision resolution.");
		return; // 碰撞地图无效，直接返回
	}

	for (RigidBody& body : rigidBodies_) {
		// 这里可以添加物体之间的碰撞检测和响应逻辑，例如玩家与地图的碰撞检测等
		const float tileSize = collmap[0][0].size; // 瓦片大小,所有瓦片大小相同
		RigidBody nextState = body; // 计算物体的下一个状态，初始为当前状态，根据速度积分进行预测

		auto isInBounds = [&collmap](int row, int col) noexcept -> bool {
			return row >= 0 && row < static_cast<int>(collmap.size()) && col >= 0 && col < static_cast<int>(collmap[0].size());
			};

		auto getTileRect = [tileSize](int row, int col) noexcept -> SDL_FRect {
			return SDL_FRect{
				col * tileSize,
				row * tileSize,
				tileSize,
				tileSize
			};
			};

		// 先处理 X 轴：只处理 FULL，HALF 不参与水平阻挡 full - 2, half - 1, none - 0
		nextState.hitBox.setX(nextState.hitBox.getX() + nextState.velocity.getX() * dt);

		int rowStart = static_cast<int>(nextState.hitBox.getY() / tileSize);
		int rowEnd = static_cast<int>((nextState.hitBox.getY() + nextState.hitBox.getH() - epsilon) / tileSize);
		int colStart = static_cast<int>(nextState.hitBox.getX() / tileSize);
		int colEnd = static_cast<int>((nextState.hitBox.getX() + nextState.hitBox.getW() - epsilon) / tileSize);

		for (int row = rowStart; row <= rowEnd; ++row) {
			for (int col = colStart; col <= colEnd; ++col) {
				if (!isInBounds(row, col)) {
					continue;
				}

				if (collmap[row][col].coll != 2) { 
					continue;
				}

				const SDL_FRect tileRect = getTileRect(row, col);
				if (!isColliding(nextState.hitBox, tileRect)) {
					continue;
				}

				if (nextState.velocity.getX() > 0.0f) {
					nextState.hitBox.setX(tileRect.x - nextState.hitBox.getW());
				}
				else if (nextState.velocity.getX() < 0.0f) {
					nextState.hitBox.setX(tileRect.x + tileRect.w);
				}

				nextState.velocity.setX(0.0f);
			}
		}

		// 再处理 Y 轴：FULL 正常阻挡，HALF 仅在下落且从上方穿过平台顶面时生效
		const float previousTop = nextState.hitBox.getY();
		const float previousBottom = previousTop + nextState.hitBox.getH();

		nextState.hitBox.setY(previousTop + nextState.velocity.getY() * dt);
		nextState.isLanded = false;

		const float currentTop = nextState.hitBox.getY();
		const float currentBottom = currentTop + nextState.hitBox.getH();

		const float sweepTop = std::min(previousTop, currentTop);
		const float sweepBottom = std::max(previousBottom, currentBottom);

		rowStart = static_cast<int>(std::floor((sweepTop - epsilon) / tileSize));
		rowEnd = static_cast<int>(std::floor((sweepBottom + epsilon) / tileSize));
		colStart = static_cast<int>(std::floor(nextState.hitBox.getX() / tileSize));
		colEnd = static_cast<int>(std::floor((nextState.hitBox.getX() + nextState.hitBox.getW() - epsilon) / tileSize));

		for (int row = rowStart; row <= rowEnd; ++row) {
			for (int col = colStart; col <= colEnd; ++col) {
				if (!isInBounds(row, col)) {
					continue;
				}

				const uint8_t collision = collmap[row][col].coll;
				const SDL_FRect tileRect = getTileRect(row, col);

				const bool overlapX =
					nextState.hitBox.getX() < tileRect.x + tileRect.w &&
					nextState.hitBox.getX() + nextState.hitBox.getW() > tileRect.x;

				if (collision == 2) {
					const bool touchingOrCrossingTop =
						nextState.velocity.getY() >= 0.0f &&
						overlapX &&
						previousBottom <= tileRect.y + epsilon &&
						currentBottom >= tileRect.y - epsilon;

					if (touchingOrCrossingTop) {
						nextState.hitBox.setY(tileRect.y - nextState.hitBox.getH());
						nextState.velocity.setY(0.0f);
						nextState.isLanded = true;
						continue;
					}

					if (!Physics::isColliding(nextState.hitBox, tileRect)) {
						continue;
					}

					if (nextState.velocity.getY() > 0.0f) {
						nextState.hitBox.setY(tileRect.y - nextState.hitBox.getH());
						nextState.velocity.setY(0.0f);
						nextState.isLanded = true;
					}
					else if (nextState.velocity.getY() < 0.0f) {
						nextState.hitBox.setY(tileRect.y + tileRect.h);
						nextState.velocity.setY(0.0f);
					}
				}
				else if (collision == 1) {
					if (body.wantsDropDown) {
						continue;
					}

					const bool touchingOrCrossingPlatformTop =
						nextState.velocity.getY() >= 0.0f &&
						overlapX &&
						previousBottom <= tileRect.y + epsilon &&
						currentBottom >= tileRect.y - epsilon; // 允许一定的误差，确保在玩家站在平台边缘时也能正确检测到碰撞

					if (!touchingOrCrossingPlatformTop) {
						continue;
					}

					nextState.hitBox.setY(tileRect.y - nextState.hitBox.getH());
					nextState.velocity.setY(0.0f);
					nextState.isLanded = true;
				}
			}
		}

		// 边界处理：如果玩家碰到地图边界，强制调整位置和速度
		if (nextState.hitBox.getX() < 0.0f) {
			nextState.hitBox.setX(0.0f);
			nextState.velocity.setX(0.0f);
		}
		else if (nextState.hitBox.getX() + nextState.hitBox.getW() > collmap[0].size() * tileSize) {
			nextState.hitBox.setX(collmap[0].size() * tileSize - nextState.hitBox.getW());
			nextState.velocity.setX(0.0f);
		}

		nextState.wantsDropDown = false; // 每帧重置希望下落标志，等待下一帧根据输入重新设置

		body = nextState; // 更新物体状态
	}
}