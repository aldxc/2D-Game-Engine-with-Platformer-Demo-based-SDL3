#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include <queue>
#include <atomic>
#include "EventType.h"
#include "SubscriptionId.h"

//最好单例
//事件总线
using EventListener = std::function<void(const Event&)>;

struct ListenerEntry {
	SubscriptionId Id;
	EventListener callback;
};

class EventManager {
public:
	explicit EventManager() noexcept = default;
	~EventManager() noexcept= default;

	//static EventManager& getInstance() {
	//	static EventManager instance;
	//	return instance;
	//}

	EventManager(const EventManager&) = delete;
	EventManager& operator=(const EventManager&) = delete;
	EventManager(EventManager&&) = delete;
	EventManager& operator=(EventManager&&) = delete;

	bool init() noexcept; // 初始化事件系统

	// 订阅某个类型的事件,返回ID
	SubscriptionId subscribe(EventType type, EventListener listener) noexcept;

	// 取消订阅（需要保留返回的 ID，简单实现中可用 token，这里暂略）
	// 实际项目中可返回一个 Subscription 对象用于取消
	void unsubscribe(const SubscriptionId& id) noexcept; // 需要精确匹配函数对象，较复杂，暂略

	// 发送事件（放入队列）
	void sendEvent(const Event& event) noexcept;

	// 处理队列中的所有事件（通常在游戏主循环末尾调用）
	void update() noexcept;

	// 清除所有订阅和队列
	void clear() noexcept;
private:
	// 事件类型 -> 监听器列表
	std::unordered_map<EventType, std::vector<ListenerEntry>> listeners_;

	// 事件队列
	std::queue<Event> eventQueue_;
	static std::atomic<uint64_t> nextId_; //原子操作保证后续添加多线程时线程安全
};