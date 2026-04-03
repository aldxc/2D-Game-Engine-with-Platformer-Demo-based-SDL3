#pragma once
#include <memory>
#include "core/UI.h"
#include "UIType.h"
#include "MenuUI.h"
#include "PlayingUI.h"

struct GameUIFactory {
	static std::unique_ptr<UI<UIType>> create(UIType type) {
		switch (type) {
		case UIType::MENU:
			return std::make_unique<MenuUI>();
		case UIType::PLAYING:
			return std::make_unique<PlayingUI>();
		case UIType::PAUSE:
		case UIType::WON:
		case UIType::LOST:
		default:
			return std::make_unique<MenuUI>();
		}
	}
};