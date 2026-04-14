#pragma once
#include <memory>
#include <vector>
#include "UI.h"
#include "render/Renderer.h"
#include "SubscriptionId.h"

class Renderer;
class Input;
class EventManager;

template<class TUIType, class TFactory>
class UIManager {
public:
	explicit UIManager(TUIType initType, Input& iM, EventManager& eM, Renderer& r) noexcept;
	~UIManager() noexcept;

	void update(float dt) noexcept;
	void handleInput() noexcept;
	void render() const noexcept;
	void show(TUIType type) noexcept;

private:
	std::unique_ptr<UI<TUIType>> createUI(TUIType type) noexcept ;

private:
	Renderer& renderer_; // 渲染器引用，供UI内的渲染相关操作使用
	EventManager& eventManager_; // 事件管理器引用，供UI内的事件相关操作使用
	Input& inputManager_; // 输入管理器引用，供UI内的输入相关操作使用

	std::unique_ptr<UI<TUIType>> currentUI_;
	std::vector<SubscriptionId> uiSubscriptionIds_{};
};

template<class TUIType, class TFactory>
UIManager<TUIType, TFactory>::UIManager(TUIType initType, Input& iM, EventManager& eM, Renderer& r) noexcept
	: inputManager_(iM), eventManager_(eM), renderer_(r), currentUI_(createUI(initType)) {
	uiSubscriptionIds_.push_back(
		eventManager_.subscribe(
			EventType::UI_Show,
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
	currentUI_ = TFactory::create(type, inputManager_, eventManager_, renderer_);

	// UI一般为静态元素，不需要频繁更新，现只在初始化中更新纹理
	//后续增加血条等需要频繁更新的UI元素时可以订阅事件来更新UI纹理，避免每帧都更新UI纹理带来的性能问题
	renderer_.clearUITexture();

	if (currentUI_) {
		currentUI_->render();
	}

	renderer_.resetRenderTarget();

	return std::move(currentUI_);
}