#include "StateMachine.h"
#include "MenuState.h"
#include "Renderer.h"

StateMachine::StateMachine() : currentState_(createState(StateType::MENU)) {

}

void StateMachine::update(){

}

void StateMachine::render(){
	Renderer::getInstance().beginRender();

	currentState_->render();

	Renderer::getInstance().restoreDefaultAndPresent();
}

inline std::unique_ptr<State> StateMachine::createState(StateType type){
	switch (type){	
	case StateType::MENU:
		return std::make_unique<MenuState>();
	//case StateType::Playing:
	//	return std::make_unique<PlayingState>(info);
	//case StateType::Won:
	//case StateType::Lost:
	//	return std::make_unique<EndState>(info);
	//case StateType::Paused:
	//	return std::make_unique<PausedState>(info);
	default:
		return std::make_unique<MenuState>();// Ä¬ÈÏ·µ»Ø²Ëµ¥×´Ì¬
	}
}
