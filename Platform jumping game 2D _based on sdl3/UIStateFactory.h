#pragma once
#include <memory>
#include "core/UI.h"
#include "UIType.h"
#include "MenuUI.h"
#include "PlayingUI.h"
#include "WonUI.h"
#include "LoseUI.h"
#include "SelectUI.h"
#include "PauseUI.h"

class Renderer;
class EventManager;
class Input;
class GameSession;

struct GameUIFactory {
	static std::unique_ptr<UI<UIType>> create(UIType type, Input& iM, EventManager& eM, Renderer& r, GameSession& gS) noexcept {
		switch (type) {
		case UIType::MENU:
			return std::make_unique<MenuUI>(iM, eM, r, gS);
		case UIType::PLAYING:
			return std::make_unique<PlayingUI>(iM, eM, r);
		case UIType::PAUSE:
			return std::make_unique<PauseUI>(iM, eM, r); 
		case UIType::WON:
			return std::make_unique<WonUI>(iM, eM, r);
		case UIType::LOSE:
			return std::make_unique<LoseUI>(iM, eM, r);
		case UIType::SelectState:
			return std::make_unique<SelectUI>(iM, eM, r, gS);
		default:
			return std::make_unique<MenuUI>(iM, eM, r, gS);
		}
	}
};