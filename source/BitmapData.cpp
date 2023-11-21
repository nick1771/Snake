#include "BitmapData.h"

namespace {
    constexpr i32 BYTES_PER_PIXEL = 4;

    usize getPixelStartIndex(i32 x, i32 y, i32 width) {
        auto index = x + y * width;
        return static_cast<usize>(index) * BYTES_PER_PIXEL;
    }
}

BitmapData::BitmapData(i32 width, i32 height)	
    : _width(width), _height(height) {
    _data.resize(static_cast<usize>(_width) * _height * BYTES_PER_PIXEL);
}

void BitmapData::setPixel(i32 x, i32 y, const Pixel& pixel) {
    auto startIndex = getPixelStartIndex(x, y, _width);

    _data[startIndex] = pixel.b;
    _data[startIndex + 1] = pixel.g;
    _data[startIndex + 2] = pixel.r;
    _data[startIndex + 3] = pixel.a;
}

i32 BitmapData::getWidth() const {
    return _width;
}

i32 BitmapData::getHeight() const {
    return _height;
}

Pixel BitmapData::getPixel(i32 x, i32 y) const {
    auto startIndex = getPixelStartIndex(x, y, _width);
    return Pixel{ 
        _data[startIndex], 
        _data[startIndex + 1], 
        _data[startIndex + 2], 
        _data[startIndex + 3] 
    };
}

const u8* const BitmapData::getData() const {
    return _data.data();
}
