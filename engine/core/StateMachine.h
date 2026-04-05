#pragma once
#include <memory>
#include "render/Renderer.h"
#include "SubscriptionId.h"
#include "EventManager.h"
#include "State.h"

template<class TStateType, class TFactory>
class StateMachine {
public:
	explicit StateMachine(TStateType initState);
	~StateMachine();

	void update(float dt);
	void render() const;

private:
	std::unique_ptr<State<TStateType>> createState(TStateType stateType);
	std::unique_ptr<State<TStateType>> currentState_;
	SubscriptionId stateTransitionSubscriptionId_{};
};

template<class TStateType, class TFactory>
StateMachine<TStateType, TFactory>::StateMachine(TStateType initState)
	: currentState_(createState(initState)) {
	stateTransitionSubscriptionId_ = EventManager::getInstance().subscribe(
		EventType::State_Transition,
		[this](const Event& event) {
			if (event.hasData<TStateType>()) {
				const TStateType newStateType = event.getData<TStateType>();
				currentState_ = createState(newStateType);
			}
		});
}

template<class TStateType, class TFactory>
StateMachine<TStateType, TFactory>::~StateMachine() {
	EventManager::getInstance().unsubscribe(stateTransitionSubscriptionId_);
}

template<class TStateType, class TFactory>
void StateMachine<TStateType, TFactory>::update(float dt) {
	if (currentState_) {
		currentState_->update(dt);
	}
}

template<class TStateType, class TFactory>
void StateMachine<TStateType, TFactory>::render() const {
	Renderer::getInstance().clearDynamicTexture(); // 每帧开始前清除动态纹理内容，准备渲染当前状态的动态元素，后续可优化为根据状态类型选择性清除

	if (currentState_) {
		currentState_->render();
	}

	Renderer::getInstance().resetRenderTarget(); // 渲染状态的动态元素后重置渲染目标为默认，准备将动态纹理内容呈现到屏幕
	Renderer::getInstance().renderDynamicTexture(); // 将动态纹理渲染到默认渲染目标，呈现当前状态的动态元素，后续可优化为根据状态类型选择性渲染
}

template<class TStateType, class TFactory>
std::unique_ptr<State<TStateType>> StateMachine<TStateType, TFactory>::createState(TStateType stateType) {
	return TFactory::create(stateType);
}