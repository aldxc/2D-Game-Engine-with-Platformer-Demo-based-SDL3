#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <functional>

// 对象池模板类组件，拥有对象所有权，内部维护可用队列，提供获取和归还对象的接口
template <class TObject>
class ObjectPool {
public:
	template<class TFactory>
	explicit ObjectPool(size_t poolSize, TFactory&& factory) noexcept /* : factory_(factory)*/ {
		objects_.reserve(poolSize);
		for (size_t i = 0; i < poolSize; ++i) {
			auto ptr = factory();
			availableObjects_.push(ptr.get());
			objects_.push_back(std::move(ptr));
		}
	}
	// 禁止拷贝和移动
	ObjectPool(const ObjectPool&) = delete;
	ObjectPool& operator=(const ObjectPool&) = delete;

	TObject* acquire() noexcept {
		while (availableObjects_.empty()) {
			//expansion(); // 空间不足时自动扩容，扩容策略为当前大小的两倍
			return nullptr; // 目前不自动扩容，直接返回nullptr，后续增加自动扩容功能
		}
		TObject* obj = availableObjects_.front();
		availableObjects_.pop();
		return obj;
	}

	void release(TObject* obj) noexcept {
		// 已经约定obj必须有 reset 方法，重置对象状态，否则可能出现状态混乱的问题
		obj->reset();
		availableObjects_.push(obj);
	}

	size_t availableCount() const noexcept {
		return availableObjects_.size();
	}
	size_t totalCount() const noexcept {
		return objects_.size();
	}

	void clear() noexcept {
		while (!availableObjects_.empty()) {
			availableObjects_.pop();
		}
		objects_.clear();
	}

	//void expansion() noexcept {
	//	size_t currentSize = objects_.size();
	//	size_t newSize = currentSize * 2; // 扩容为当前大小的两倍
	//	objects_.reserve(newSize);
	//	for (size_t i = currentSize; i < newSize; ++i) {
	//		auto ptr = factory_();
	//		availableObjects_.push(ptr.get());
	//		objects_.push_back(std::move(ptr));
	//	}
	//}
private:
	std::vector<std::unique_ptr<TObject>> objects_; // 对象池拥有对象所有权，使用智能指针管理对象生命周期
	std::queue<TObject*> availableObjects_; // 可用对象队列，存储对象指针，指向objects_中的对象
	//std::function<std::unique_ptr<TObject>()> factory_; // 对象工厂函数，用于创建新对象
};