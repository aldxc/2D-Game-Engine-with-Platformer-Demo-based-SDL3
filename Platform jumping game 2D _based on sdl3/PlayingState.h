#pragma once
#include <memory>
#include "core/State.h"
#include "StateType.h"
#include "Player.h"

class PlayingState : public State<StateType> {
public:
	PlayingState() noexcept;
	~PlayingState() noexcept override = default;
	void render() const noexcept override final;
	void update(float dt) noexcept override final;
private:
	//将玩家、视口、敌人、地图等暂时维护在这，后续添加世界对象来管理这些元素
	std::unique_ptr<Player> player_;

};
