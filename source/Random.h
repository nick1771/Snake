#pragma once

#include <random>
#include <stdexcept>

#include "Primitives.h"

namespace detail {

	struct Random {
		static std::mt19937& engineInstance();
	};
}

template<typename PrimitiveType>
inline PrimitiveType generateRange(PrimitiveType minInclusive, PrimitiveType maxInclusive) {
	static_assert(true, "Unimplemented generateRange type");
}

template<>
inline i32 generateRange<i32>(i32 minInclusive, i32 maxInclusive) {
	auto distribution = std::uniform_int_distribution<>{ minInclusive, maxInclusive };
	return distribution(detail::Random::engineInstance());;
}
