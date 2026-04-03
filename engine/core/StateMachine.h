#pragma once
#include <memory>
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
	if (currentState_) {
		currentState_->render();
	}
}

template<class TStateType, class TFactory>
std::unique_ptr<State<TStateType>> StateMachine<TStateType, TFactory>::createState(TStateType stateType) {
	return TFactory::create(stateType);
}