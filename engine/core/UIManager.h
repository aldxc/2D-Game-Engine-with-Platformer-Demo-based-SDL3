#pragma once
#include <memory>
#include <vector>
#include "UI.h"
#include "render/Renderer.h"
#include "SubscriptionId.h"
#include "EventManager.h"

template<class TUIType, class TFactory>
class UIManager {
public:
	explicit UIManager(TUIType initType);
	~UIManager();

	void update(float dt) noexcept;
	void handleInput() noexcept;
	void render() const noexcept;
	void show(TUIType type);

private:
	std::unique_ptr<UI<TUIType>> createUI(TUIType type);

private:
	std::unique_ptr<UI<TUIType>> currentUI_;
	std::vector<SubscriptionId> uiSubscriptionIds_{};
};

template<class TUIType, class TFactory>
UIManager<TUIType, TFactory>::UIManager(TUIType initType)
	: currentUI_(createUI(initType)) {
	uiSubscriptionIds_.push_back(
		EventManager::getInstance().subscribe(
			EventType::UI_Show,
			[this](const Event& event) {
				if (event.hasData<TUIType>()) {
					show(event.getData<TUIType>());
				}
			}));
}

template<class TUIType, class TFactory>
UIManager<TUIType, TFactory>::~UIManager() {
	for (const auto& id : uiSubscriptionIds_) {
		EventManager::getInstance().unsubscribe(id);
	}
}

template<class TUIType, class TFactory>
void UIManager<TUIType, TFactory>::update(float dt) noexcept {
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

	//后续增加需要频繁更新的UI元素时可以订阅事件来更新UI纹理，避免每帧都更新UI纹理带来的性能问题

	Renderer::getInstance().renderUITexture();
}

template<class TUIType, class TFactory>
void UIManager<TUIType, TFactory>::show(TUIType type) {
	if (currentUI_ && currentUI_->isType(type)) {
		return;
	}

	currentUI_ = createUI(type);
}

template<class TUIType, class TFactory>
std::unique_ptr<UI<TUIType>> UIManager<TUIType, TFactory>::createUI(TUIType type) {
	currentUI_ = TFactory::create(type);

	// UI一般为静态元素，不需要频繁更新，现只在初始化中更新纹理
	//后续增加血条等需要频繁更新的UI元素时可以订阅事件来更新UI纹理，避免每帧都更新UI纹理带来的性能问题
	Renderer::getInstance().clearUITexture();

	if (currentUI_) {
		currentUI_->render();
	}

	Renderer::getInstance().resetRenderTarget();

	return TFactory::create(type);
}