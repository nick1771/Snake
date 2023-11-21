#pragma once

#include "Primitives.h"

#include <optional>

struct Rectangle {
    i32 left{};
    i32 top{};
    i32 right{};
    i32 bottom{};

    std::optional<Rectangle> intersection(const Rectangle& other) const;
};
