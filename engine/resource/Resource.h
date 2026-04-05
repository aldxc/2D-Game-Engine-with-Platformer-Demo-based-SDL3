#pragma once

//最好单例
class Resource {
private:
	Resource() = default;
	~Resource() = default;
public:
	//禁用拷贝与移动
	Resource(const Resource&) = delete;
	Resource& operator=(const Resource&) = delete;
	Resource(Resource&&) = delete;
	Resource& operator=(Resource&&) = delete;

	static Resource& getInstance() {
		static Resource instance;
		return instance;
	}
	bool init() noexcept; // 初始化资源，如加载纹理、字体等
private:

};