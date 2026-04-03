#pragma once
#include <cstdint>
#include "EventType.h"

class SubscriptionId {
public:
    SubscriptionId() : id_(0), type_(EventType::Custom) {}
    SubscriptionId(uint64_t id, EventType type) : id_(id), type_(type) {}

    uint64_t getId() const { return id_; }
    EventType getType() const { return type_; }

    bool isValid() const { return id_ != 0; }

    // 比较运算符，用于容器查找
    bool operator==(const SubscriptionId& other) const {
        return id_ == other.id_ && type_ == other.type_;
    }

private:
    uint64_t id_;       // 唯一 ID（全局递增）
    EventType type_;    // 所属事件类型，便于快速定位
};