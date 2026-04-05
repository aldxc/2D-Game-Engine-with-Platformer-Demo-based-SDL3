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

	void update() noexcept;
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
void UIManager<TUIType, TFactory>::update() noexcept {
	if (currentUI_) {
		currentUI_->update();
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
	//后续可以考虑UI分组渲染等优化
	Renderer::getInstance().clearStaticTexture(); // 每帧切换渲染对象同时清除静态纹理，避免残影问题，后续可优化为按需清除

	if (currentUI_) {
		currentUI_->render();
	}

	Renderer::getInstance().resetRenderTarget();
	Renderer::getInstance().renderStaticTexture();
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
	return TFactory::create(type);
}