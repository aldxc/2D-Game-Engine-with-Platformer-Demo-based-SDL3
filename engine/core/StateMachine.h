#pragma once
#include <memory>
#include "render/Renderer.h"
#include "SubscriptionId.h"
#include "State.h"


struct RenderContext;
class EventManager;
class Physics;
class Input;
class Resource;

template<class TStateType, class TFactory>
class StateMachine {
public:
	explicit StateMachine(TStateType initState, RenderContext& renderContext, Physics& pm, Input& iM, EventManager& eM, Resource& rM) noexcept;
	~StateMachine() noexcept;

	void update(float dt) noexcept;
	void render() const noexcept;

private:
	RenderContext& renderContext_; // 渲染上下文，存储全局共享的数据和资源
	EventManager& eventManager_; // 事件管理器实例
	Physics& physics_; // 物理引擎实例
	Input& inputManager_; // 输入管理器实例
	Resource& resourceManager_; // 资源管理器实例

	std::unique_ptr<State<TStateType>> createState(TStateType stateType) noexcept;
	std::unique_ptr<State<TStateType>> currentState_;
	SubscriptionId stateTransitionSubscriptionId_{};
};

template<class TStateType, class TFactory>
StateMachine<TStateType, TFactory>::StateMachine(TStateType initState, RenderContext& renderContext, Physics& pm, Input& iM, EventManager& eM, Resource& rM) noexcept
	: renderContext_(renderContext),physics_(pm), inputManager_(iM), eventManager_(eM), resourceManager_(rM), currentState_(createState(initState)) {
	stateTransitionSubscriptionId_ = eventManager_.subscribe(
		EventType::State_Transition,
		[this](const Event& event) {
			if (event.hasData<TStateType>()) {
				const TStateType newStateType = event.getData<TStateType>();
				currentState_ = createState(newStateType);
			}
		});
}

template<class TStateType, class TFactory>
StateMachine<TStateType, TFactory>::~StateMachine() noexcept {
	eventManager_.unsubscribe(stateTransitionSubscriptionId_);
}

template<class TStateType, class TFactory>
void StateMachine<TStateType, TFactory>::update(float dt) noexcept {
	if (currentState_) {
		currentState_->update(dt);
	}
}

template<class TStateType, class TFactory>
void StateMachine<TStateType, TFactory>::render() const noexcept {

	if (currentState_) {
		currentState_->render();
	}

	renderContext_.renderer.resetRenderTarget(); // 渲染状态的动态元素后重置渲染目标为默认，准备将动态纹理内容呈现到屏幕
	renderContext_.renderer.renderStaticTexture(); // 将静态纹理渲染到默认渲染目标，呈现当前状态的静态元素，后续可优化为根据状态类型选择性渲染
	renderContext_.renderer.renderDynamicTexture(); // 将动态纹理渲染到默认渲染目标，呈现当前状态的动态元素，后续可优化为根据状态类型选择性渲染
}

template<class TStateType, class TFactory>
std::unique_ptr<State<TStateType>> StateMachine<TStateType, TFactory>::createState(TStateType stateType) noexcept {
	return TFactory::create(stateType, renderContext_, physics_, inputManager_, eventManager_, resourceManager_);
}