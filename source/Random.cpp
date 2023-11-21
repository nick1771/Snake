#include "Random.h"

namespace detail {

	std::mt19937& Random::engineInstance() {
		static std::mt19937 engineInstance{ std::random_device{}() };
		return engineInstance;
	}
}
