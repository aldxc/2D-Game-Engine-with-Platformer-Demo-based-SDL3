#pragma once
#include <memory>
#include "core/State.h"
#include "StateType.h"
#include "MenuState.h"
#include "PlayingState.h"
#include "WonState.h"
#include "LoseState.h"

struct RenderContext;
class EventManager;
class Physics;
class Input;
class Resource;

struct GameStateFactory {
	static std::unique_ptr<State<StateType>> create(StateType stateType, RenderContext& renderContext, Physics& pM, Input& iM, EventManager& eM, Resource& rM) noexcept {
		switch (stateType) {
		case StateType::MENU:
			return std::make_unique<MenuState>(renderContext.renderer);
		case StateType::PLAYING:
			return std::make_unique<PlayingState>(renderContext, pM, iM, eM, rM);
		case StateType::PAUSE:
			return std::make_unique<MenuState>(renderContext.renderer);//暂时用菜单状态来表示暂停状态，后续增加专门的暂停状态
		case StateType::WON:
			return std::make_unique<WonState>(renderContext.renderer);
		case StateType::LOSE:
			return std::make_unique<LoseState>(renderContext.renderer);//
		default:
			return std::make_unique<MenuState>(renderContext.renderer);
		}
	}
};