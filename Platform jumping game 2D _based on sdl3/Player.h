#pragma once
#include "Object.h"

class Player : public Object {
public:
	struct PlayerCommand {
		bool moveLeft = false;
		bool moveRight = false;
		bool jump = false;
		//后续增加更多命令，如攻击、特殊技能等
		PlayerCommand() = default;
	};
	explicit Player() = default;
	~Player() override = default;

	void setCommand(const PlayerCommand& command) noexcept { command_ = command; } // 设置玩家命令接口
	void update(float dt) noexcept override;
	void render() const noexcept override;
private:
	// 玩家特有的属性，如速度、跳跃状态等
	float speed_ = 200.0f; // 玩家移动速度
	float velocityX_ = 0.0f; // 玩家水平速度
	float velocityY_ = 0.0f; // 玩家垂直速度
	bool facingRight_ = true; // 玩家朝向
	bool isLanded_ = false; // 玩家是否着陆
	bool isJumping_ = false; // 玩家是否正在跳跃

	PlayerCommand command_ = {}; // 当前玩家命令
};