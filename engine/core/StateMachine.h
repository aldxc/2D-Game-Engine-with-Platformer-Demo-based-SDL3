#pragma once
#include <memory>
#include <vector>
#include "render/Renderer.h"
#include "SubscriptionId.h"
#include "State.h"
#include "EventType.h"

// 前置声明，避免循环依赖
struct RenderContext;
class EventManager;
class Physics;
class Input;
class Resource;
class GameSession;

template<class TStateType, class TFactory>
class StateMachine {
public:
	explicit StateMachine(TStateType initState, RenderContext& renderContext, Physics& pm, Input& iM, EventManager& eM, Resource& rM, GameSession& gS) noexcept;
	~StateMachine() noexcept;

	void update(double dt) noexcept;
	void render() const noexcept;
	// 获取当前状态类型
	TStateType getTopStateType() const noexcept { return m_stateStack.empty() ? TStateType{} : m_stateStack.back()->getType(); } 

private:
	// 引擎核心系统的引用，供状态创建、管理和注入使用
	RenderContext& m_renderContext; 
	EventManager& m_eventManager; 
	Physics& m_physics; 
	Input& m_inputManager; 
	Resource& m_resourceManager;
	GameSession& m_gameSession;

	std::unique_ptr<State<TStateType>> createState(TStateType stateType) noexcept;
	// 状态栈，支持状态的压入和弹出
	std::vector<std::unique_ptr<State<TStateType>>> m_stateStack; 
	SubscriptionId m_stateTransitionSubscriptionId{};
};

template<class TStateType, class TFactory>
StateMachine<TStateType, TFactory>::StateMachine(TStateType initState, RenderContext& renderContext, Physics& pm, Input& iM, EventManager& eM, Resource& rM, GameSession& gS) noexcept
	: m_renderContext(renderContext), m_physics(pm), m_inputManager(iM), m_eventManager(eM), m_resourceManager(rM), m_gameSession(gS) {
	m_stateStack.push_back(createState(initState));
	m_stateTransitionSubscriptionId = m_eventManager.subscribe(
		EventType::STATE_TRANSITION,
		[this](const Event& event) {
			if (event.hasData<StateRequest>()) {
				auto req = event.getData<StateRequest>();

				switch (req.op) {
				case StateOperator::PUSH: {
						// StateRequest中data字段存储了新的状态类型
						TStateType newStateType = std::any_cast<TStateType>(req.data); 
						m_stateStack.push_back(createState(newStateType));
						break;
					}
					case StateOperator::POP:{
						if (!m_stateStack.empty()) {
							m_stateStack.pop_back();
						}
						break;
					}
					case StateOperator::REPLACE:{
						// StateRequest中data字段存储了新的状态类型
						TStateType newStateType = std::any_cast<TStateType>(req.data); 
						if (!m_stateStack.empty()) {
							m_stateStack.pop_back();
						}
						m_stateStack.push_back(createState(newStateType));
						break;
					}
					case StateOperator::CLEAR_AND_PUSH:{
						// StateRequest中data字段存储了新的状态类型
						TStateType newStateType = std::any_cast<TStateType>(req.data); 
						m_stateStack.clear();
						m_stateStack.push_back(createState(newStateType));
						break;
					}
				}
			}
		});
}

template<class TStateType, class TFactory>
StateMachine<TStateType, TFactory>::~StateMachine() noexcept {
	m_eventManager.unsubscribe(m_stateTransitionSubscriptionId);
}

template<class TStateType, class TFactory>
void StateMachine<TStateType, TFactory>::update(double dt) noexcept {
	if (!m_stateStack.empty()) {
		m_stateStack.back()->update(dt);
	}
}

template<class TStateType, class TFactory>
void StateMachine<TStateType, TFactory>::render() const noexcept {
	for(const auto& state : m_stateStack) {
		state->render();
	}

	m_renderContext.renderer.resetRenderTarget(); 
	m_renderContext.renderer.renderStaticTexture(); 
	m_renderContext.renderer.renderDynamicTexture(); 
}

template<class TStateType, class TFactory>
std::unique_ptr<State<TStateType>> StateMachine<TStateType, TFactory>::createState(TStateType stateType) noexcept {
	return TFactory::create(stateType, m_renderContext, m_physics, m_inputManager, m_eventManager, m_resourceManager, m_gameSession);
}