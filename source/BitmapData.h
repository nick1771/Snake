#pragma once

#include <vector>
#include <iterator>

#include "Primitives.h"

struct Pixel {
    u8 b{};
    u8 g{};
    u8 r{};
    u8 a{};
};

class BitmapData {
public:
    BitmapData(i32 width, i32 height);

    void setPixel(i32 x, i32 y, const Pixel& pixel);

    i32 getWidth() const;
    i32 getHeight() const;

    Pixel getPixel(i32 x, i32 y) const;
    const u8* const getData() const;
private:
    std::vector<u8> _data{};

    i32 _width{};
    i32 _height{};
};
