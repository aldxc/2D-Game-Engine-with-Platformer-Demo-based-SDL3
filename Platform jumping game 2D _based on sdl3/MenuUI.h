#pragma once
#include <array>
#include "UI.h"
#include "Botton.h"
#include "Config.h"

class MenuUI : public UI {
public:
	MenuUI();
	~MenuUI() override;
	void handleInput() noexcept override final;
	void update() noexcept override final;
	void render() const noexcept override final;
private:
	std::array<Botton, Config::MENU_BOTTOMS_NUM> bottons_; // start continue settings quit
};