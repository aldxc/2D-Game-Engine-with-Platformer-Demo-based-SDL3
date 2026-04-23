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
	TStateType getTopStateType() const noexcept { return stateStack_.empty() ? TStateType{} : stateStack_.back()->getType(); } 

private:
	// 引擎核心系统的引用，供状态创建、管理和注入使用
	RenderContext& renderContext_; 
	EventManager& eventManager_; 
	Physics& physics_; 
	Input& inputManager_; 
	Resource& resourceManager_;
	GameSession& gameSession_;

	std::unique_ptr<State<TStateType>> createState(TStateType stateType) noexcept;
	// 状态栈，支持状态的压入和弹出
	std::vector<std::unique_ptr<State<TStateType>>> stateStack_; 
	SubscriptionId stateTransitionSubscriptionId_{};
};

template<class TStateType, class TFactory>
StateMachine<TStateType, TFactory>::StateMachine(TStateType initState, RenderContext& renderContext, Physics& pm, Input& iM, EventManager& eM, Resource& rM, GameSession& gS) noexcept
	: renderContext_(renderContext), physics_(pm), inputManager_(iM), eventManager_(eM), resourceManager_(rM), gameSession_(gS) {
	stateStack_.push_back(createState(initState));
	stateTransitionSubscriptionId_ = eventManager_.subscribe(
		EventType::STATE_TRANSITION,
		[this](const Event& event) {
			if (event.hasData<StateRequest>()) {
				auto req = event.getData<StateRequest>();

				switch (req.op) {
				case StateOperator::PUSH: {
						// StateRequest中data字段存储了新的状态类型
						TStateType newStateType = std::any_cast<TStateType>(req.data); 
						stateStack_.push_back(createState(newStateType));
						break;
					}
					case StateOperator::POP:{
						if (!stateStack_.empty()) {
							stateStack_.pop_back();
						}
						break;
					}
					case StateOperator::REPLACE:{
						// StateRequest中data字段存储了新的状态类型
						TStateType newStateType = std::any_cast<TStateType>(req.data); 
						if (!stateStack_.empty()) {
							stateStack_.pop_back();
						}
						stateStack_.push_back(createState(newStateType));
						break;
					}
					case StateOperator::CLEAR_AND_PUSH:{
						// StateRequest中data字段存储了新的状态类型
						TStateType newStateType = std::any_cast<TStateType>(req.data); 
						stateStack_.clear();
						stateStack_.push_back(createState(newStateType));
						break;
					}
				}
			}
		});
}

template<class TStateType, class TFactory>
StateMachine<TStateType, TFactory>::~StateMachine() noexcept {
	eventManager_.unsubscribe(stateTransitionSubscriptionId_);
}

template<class TStateType, class TFactory>
void StateMachine<TStateType, TFactory>::update(double dt) noexcept {
	if (!stateStack_.empty()) {
		stateStack_.back()->update(dt);
	}
}

template<class TStateType, class TFactory>
void StateMachine<TStateType, TFactory>::render() const noexcept {
	for(const auto& state : stateStack_) {
		state->render();
	}

	renderContext_.renderer.resetRenderTarget(); 
	renderContext_.renderer.renderStaticTexture(); 
	renderContext_.renderer.renderDynamicTexture(); 
}

template<class TStateType, class TFactory>
std::unique_ptr<State<TStateType>> StateMachine<TStateType, TFactory>::createState(TStateType stateType) noexcept {
	return TFactory::create(stateType, renderContext_, physics_, inputManager_, eventManager_, resourceManager_, gameSession_);
}