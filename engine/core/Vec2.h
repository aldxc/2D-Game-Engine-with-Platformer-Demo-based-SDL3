#pragma once

// 二维向量类
class Vec2{
public:
	Vec2() noexcept = default;
	Vec2(float x, float y) noexcept : x_(x), y_(y) {}
	~Vec2() noexcept = default;
	Vec2& operator=(const Vec2& v) noexcept;
	Vec2 operator+(const Vec2& v) const noexcept;
	Vec2 operator-(const Vec2& v) const noexcept;
	Vec2 operator*(float scalar) const noexcept; // 标量乘法
	friend Vec2 operator*(float scalar, const Vec2& v) noexcept; // 标量乘法，支持scalar在前的情况
	Vec2 operator/(float scalar) const noexcept; // 标量除法
	Vec2 operator*(const Vec2& v) const noexcept; // 分量乘法
	Vec2 operator-() const noexcept; // 取反
	Vec2& operator+=(const Vec2& v) noexcept;
	float length() const noexcept; // 向量长度
	Vec2 normalized() const noexcept; // 返回单位向量
	void set(float x, float y) noexcept { x_ = x; y_ = y; } // 设置向量值
	void setX(float x) noexcept { x_ = x; }
	void setY(float y) noexcept { y_ = y; }
	float getX() const noexcept { return x_; }
	float getY() const noexcept { return y_; }
private:
	float x_ = 0.0f;
	float y_ = 0.0f;
};