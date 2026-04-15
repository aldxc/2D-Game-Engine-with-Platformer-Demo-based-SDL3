#pragma once
#include <memory>
#include "core/State.h"
#include "core/SubscriptionId.h"
#include "core/Timer.h" // test
#include "StateType.h"
#include "Player.h"
#include "TileMap.h"
#include "Enemy.h"
#include "EnemyManager.h"

struct RenderContext;
class EventManager;
class Physics;
class Input;
class Resource;

class PlayingState : public State<StateType> {
public:
	explicit PlayingState(RenderContext& renderContext, Physics& pM, Input& iM, EventManager& eM, Resource& rM) noexcept;
	~PlayingState() noexcept override;
	void render() const noexcept override final;
	void update(float dt) noexcept override final;
private:
	void loadLevel(const std::string& filePath); // 从文件加载关卡数据，后续增加地图资源管理等功能
	void resolveInteractions(); // 处理碰撞检测和响应，后续增加更复杂的碰撞处理等功能
private:
	RenderContext& renderContext_; // 状态上下文，存储全局共享的数据和资源
	EventManager& eventManager_; // 事件管理器引用，供状态内的事件相关操作使用
	Physics& physics_; // 物理引擎引用，供状态内的物理相关操作使用
	Input& inputManager_; // 输入管理器引用，供状态内的输入相关操作使用
	Resource& resourceManager_; // 资源管理器引用，供状态内的资源相关操作使用

	//将玩家、视口、敌人、地图等暂时维护在这，后续添加世界对象来管理这些元素
	std::unique_ptr<Player> player_;
	std::unique_ptr<TileMap> tileMap_;
	std::unique_ptr<EnemyManager> enemyManager_; // 敌人管理器，负责管理敌人的创建、更新、渲染等，后续增加敌人生成、销毁等功能

	int currentLevel_ = 0; // 当前关卡编号，后续增加关卡管理等功能

	//调试信息显示
	bool showPlayerDebugInfo_ = true;
	SubscriptionId debugSubscriptionId_;

};
