#pragma once

#if (defined(__aarch64__) || defined(_M_ARM64)) || defined(__arm__)
#include "sse2neon.h"
#else
#include <xmmintrin.h>
#endif


#ifdef _MSC_VER
#include <intrin.h>
#endif



#include "./MathUtils.h"

#include "./Base/BaseType.h"

#include "./Axis.h"
#include "./Axisd.h"

#include "./BaseMath.h"
#include "./BaseMathd.h"
#include "./Quaternion.h"
#include "./Quaterniond.h"
#include "./BoundingBox.h"
#include "./BoundingBoxd.h"
#include "./BoundingBox2D.h"
#include "./BoundingBox2Dd.h"
#include "./BoundingSphere2D.h"
#include "./BoundingSphere2Dd.h"
#include "./BoundingSphere.h"
#include "./BoundingSphered.h"
#include "./Plane.h"
#include "./Planed.h"
#include "./Triangle.h"
#include "./Triangled.h"
#include "./Spherical.h"
#include "./Curves.h"

