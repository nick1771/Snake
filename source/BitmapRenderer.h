#include "BitmapData.h"

class BitmapRenderer {
public:
	BitmapRenderer(i32 width, i32 height);

	void clear();
	void fillRect(i32 x, i32 y, i32 width, i32 height);
	void drawImage(i32 x, i32 y, const BitmapData& data);

	void setFillColor(const Pixel& pixel);

	const u8* const getImageData() const;
private:
	Pixel _fillColor{};
	BitmapData _image;
};
