#pragma once
//test
#include <array>
#include "Botton.h"

#include "core/UI.h"
#include "UIType.h"

class PlayingUI : public UI<UIType> {
public:
	explicit PlayingUI();
	~PlayingUI() override;
	void handleInput() noexcept override final;
	void update(float dt) noexcept override final;
	void render() const noexcept override final;
private:
	//test
	std::array<Botton, 3> bottons_; // ≤‚ ‘∞¥≈•
};