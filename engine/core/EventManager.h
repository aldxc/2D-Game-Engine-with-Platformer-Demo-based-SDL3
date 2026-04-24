#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include <queue>
#include <atomic>
#include "EventType.h"
#include "SubscriptionId.h"

using EventListener = std::function<void(const Event&)>;
// 监听器条目，包含订阅ID和回调函数
struct ListenerEntry { 
	SubscriptionId Id;
	EventListener callback;
};

//事件总线
class EventManager {
public:
	explicit EventManager() noexcept = default;
	~EventManager() noexcept= default;

	EventManager(const EventManager&) = delete;
	EventManager& operator=(const EventManager&) = delete;
	EventManager(EventManager&&) = delete;
	EventManager& operator=(EventManager&&) = delete;

	bool init() noexcept; // 初始化事件系统

	// 订阅某个类型的事件,返回ID
	SubscriptionId subscribe(EventType type, EventListener listener) noexcept;

	// 取消订阅（需要保留返回的 ID）
	void unsubscribe(const SubscriptionId& id) noexcept; 

	// 发送事件（放入队列）
	void sendEvent(const Event& event) noexcept;

	// 立即处理事件（不使用队列，直接调用监听器，适用于需要立即响应的事件）
	void triggerEvent(const Event& event) noexcept;

	// 处理队列中的所有事件（通常在游戏主循环末尾调用）
	void update() noexcept;

	// 清除所有订阅和队列
	void clear() noexcept;
private:
	// 事件类型 -> 监听器列表
	std::unordered_map<EventType, std::vector<ListenerEntry>> m_listeners;
	// 事件队列
	std::queue<Event> m_eventQueue;
	//原子操作保证后续添加多线程时线程安全
	static std::atomic<uint64_t> m_nextId; 
};