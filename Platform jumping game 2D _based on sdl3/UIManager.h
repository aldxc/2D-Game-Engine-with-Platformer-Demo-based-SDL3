#pragma once
#include <memory>
#include <vector>
#include "UI.h"
#include "SubscriptionId.h"


class UIManager {
public:
	UIManager();
	~UIManager();
	void update() noexcept;
	void handleInput() noexcept;
	void render() const noexcept;
private:
	std::unique_ptr<UI> createUI(UIType type = UIType::MENU) noexcept;
	std::unique_ptr<UI> currentUI_; 
	std::vector<SubscriptionId> uiSubscriptionIds_; //UI事件订阅ID列表
};