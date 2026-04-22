#pragma once
#include <memory>
#include <string>
#include "core/State.h"
#include "core/SubscriptionId.h"
#include "core/Timer.h"
#include "core/Vec2.h"
#include "StateType.h"
#include "AudioId.h"

struct RenderContext;
class EventManager;
class Physics;
class Input;
class Resource;
class GameSession;
class Player;
class TileMap;
class EnemyManager;

class PlayingState : public State<StateType> {
public:
	explicit PlayingState(RenderContext& renderContext, Physics& pM, Input& iM, EventManager& eM, Resource& rM, GameSession& gS) noexcept;
	~PlayingState() noexcept override;

	void render() const noexcept override final;
	void update(double dt) noexcept override final;
	void dropDead() noexcept;

private:
	// 从文件加载关卡数据
	void loadLevel(const std::string& filePath);

	// 处理碰撞检测和响应
	void resolveInteractions();

	// 处理音效播放逻辑
	void resolveSfx(std::vector<SfxId>& sfxs);

private:
	// 外部依赖
	RenderContext& renderContext_;
	EventManager& eventManager_;
	Physics& physics_;
	Input& inputManager_;
	Resource& resourceManager_;
	GameSession& gameSession_;

	// 场景对象
	// 暂时由 PlayingState 直接维护
	// 后续可以抽象世界对象统一管理
	std::unique_ptr<Player> player_;
	std::unique_ptr<TileMap> tileMap_;

	// 敌人管理器，负责敌人的创建、更新和渲染
	// 后续可以增加更完整的生成与销毁逻辑
	std::unique_ptr<EnemyManager> enemyManager_;

	// 关卡状态
	// 当前关卡编号
	size_t currentLevel_ = 0;

	// 调试状态
	bool showPlayerDebugInfo_ = false;
	SubscriptionId debugSubscriptionId_;

	// 胜利状态
	// 胜利条件位置
	Vec2 winPosition_ = Vec2(-100, 0);

	// 胜利计时器，用于胜利后延迟切换状态
	Timer winTimer_;
};
