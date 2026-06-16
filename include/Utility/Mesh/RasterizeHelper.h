#pragma once

#include <cstdint>
#include "Utility/Math/MathHeader.h"

namespace bc
{
	class Image;
	namespace RasterizeHelper
	{
		extern void RasterizeTriangle(Image* dstImage, Vector2d* dstVert, Image* srcImage, Vector2* srcVert, const uint8_t* color);
		extern void RasterizeTriangle2(Image* dstImage, Vector2d* dstVert, Image* srcImage, Vector2* srcVert);
		extern void RasterizeTriangleByColor(Image* dstImage, Vector2d* dstVert, Vector2* srcVert, const uint8_t* color);
	}
}




