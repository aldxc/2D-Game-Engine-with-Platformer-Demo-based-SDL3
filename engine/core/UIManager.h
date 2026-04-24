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
	Renderer& m_renderer; 
	EventManager& m_eventManager; 
	Input& m_inputManager;
	GameSession& m_gameSession; 

	std::unique_ptr<UI<TUIType>> m_currentUI;
	std::vector<SubscriptionId> m_uiSubscriptionIds{};
};

template<class TUIType, class TFactory>
UIManager<TUIType, TFactory>::UIManager(TUIType initType, Input& iM, EventManager& eM, Renderer& r, GameSession& gS) noexcept
	: m_inputManager(iM), m_eventManager(eM), m_renderer(r), m_currentUI(createUI(initType)), m_gameSession(gS){
	m_uiSubscriptionIds.push_back(
		m_eventManager.subscribe(
			EventType::UI_SHOW,
			[this](const Event& event) {
				if (event.hasData<TUIType>()) {
					show(event.getData<TUIType>());
				}
			}));
}

template<class TUIType, class TFactory>
UIManager<TUIType, TFactory>::~UIManager() noexcept {
	for (const auto& id : m_uiSubscriptionIds) {
		m_eventManager.unsubscribe(id);
	}
}

template<class TUIType, class TFactory>
void UIManager<TUIType, TFactory>::update(double dt) noexcept {
	if (m_currentUI) {
		m_currentUI->update(dt);
	}
}

template<class TUIType, class TFactory>
void UIManager<TUIType, TFactory>::handleInput() noexcept {
	if (m_currentUI) {
		m_currentUI->handleInput();
	}
}

template<class TUIType, class TFactory>
void UIManager<TUIType, TFactory>::render() const noexcept {
	//后续增加血条等需要频繁更新的UI元素时可以订阅事件来更新UI纹理，避免每帧都更新UI纹理带来的性能问题
	m_renderer.renderUITexture();
}

template<class TUIType, class TFactory>
void UIManager<TUIType, TFactory>::show(TUIType type) noexcept {
	if (m_currentUI && m_currentUI->isType(type)) {
		return;
	}

	m_currentUI = createUI(type);
}

template<class TUIType, class TFactory>
std::unique_ptr<UI<TUIType>> UIManager<TUIType, TFactory>::createUI(TUIType type) noexcept {
	m_currentUI = TFactory::create(type, m_inputManager, m_eventManager, m_renderer, m_gameSession);
	// UI一般为静态元素，不需要频繁更新，现只在初始化中更新纹理
	m_renderer.clearUITexture();

	if (m_currentUI) {
		m_currentUI->render();
	}

	m_renderer.resetRenderTarget();

	return std::move(m_currentUI);
}