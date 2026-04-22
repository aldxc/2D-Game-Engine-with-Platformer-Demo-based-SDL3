#pragma once
#include <memory>
#include <vector>
#include "render/Renderer.h"
#include "SubscriptionId.h"
#include "State.h"
#include "EventType.h"

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
	TStateType getTopStateType() const noexcept { return stateStack_.empty() ? TStateType{} : stateStack_.back()->getType(); } // 获取当前状态类型，后续增加异常处理等功能

private:
	RenderContext& renderContext_; // 渲染上下文，存储全局共享的数据和资源
	EventManager& eventManager_; // 事件管理器实例
	Physics& physics_; // 物理引擎实例
	Input& inputManager_; // 输入管理器实例
	Resource& resourceManager_; // 资源管理器实例
	GameSession& gameSession_;

	std::unique_ptr<State<TStateType>> createState(TStateType stateType) noexcept;
	std::vector<std::unique_ptr<State<TStateType>>> stateStack_; // 状态栈，支持状态的压入和弹出
	SubscriptionId stateTransitionSubscriptionId_{};
};

template<class TStateType, class TFactory>
StateMachine<TStateType, TFactory>::StateMachine(TStateType initState, RenderContext& renderContext, Physics& pm, Input& iM, EventManager& eM, Resource& rM, GameSession& gS) noexcept
	: renderContext_(renderContext), physics_(pm), inputManager_(iM), eventManager_(eM), resourceManager_(rM), gameSession_(gS) {
	stateStack_.push_back(createState(initState));
	stateTransitionSubscriptionId_ = eventManager_.subscribe(
		EventType::State_Transition,
		[this](const Event& event) {
			if (event.hasData<StateRequest>()) {
				auto req = event.getData<StateRequest>();

				switch (req.op) {
				case StateOperator::Push: {
						TStateType newStateType = std::any_cast<TStateType>(req.data); // StateRequest中data字段存储了新的状态类型
						stateStack_.push_back(createState(newStateType));
						break;
					}
					case StateOperator::Pop:{
						if (!stateStack_.empty()) {
							stateStack_.pop_back();
						}
						break;
					}
					case StateOperator::Replace:{
						TStateType newStateType = std::any_cast<TStateType>(req.data); // StateRequest中data字段存储了新的状态类型
						if (!stateStack_.empty()) {
							stateStack_.pop_back();
						}
						stateStack_.push_back(createState(newStateType));
						break;
					}
					case StateOperator::ClearAndPush:{
						TStateType newStateType = std::any_cast<TStateType>(req.data); // StateRequest中data字段存储了新的状态类型
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

	renderContext_.renderer.resetRenderTarget(); // 渲染状态的动态元素后重置渲染目标为默认，准备将动态纹理内容呈现到屏幕
	renderContext_.renderer.renderStaticTexture(); // 将静态纹理渲染到默认渲染目标，呈现当前状态的静态元素，后续可优化为根据状态类型选择性渲染
	renderContext_.renderer.renderDynamicTexture(); // 将动态纹理渲染到默认渲染目标，呈现当前状态的动态元素，后续可优化为根据状态类型选择性渲染
}

template<class TStateType, class TFactory>
std::unique_ptr<State<TStateType>> StateMachine<TStateType, TFactory>::createState(TStateType stateType) noexcept {
	return TFactory::create(stateType, renderContext_, physics_, inputManager_, eventManager_, resourceManager_, gameSession_);
}