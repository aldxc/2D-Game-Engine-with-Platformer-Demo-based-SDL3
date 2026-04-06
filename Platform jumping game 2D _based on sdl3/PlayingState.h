#pragma once
#include <memory>
#include "core/State.h"
#include "StateType.h"
#include "Player.h"
#include "TileMap.h"

class PlayingState : public State<StateType> {
public:
	explicit PlayingState(Animation& animation) noexcept;
	~PlayingState() noexcept override = default;
	void render() const noexcept override final;
	void update(float dt) noexcept override final;
private:
	void loadLevel(const std::string& filePath); // 从文件加载关卡数据，后续增加地图资源管理等功能
	//将玩家、视口、敌人、地图等暂时维护在这，后续添加世界对象来管理这些元素
	std::unique_ptr<Player> player_;
	std::unique_ptr<TileMap> tileMap_;
	int currentLevel_ = 0; // 当前关卡编号，后续增加关卡管理等功能
};
