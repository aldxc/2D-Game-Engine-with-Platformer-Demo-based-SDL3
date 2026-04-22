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
	Vec2 operator*(float scalar) const noexcept; 
	friend Vec2 operator*(float scalar, const Vec2& v) noexcept; 
	Vec2 operator/(float scalar) const noexcept; 
	Vec2 operator*(const Vec2& v) const noexcept; 
	Vec2 operator-() const noexcept; 
	Vec2& operator+=(const Vec2& v) noexcept;
	float length() const noexcept; 
	// 返回单位向量，如果长度为0，则返回零向量
	Vec2 normalized() const noexcept; 
	void set(float x, float y) noexcept { x_ = x; y_ = y; }
	void setX(float x) noexcept { x_ = x; }
	void setY(float y) noexcept { y_ = y; }
	float x() const noexcept { return x_; }
	float y() const noexcept { return y_; }
private:
	float x_ = 0.0f;
	float y_ = 0.0f;
};