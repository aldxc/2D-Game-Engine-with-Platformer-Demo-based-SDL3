#include "StateMachine.h"
#include "MenuState.h"
#include "Renderer.h"
#include "UI.h"
#include "EventManager.h"
#include "PlayingState.h"

StateMachine::StateMachine() : currentState_(createState(StateType::MENU)) {
	stateTransitionSubscriptionId_ = EventManager::getInstance().subscribe(EventType::State_Transition, [this](const Event& event) {
		if (event.hasData<StateType>()) {
			StateType newStateType = event.getData<StateType>();
			currentState_ = createState(newStateType);
		}
		});
}

StateMachine::~StateMachine(){
	EventManager::getInstance().unsubscribe(stateTransitionSubscriptionId_);
}

void StateMachine::update(){

}

void StateMachine::render(){
	currentState_->render();
}

inline std::unique_ptr<State> StateMachine::createState(StateType type){
	switch (type){	
	case StateType::MENU:
		return std::make_unique<MenuState>();
	case StateType::PLAYING:
		return std::make_unique<PlayingState>();
	//case StateType::Won:
	//case StateType::Lost:
	//	return std::make_unique<EndState>(info);
	//case StateType::Paused:
	//	return std::make_unique<PausedState>(info);
	default:
		return std::make_unique<MenuState>();// Ä¬ÈÏ·µ»Ø²Ëµ¥×´Ì¬
	}
}
