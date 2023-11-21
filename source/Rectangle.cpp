#include "Rectangle.h"

std::optional<Rectangle> Rectangle::intersection(const Rectangle& other) const {
	auto intersectionLeft = std::max(left, other.left);
	auto intersectionTop = std::max(top, other.top);

	auto intersectionRight = std::min(right, other.right);
	auto intersectionBottom = std::min(bottom, other.bottom);

	if (intersectionLeft >= intersectionRight || intersectionTop >= intersectionBottom) {
		return {};
	}

	return Rectangle{ intersectionLeft, intersectionTop, intersectionRight, intersectionBottom };
}
