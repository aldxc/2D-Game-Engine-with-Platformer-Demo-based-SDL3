#include "UIManager.h"
#include "MenuUI.h"
#include "EventManager.h"
#include "PlayingUI.h"
#include "Renderer.h"

UIManager::UIManager() : currentUI_(createUI(UIType::MENU)){
	uiSubscriptionIds_.push_back(
		EventManager::getInstance().subscribe(EventType::State_Transition, [this](const Event& event) {
		if (event.hasData<StateType>()) {
			StateType newStateType = event.getData<StateType>();
			switch (newStateType) {
			case StateType::PLAYING:
				currentUI_ = createUI(UIType::PLAYING);
				break;
			case StateType::PAUSE:
				break;
			default:
				currentUI_ = createUI(UIType::MENU);
				break;
			}
		}
		})
	);
}

UIManager::~UIManager(){
	for(auto& id : uiSubscriptionIds_) {
		EventManager::getInstance().unsubscribe(id);
	}
}

std::unique_ptr<UI> UIManager::createUI(UIType type) noexcept {
	if(currentUI_ && currentUI_->isType(type)) {
		return std::move(currentUI_); // 如果当前UI已经是目标类型，直接返回
	}
	switch (type){
	case UIType::MENU:
		return std::make_unique<MenuUI>();
		break;
	case UIType::PLAYING:
		return std::make_unique<PlayingUI>();
		break;
	case UIType::PAUSE:
		break;
	case UIType::WON:
		break;
	case UIType::LOST:
		break;
	default:
		return std::unique_ptr<MenuUI>();
		break;
	}
}

void UIManager::update() noexcept {
	//if(currentUI_) currentUI_->update();
}

void UIManager::handleInput() noexcept {
	if(currentUI_) currentUI_->handleInput();
}

void UIManager::render() const noexcept {
	if(currentUI_) currentUI_->render();
}
