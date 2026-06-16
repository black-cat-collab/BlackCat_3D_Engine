#pragma once

#include "Utility/Math/MathHeader.h"

namespace bc
{
	extern Vector4 GammaCorrect(Vector4 vColor);
	extern Vector3 GammaCorrect(Vector3 vColor);
	// 将色温转换为RGB 色温单位为K
	extern Vector3 ColorTemperatureToRGB(const float& fTemperature);
}
