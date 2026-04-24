#include "EventManager.h"

// 从1开始，0表示无效ID
std::atomic<uint64_t> EventManager::m_nextId = { 1 }; 

bool EventManager::init() noexcept {

    return true;
}

SubscriptionId EventManager::subscribe(EventType type, EventListener listener) noexcept {
    uint64_t id = m_nextId.fetch_add(1);
    SubscriptionId s_id(id, type);
    m_listeners[type].push_back({ s_id, std::move(listener) });
    return s_id;
}

void EventManager::unsubscribe(const SubscriptionId& id) noexcept {
    auto it = m_listeners.find(id.getType());
    if (it == m_listeners.end()) return;// 映射不包含该事件

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
        m_listeners.erase(it);
    }
}

void EventManager::sendEvent(const Event& event) noexcept {
    m_eventQueue.push(event);
}

void EventManager::triggerEvent(const Event& event) noexcept{
    auto it = m_listeners.find(event.type);
    if (it != m_listeners.end()) {
        for (const auto& entry : it->second) {
            entry.callback(event);
        }
	}
}

void EventManager::update() noexcept {
    while (!m_eventQueue.empty()) {
        Event event = m_eventQueue.front();
        m_eventQueue.pop();

        // 查找该类型事件的监听器并调用
        auto it = m_listeners.find(event.type);
        if (it != m_listeners.end()) {
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
    m_listeners.clear();
    while (!m_eventQueue.empty()) m_eventQueue.pop();
}