#pragma once
#include <memory>
#include "core/UI.h"
#include "UIType.h"
#include "MenuUI.h"
#include "PlayingUI.h"
#include "WonUI.h"
#include "LoseUI.h"

class Renderer;
class EventManager;
class Input;

struct GameUIFactory {
	static std::unique_ptr<UI<UIType>> create(UIType type, Input& iM, EventManager& eM, Renderer& r) noexcept {
		switch (type) {
		case UIType::MENU:
			return std::make_unique<MenuUI>(iM, eM, r);
		case UIType::PLAYING:
			return std::make_unique<PlayingUI>(iM, eM, r);
		case UIType::PAUSE:
		case UIType::WON:
			return std::make_unique<WonUI>(iM, eM, r);
		case UIType::LOSE:
			return std::make_unique<LoseUI>(iM, eM, r);
		default:
			return std::make_unique<MenuUI>(iM, eM, r);
		}
	}
};