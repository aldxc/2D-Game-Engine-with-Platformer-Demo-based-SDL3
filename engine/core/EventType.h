#pragma once
#include <variant>
#include <string>
#include <any>
#include <cstdint>
#include <SDL3/SDL.h>

//后续可设计成更通用的事件系统，支持更多类型的事件和数据，甚至可以增加事件优先级、事件过滤等功能

// 强枚举：事件类型
enum class EventType : uint8_t {
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
	UI_Change,

    Debug_TogglePlayerInfo,

    // 应用事件
    App_Quit,

    // 音频事件
	Audio_PlayBgm,
	Audio_PauseBgm,
	Audio_StopBgm,
	Audio_ResumeBgm,
	Audio_PlaySfx,

    Custom,
};

enum class StateOperator : uint8_t {
    Replace, // 替换当前状态
    Push,    // 将新状态压入状态栈
    Pop,      // 从状态栈弹出当前状态
	ClearAndPush // 清空状态栈并将新状态压入
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
