#include "BitmapRenderer.h"

#include "Rectangle.h"

BitmapRenderer::BitmapRenderer(i32 width, i32 height)
	: _image(width, height) {
}

void BitmapRenderer::clear() {
	fillRect(0, 0, _image.getWidth(), _image.getHeight());
}

void BitmapRenderer::fillRect(i32 x, i32 y, i32 width, i32 height) {
	auto renderArea = Rectangle{ 0, 0, _image.getWidth(), _image.getHeight() };
	auto fillArea = Rectangle{ x, y, x + width, y + height };

	if (auto intersection = renderArea.intersection(fillArea); intersection.has_value()) {
		auto& intersectionValue = intersection.value();

		for (auto intersectionX = intersectionValue.left; intersectionX < intersectionValue.right; intersectionX++) {
			for (auto intersectionY = intersectionValue.top; intersectionY < intersectionValue.bottom; intersectionY++) {
				_image.setPixel(intersectionX, intersectionY, _fillColor);
			}
		}
	}
}

void BitmapRenderer::setFillColor(const Pixel& pixel) {
	_fillColor = pixel;
}

const u8* const BitmapRenderer::getImageData() const {
	return _image.getData();
}

void BitmapRenderer::drawImage(i32 x, i32 y, const BitmapData& image) {
	auto renderArea = Rectangle{ 0, 0, _image.getWidth(), _image.getHeight() };
	auto fillArea = Rectangle{ x, y, image.getWidth(), image.getHeight() };

	if (auto intersection = renderArea.intersection(fillArea); intersection.has_value()) {
		auto& intersectionValue = intersection.value();

		for (auto intersectionX = intersectionValue.left; intersectionX < intersectionValue.right; intersectionX++) {
			for (auto intersectionY = intersectionValue.top; intersectionY < intersectionValue.bottom; intersectionY++) {
                auto imageDataX = intersectionX - x;
				auto imageDataY = intersectionX - y;

				_image.setPixel(intersectionX, intersectionY, image.getPixel(imageDataX, imageDataY));
			}
		}
	}
}
