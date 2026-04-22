#pragma once
#include <memory>
#include <vector>
#include "UI.h"
#include "render/Renderer.h"
#include "SubscriptionId.h"

// 前置声明，避免头文件循环依赖
class Renderer;
class Input;
class EventManager;
class GameSession;

template<class TUIType, class TFactory>
class UIManager {
public:
	explicit UIManager(TUIType initType, Input& iM, EventManager& eM, Renderer& r, GameSession& gS) noexcept;
	~UIManager() noexcept;

	void update(double dt) noexcept;
	void handleInput() noexcept;
	void render() const noexcept;
	void show(TUIType type) noexcept;

private:
	std::unique_ptr<UI<TUIType>> createUI(TUIType type) noexcept ;

private:
	// 持有引擎组件的引用，供UI内的渲染、事件、输入和游戏状态相关操作使用
	Renderer& renderer_; 
	EventManager& eventManager_; 
	Input& inputManager_;
	GameSession& gameSession_; 

	std::unique_ptr<UI<TUIType>> currentUI_;
	std::vector<SubscriptionId> uiSubscriptionIds_{};
};

template<class TUIType, class TFactory>
UIManager<TUIType, TFactory>::UIManager(TUIType initType, Input& iM, EventManager& eM, Renderer& r, GameSession& gS) noexcept
	: inputManager_(iM), eventManager_(eM), renderer_(r), currentUI_(createUI(initType)), gameSession_(gS){
	uiSubscriptionIds_.push_back(
		eventManager_.subscribe(
			EventType::UI_SHOW,
			[this](const Event& event) {
				if (event.hasData<TUIType>()) {
					show(event.getData<TUIType>());
				}
			}));
}

template<class TUIType, class TFactory>
UIManager<TUIType, TFactory>::~UIManager() noexcept {
	for (const auto& id : uiSubscriptionIds_) {
		eventManager_.unsubscribe(id);
	}
}

template<class TUIType, class TFactory>
void UIManager<TUIType, TFactory>::update(double dt) noexcept {
	if (currentUI_) {
		currentUI_->update(dt);
	}
}

template<class TUIType, class TFactory>
void UIManager<TUIType, TFactory>::handleInput() noexcept {
	if (currentUI_) {
		currentUI_->handleInput();
	}
}

template<class TUIType, class TFactory>
void UIManager<TUIType, TFactory>::render() const noexcept {
	//后续增加血条等需要频繁更新的UI元素时可以订阅事件来更新UI纹理，避免每帧都更新UI纹理带来的性能问题
	renderer_.renderUITexture();
}

template<class TUIType, class TFactory>
void UIManager<TUIType, TFactory>::show(TUIType type) noexcept {
	if (currentUI_ && currentUI_->isType(type)) {
		return;
	}

	currentUI_ = createUI(type);
}

template<class TUIType, class TFactory>
std::unique_ptr<UI<TUIType>> UIManager<TUIType, TFactory>::createUI(TUIType type) noexcept {
	currentUI_ = TFactory::create(type, inputManager_, eventManager_, renderer_, gameSession_);
	// UI一般为静态元素，不需要频繁更新，现只在初始化中更新纹理
	renderer_.clearUITexture();

	if (currentUI_) {
		currentUI_->render();
	}

	renderer_.resetRenderTarget();

	return std::move(currentUI_);
}