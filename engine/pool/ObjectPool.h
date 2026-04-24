#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <functional>
#include <type_traits>
#include <utility>

// 默认删除器：一个空的可调用类，其 operator() 会调用 delete
struct DefaultDeleter {
	template <class T>
	void operator()(T* ptr) const {
		delete ptr;
	}
};

// 辅助类型萃取：如果 TDeleter 是 DefaultDeleter，
// 则智能指针类型退化为 std::unique_ptr<TObject>（零开销）；
// 否则使用带自定义删除器的 std::unique_ptr<TObject, TDeleter>
template <class TObject, class TDeleter>
using PoolPtr = typename std::conditional<
	std::is_same<TDeleter, DefaultDeleter>::value,
	std::unique_ptr<TObject>,
	std::unique_ptr<TObject, TDeleter>
>::type;

// 对象池模板类组件，拥有对象所有权，内部维护可用队列，提供获取和归还对象的接口, 支持自定义删除器和工厂函数，自动扩容功能
template <class TObject, class TDeleter = DefaultDeleter>
class ObjectPool {
public:
	template<class TFactory>
	explicit ObjectPool(size_t poolSize, TFactory&& factory) noexcept  : m_factory(std::forward<TFactory>(factory)) {
		expansion(poolSize);
	}
	// 禁止拷贝和移动
	ObjectPool(const ObjectPool&) = delete;
	ObjectPool& operator=(const ObjectPool&) = delete;

	TObject* acquire() noexcept {
		if (m_availableObjects.empty()) {
			// 当前没有可用对象时自动扩容：
			// 非空池按当前总量翻倍；空池至少扩 1 个
			const size_t growCount = m_objects.empty() ? 1 : m_objects.size();
			expansion(growCount);
		}
		TObject* obj = m_availableObjects.front();
		m_availableObjects.pop();
		return obj;
	}

	void release(TObject* obj) noexcept {
		if(obj == nullptr) {
			return; // 不允许归还空指针，直接返回
		}
		// 已经约定obj必须有 reset 方法，重置对象状态，否则可能出现不可控的问题
		obj->reset();
		m_availableObjects.push(obj);
	}

	size_t availableCount() const noexcept {
		return m_availableObjects.size();
	}
	size_t totalCount() const noexcept {
		return m_objects.size();
	}

	void clear() noexcept {
		while (!m_availableObjects.empty()) {
			m_availableObjects.pop();
		}
		m_objects.clear();
	}

	void expansion(size_t addCount) noexcept {
		if(addCount == 0) {
			// 默认扩容为当前大小的两倍
			addCount = m_objects.size(); 
		}
		size_t currentSize = m_objects.size();
		m_objects.reserve(currentSize + addCount);
		for (size_t i = 0; i < addCount; ++i) {
			auto ptr = m_factory();
			m_availableObjects.push(ptr.get());
			m_objects.push_back(std::move(ptr));
		}
	}
private:
	// 对象池拥有对象所有权，使用智能指针管理对象生命周期
	std::vector<PoolPtr<TObject, TDeleter>> m_objects; 
	// 可用对象队列，存储对象指针，指向m_objects中的对象
	std::queue<TObject*> m_availableObjects; 
	// 对象工厂函数，用于创建新对象
	std::function<PoolPtr<TObject, TDeleter>()> m_factory; 
};