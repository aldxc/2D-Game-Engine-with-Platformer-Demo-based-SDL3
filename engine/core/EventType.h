#pragma once
#include <variant>
#include <string>
#include <any>
#include <cstdint>
#include <SDL3/SDL.h>

//后续可设计成更通用的事件系统，支持更多类型的事件和数据，甚至可以增加事件优先级、事件过滤等功能

// 事件类型
enum class EventType : uint8_t {
    //UI事件
    UI_BUTTON_CLICKED,
    UI_SLIDER_CHANGED,

    // 状态机事件（由其他系统发送）
    STATE_ENTER,      // 进入某个状态
    STATE_EXIT,       // 退出某个状态
    STATE_TRANSITION, // 请求状态切换

	//UI状态事件
    UI_SHOW,
    UI_HIDE,
    UI_CHANGE,

	// 调试事件
    DEBUG_TOGGLE_PLAYER_INFO,

    // 应用事件
    APP_QUIT,

    // 音频事件
	AUDIO_PLAY_BGM,
	AUDIO_PAUSE_BGM,
	AUDIO_STOP_BGM,
	AUDIO_RESUME_BGM,
	AUDIO_PLAY_SFX,

    // 自定义事件
	CUSTOM
};

enum class StateOperator : uint8_t {
    REPLACE, // 替换当前状态
    PUSH,    // 将新状态压入状态栈
    POP,      // 从状态栈弹出当前状态
    CLEAR_AND_PUSH // 清空状态栈并将新状态压入
};

struct StateRequest {
    StateOperator op; // 操作类型
	std::any data; // 新状态类型
};  

struct Event {
    EventType type;
	std::any data; //data 可优化为 std::variant 或者使用事件ID映射数据类型等方式来避免 std::any 的性能开销，但目前为了简化设计和接口，暂时使用 std::any 来支持任意类型的数据传递

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
