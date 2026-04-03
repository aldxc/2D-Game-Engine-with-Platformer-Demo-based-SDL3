#pragma once
#pragma once
#include <memory>
#include "core/State.h"
#include "StateType.h"
#include "MenuState.h"
#include "PlayingState.h"



struct GameStateFactory {
	static std::unique_ptr<State<StateType>> create(StateType stateType) {
		switch (stateType) {
		case StateType::MENU:
			return std::make_unique<MenuState>();
		case StateType::PLAYING:
			return std::make_unique<PlayingState>();
		case StateType::PAUSE:
		case StateType::WON:
		case StateType::LOST:
		default:
			return std::make_unique<MenuState>();
		}
	}
};