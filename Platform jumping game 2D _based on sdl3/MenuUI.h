#pragma once
#include <array>
#include "core/UI.h"
#include "UIType.h"
#include "Botton.h"
#include "Config.h"

class MenuUI : public UI<UIType> {
public:
	explicit MenuUI();
	~MenuUI() override;
	void handleInput() noexcept override final;
	void update(float dt) noexcept override final;
	void render() const noexcept override final;
private:
	std::array<Botton, Config::MENU_BOTTOMS_NUM> bottons_; // start continue settings quit
};