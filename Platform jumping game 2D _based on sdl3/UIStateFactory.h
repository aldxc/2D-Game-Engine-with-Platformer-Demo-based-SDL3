#pragma once
#include <memory>
#include "core/UI.h"
#include "UIType.h"
#include "MenuUI.h"
#include "PlayingUI.h"
#include "WonUI.h"
#include "LoseUI.h"

struct GameUIFactory {
	static std::unique_ptr<UI<UIType>> create(UIType type) {
		switch (type) {
		case UIType::MENU:
			return std::make_unique<MenuUI>();
		case UIType::PLAYING:
			return std::make_unique<PlayingUI>();
		case UIType::PAUSE:
		case UIType::WON:
			return std::make_unique<WonUI>();
		case UIType::LOSE:
			return std::make_unique<LoseUI>();
		default:
			return std::make_unique<MenuUI>();
		}
	}
};