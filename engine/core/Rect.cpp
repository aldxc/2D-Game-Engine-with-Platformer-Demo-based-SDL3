#include <cmath>
#include "Rect.h"

float Rect::distance(const Rect& rect) const noexcept{
	float centerX1 = x() + w() / 2.0f;
	float centerY1 = y() + h() / 2.0f;
	float centerX2 = rect.x() + rect.w() / 2.0f;
	float centerY2 = rect.y() + rect.h() / 2.0f;
	return std::sqrt((centerX1 - centerX2) * (centerX1 - centerX2) + (centerY1 - centerY2) * (centerY1 - centerY2));
}
