#include <cmath>
#include "Vec2.h"

Vec2& Vec2::operator=(const Vec2& v) noexcept{
	m_x = v.m_x;
	m_y = v.m_y;
	return *this;
}

Vec2 Vec2::operator+(const Vec2& v) const noexcept{
	return Vec2(m_x + v.m_x, m_y + v.m_y);
}

Vec2 Vec2::operator-(const Vec2& v) const noexcept{
	return Vec2(m_x - v.m_x, m_y - v.m_y);
}

Vec2 Vec2::operator*(float scalar) const noexcept{
	return Vec2(m_x * scalar, m_y * scalar);
}

Vec2 Vec2::operator/(float scalar) const noexcept{
	if(scalar != 0.0f) {
		return Vec2(m_x / scalar, m_y / scalar);
	}
	// 如果除数为0，返回单位向量，避免除以零导致的错误
	return this->normalized(); 
}

Vec2 Vec2::operator*(const Vec2& v) const noexcept{
	return Vec2(m_x * v.m_x, m_y * v.m_y);
}

Vec2& Vec2::operator+=(const Vec2& v) noexcept{
	m_x += v.m_x;
	m_y += v.m_y;
	return *this;
}

Vec2 Vec2::operator-() const noexcept{
	return Vec2(-m_x, -m_y);
}

float Vec2::length() const noexcept{
	return std::sqrt(m_x * m_x + m_y * m_y);
}


Vec2 Vec2::normalized() const noexcept{
	float len = length();
	// 如果长度为0，返回零向量，避免除以零导致的错误
	return Vec2((len > 0.0f) ? m_x / len : 0.0f, (len > 0.0f) ? m_y / len : 0.0f); 
}

Vec2 operator*(float scalar, const Vec2& v) noexcept{
	return Vec2(v.m_x * scalar, v.m_y * scalar);
}
