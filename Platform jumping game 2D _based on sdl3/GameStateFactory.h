#pragma once
#include <memory>
#include "core/State.h"
#include "StateType.h"
#include "MenuState.h"
#include "PlayingState.h"
#include "WonState.h"
#include "LoseState.h"

class Context;

struct GameStateFactory {
	static std::unique_ptr<State<StateType>> create(StateType stateType, Context& context) {
		switch (stateType) {
		case StateType::MENU:
			return std::make_unique<MenuState>();
		case StateType::PLAYING:
			return std::make_unique<PlayingState>(context);
		case StateType::PAUSE:
			return std::make_unique<MenuState>();//暂时用菜单状态来表示暂停状态，后续增加专门的暂停状态
		case StateType::WON:
			return std::make_unique<WonState>();
		case StateType::LOSE:
			return std::make_unique<LoseState>();//
		default:
			return std::make_unique<MenuState>();
		}
	}
};