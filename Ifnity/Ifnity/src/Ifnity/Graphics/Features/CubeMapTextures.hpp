
#include "pch.h"
#include "../Bitmap.hpp"

IFNITY_NAMESPACE

using glm::vec3;
using glm::ivec2;
using glm::vec4;

#define M_PI 3.14159265358979323846

vec3    IFNITY_API    faceCoordsToXYZ(int i, int j, int faceID, int faceSize);
Bitmap  IFNITY_API    convertEquirectangularMapToVerticalCross(const Bitmap& b);
Bitmap  IFNITY_API    convertVerticalCrossToCubeMapFaces(const Bitmap& b);


IFNITY_END_NAMESPACE
