#pragma once
#include "core/UI.h"
#include "UIType.h"

class PlayingUI : public UI<UIType> {
public:
	PlayingUI();
	~PlayingUI() override;
	void handleInput() noexcept override final;
	void update() noexcept override final;
	void render() const noexcept override final;
};