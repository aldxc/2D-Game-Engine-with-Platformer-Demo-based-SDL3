#include "PlayingState.h"
#include "render/Renderer.h"
#include "input/Input.h"

PlayingState::PlayingState() noexcept : State<StateType>(StateType::PLAYING) {
	player_ = std::make_unique<Player>();//状态开始时创建玩家实例，后续增加地图、敌人等元素的创建
}

void PlayingState::render() const noexcept{
	player_->render();
}

void PlayingState::update(float dt) noexcept{

	player_->setCommand(Player::PlayerCommand{
		Input::getInstance().getMoveLeftPressed(),
		Input::getInstance().getMoveRightPressed(),
		Input::getInstance().getJumpPressed()
		});

	player_->update(dt);
}
