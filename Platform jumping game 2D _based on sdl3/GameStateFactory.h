#pragma once
#include <memory>
#include "core/State.h"
#include "StateType.h"
#include "MenuState.h"
#include "PlayingState.h"
#include "WonState.h"
#include "LoseState.h"
#include "SelectState.h"
#include "PauseState.h"

struct RenderContext;
class EventManager;
class Physics;
class Input;
class Resource;
class GameSession;

struct GameStateFactory {
	static std::unique_ptr<State<StateType>> create(StateType stateType, RenderContext& renderContext, Physics& pM, Input& iM, EventManager& eM, Resource& rM, GameSession& gS) noexcept {
		switch (stateType) {
		case StateType::MENU:
			return std::make_unique<MenuState>(renderContext.renderer);
		case StateType::PLAYING:
			return std::make_unique<PlayingState>(renderContext, pM, iM, eM, rM, gS);
		case StateType::PAUSE:
			return std::make_unique<PauseState>(renderContext.renderer);
		case StateType::WON:
			return std::make_unique<WonState>(renderContext.renderer);
		case StateType::LOSE:
			return std::make_unique<LoseState>(renderContext.renderer);//
		case StateType::SELECT_HERO:
			return std::make_unique<SelectState>(renderContext.renderer, rM, iM); 
		default:
			return std::make_unique<MenuState>(renderContext.renderer);
		}
	}
};