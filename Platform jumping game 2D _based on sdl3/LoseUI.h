#pragma once
#include <array>
#include "core/UI.h"
#include "Botton.h"
#include "Config.h"
#include "UIType.h"

class LoseUI : public UI<UIType> {
public:
	explicit LoseUI();
	~LoseUI() override;
	void handleInput() noexcept override final;
	void update() noexcept override final;
	void render() const noexcept override final;
private:
	std::array<Botton, Config::WON_BUTTONS_NUM> bottons_; // lose  back to menu
};