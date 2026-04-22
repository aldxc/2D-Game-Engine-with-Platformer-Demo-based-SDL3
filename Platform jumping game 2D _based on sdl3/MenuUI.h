#pragma once
#include <array>
#include "core/UI.h"
#include "UIType.h"
#include "Botton.h"
#include "Config.h"

class Renderer;
class EventManager;
class Input;
class GameSession;

class MenuUI : public UI<UIType> {
public:
	explicit MenuUI(Input& iM, EventManager& eM, Renderer& r, GameSession& gS) noexcept;
	~MenuUI() noexcept override;
	void handleInput() noexcept override final;
	void update(double dt) noexcept override final;
	void render() const noexcept override final;
private:
	// ÒÀÀµ×é¼þ
	Renderer& renderer_; 
	EventManager& eventManager_; 
	Input& inputManager_; 
	GameSession& gameSession_; 

	std::array<Botton, Config::MENU_BOTTOMS_NUM> bottons_; // start continue settings quit
};