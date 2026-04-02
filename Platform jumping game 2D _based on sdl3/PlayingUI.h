#pragma once
#include "UI.h"

class PlayingUI : public UI {
public:
	PlayingUI();
	~PlayingUI() override;
	void handleInput() noexcept override final;
	void update() noexcept override final;
	void render() const noexcept override final;
};