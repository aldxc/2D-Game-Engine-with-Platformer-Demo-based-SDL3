#pragma once
#include <variant>
#include <string>
#include <any>
#include <SDL3/SDL.h>

// 强枚举：事件类型
enum class EventType {
    //UI事件
    UI_ButtonClicked,
    UI_SliderChanged,

    // 状态机事件（由其他系统发送）
    State_Enter,      // 进入某个状态
    State_Exit,       // 退出某个状态
    State_Transition, // 请求状态切换

	//UI状态事件
	UI_Show,
	UI_Hide,


    // 应用事件
    App_Quit,

    Custom,
};

struct Event {
    EventType type;
    std::any data;//data 可优化

    // 辅助方法：安全获取数据
    template<typename T>
    T getData() const {
        return std::any_cast<T>(data);
    }

    template<typename T>
    bool hasData() const {
        return data.has_value() && data.type() == typeid(T);
    }
};

//struct EventKeyDown {
//    SDL_Keycode key;        // 按键码
//};
//
//struct EventKeyUp {
//    SDL_Keycode key;
//};
//
//struct EventMouseButtonDown {
//    float x, y;             // 鼠标位置
//};
//
//struct EventMouseButtonUp {
//    float x, y;
//};
//
////事件联合体
//using GameEvent = std::variant<EventKeyDown, EventKeyUp, EventMouseButtonDown>;