#pragma once
#include <cstdint>
#include "EventType.h"

class SubscriptionId {
public:
    explicit SubscriptionId() : m_id(0), m_type(EventType::CUSTOM) {}
    SubscriptionId(uint64_t id, EventType type) : m_id(id), m_type(type) {}

    uint64_t getId() const { return m_id; }
    EventType getType() const { return m_type; }

    bool isValid() const { return m_id != 0; }

    // 比较运算符，用于容器查找
    bool operator==(const SubscriptionId& other) const {
        return m_id == other.m_id && m_type == other.m_type;
    }

private:
    // 唯一 ID（全局递增）
    uint64_t m_id;       
    // 所属事件类型，便于快速定位
    EventType m_type;    
};