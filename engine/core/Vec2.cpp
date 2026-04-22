#include <cmath>
#include "Vec2.h"

Vec2& Vec2::operator=(const Vec2& v) noexcept{
	x_ = v.x_;
	y_ = v.y_;
	return *this;
}

Vec2 Vec2::operator+(const Vec2& v) const noexcept{
	return Vec2(x_ + v.x_, y_ + v.y_);
}

Vec2 Vec2::operator-(const Vec2& v) const noexcept{
	return Vec2(x_ - v.x_, y_ - v.y_);
}

Vec2 Vec2::operator*(float scalar) const noexcept{
	return Vec2(x_ * scalar, y_ * scalar);
}

Vec2 Vec2::operator/(float scalar) const noexcept{
	if(scalar != 0.0f) {
		return Vec2(x_ / scalar, y_ / scalar);
	}
	// 如果除数为0，返回单位向量，避免除以零导致的错误
	return this->normalized(); 
}

Vec2 Vec2::operator*(const Vec2& v) const noexcept{
	return Vec2(x_ * v.x_, y_ * v.y_);
}

Vec2& Vec2::operator+=(const Vec2& v) noexcept{
	x_ += v.x_;
	y_ += v.y_;
	return *this;
}

Vec2 Vec2::operator-() const noexcept{
	return Vec2(-x_, -y_);
}

float Vec2::length() const noexcept{
	return std::sqrt(x_ * x_ + y_ * y_);
}


Vec2 Vec2::normalized() const noexcept{
	float len = length();
	// 如果长度为0，返回零向量，避免除以零导致的错误
	return Vec2((len > 0.0f) ? x_ / len : 0.0f, (len > 0.0f) ? y_ / len : 0.0f); 
}

Vec2 operator*(float scalar, const Vec2& v) noexcept{
	return Vec2(v.x_ * scalar, v.y_ * scalar);
}
