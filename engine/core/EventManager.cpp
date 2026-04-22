#include "EventManager.h"

std::atomic<uint64_t> EventManager::nextId_ = { 1 }; // 从1开始，0表示无效ID

bool EventManager::init() noexcept {
	// 目前没有复杂的初始化需求，只是为了保持接口一致性和未来扩展，为了在game中统一初始化全部单例资源

    return true;
}

SubscriptionId EventManager::subscribe(EventType type, EventListener listener) noexcept {
    uint64_t id = nextId_.fetch_add(1);
    SubscriptionId s_id(id, type);
    listeners_[type].push_back({ s_id, std::move(listener) });
    return s_id;
}

void EventManager::unsubscribe(const SubscriptionId& id) noexcept {
    auto it = listeners_.find(id.getType());
    if (it == listeners_.end()) return;// 映射不包含该事件

    auto& listeners = it->second;
    listeners.erase(
        std::remove_if(listeners.begin(), listeners.end(), 
            [&id](const ListenerEntry& entry) {
                return entry.Id == id;
            }),
        listeners.end()
    );

    // 事件无人监听直接删除
    if (listeners.empty()) {
        listeners_.erase(it);
    }
}

void EventManager::sendEvent(const Event& event) noexcept {
    eventQueue_.push(event);
}

void EventManager::triggerEvent(const Event& event) noexcept{
    auto it = listeners_.find(event.type);
    if (it != listeners_.end()) {
        for (const auto& entry : it->second) {
            entry.callback(event);
        }
	}
}

void EventManager::update() noexcept {
    while (!eventQueue_.empty()) {
        Event event = eventQueue_.front();
        eventQueue_.pop();

        // 查找该类型事件的监听器并调用
        auto it = listeners_.find(event.type);
        if (it != listeners_.end()) {
            // 事件安全
            // 注意：回调执行期间可能修改监听器列表（比如 unsubscribe）
            // 为了防止迭代器失效，先复制一份回调列表
            std::vector<EventListener> callbacks;
            callbacks.reserve(it->second.size());
            for (const auto& entry : it->second) {
                callbacks.push_back(entry.callback);
            }
            for (const auto& cb : callbacks) {
                cb(event);
            }
        }
    }
}

void EventManager::clear() noexcept {
    listeners_.clear();
    while (!eventQueue_.empty()) eventQueue_.pop();
}