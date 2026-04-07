#pragma once
#include <array>
#include "core/UI.h"
#include "Botton.h"
#include "UIType.h"
#include "Config.h"

class WonUI : public UI<UIType> {
public:
	explicit WonUI();
	~WonUI() override;
	void handleInput() noexcept override final;
	void update(float dt) noexcept override final;
	void render() const noexcept override final;
private:
	std::array<Botton, Config::WON_BUTTONS_NUM> bottons_; // pass the level next level  back to menu 
};