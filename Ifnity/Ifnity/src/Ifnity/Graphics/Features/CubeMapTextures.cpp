

#include "CubeMapTextures.hpp"




IFNITY_NAMESPACE



template <typename T>
T clamp(T v, T a, T b)
{
	if(v < a) return a;
	if(v > b) return b;
	return v;
}

/**
 * @brief Converts 2D face coordinates to 3D coordinates for a cubemap texture.
 *
 * This function takes the 2D coordinates (i, j) on a specific face of a cubemap texture
 * and converts them to 3D coordinates (x, y, z) in the cubemap's coordinate system.
 *
 * @param i The x-coordinate on the face.
 * @param j The y-coordinate on the face.
 * @param faceID The ID of the face (0 to 5) where the coordinates are located.
 * @param faceSize The size of the face (width and height are assumed to be equal).
 * @return A vec3 object representing the 3D coordinates corresponding to the input 2D face coordinates.
 */
	vec3 faceCoordsToXYZ(int i, int j, int faceID, int faceSize)
{
	const float A = 2.0f * float(i) / faceSize;
	const float B = 2.0f * float(j) / faceSize;

	if(faceID == 0) return vec3(-1.0f, A - 1.0f, B - 1.0f);
	if(faceID == 1) return vec3(A - 1.0f, -1.0f, 1.0f - B);
	if(faceID == 2) return vec3(1.0f, A - 1.0f, 1.0f - B);
	if(faceID == 3) return vec3(1.0f - A, 1.0f, 1.0f - B);
	if(faceID == 4) return vec3(B - 1.0f, A - 1.0f, 1.0f);
	if(faceID == 5) return vec3(1.0f - B, A - 1.0f, -1.0f);

	return vec3();
}

Bitmap convertEquirectangularMapToVerticalCross(const Bitmap& b)
{
	if(b.type_ != eBitmapType_2D) return Bitmap();

	const int faceSize = b.w_ / 4; //width total image/4 

	const int w = faceSize * 3; // the bitmap result in width  will be 3 x faceSize
	const int h = faceSize * 4; // the bitmap result in height will be 4 x faceSize 

	Bitmap result(w, h, b.comp_, b.fmt_); 

	const ivec2 kFaceOffsets[] =
	{
		ivec2(faceSize, faceSize * 3),
		ivec2(0, faceSize),
		ivec2(faceSize, faceSize),
		ivec2(faceSize * 2, faceSize),
		ivec2(faceSize, 0),
		ivec2(faceSize, faceSize * 2)
	};

	const int clampW = b.w_ - 1;
	const int clampH = b.h_ - 1;

	for(int face = 0; face != 6; face++)
	{
		for(int i = 0; i != faceSize; i++)
		{
			for(int j = 0; j != faceSize; j++)
			{
				const vec3 P = faceCoordsToXYZ(i, j, face, faceSize);
				const float R = hypot(P.x, P.y);
				const float theta = atan2(P.y, P.x);
				const float phi = atan2(P.z, R);
				//	float point source coordinates
				const float Uf = float(2.0f * faceSize * (theta + M_PI) / M_PI);
				const float Vf = float(2.0f * faceSize * (M_PI / 2.0f - phi) / M_PI);
				// 4-samples for bilinear interpolation
				const int U1 = clamp(int(floor(Uf)), 0, clampW);
				const int V1 = clamp(int(floor(Vf)), 0, clampH);
				const int U2 = clamp(U1 + 1, 0, clampW);
				const int V2 = clamp(V1 + 1, 0, clampH);
				// fractional part
				const float s = Uf - U1;
				const float t = Vf - V1;
				// fetch 4-samples
				const vec4 A = b.getPixel(U1, V1);
				const vec4 B = b.getPixel(U2, V1);
				const vec4 C = b.getPixel(U1, V2);
				const vec4 D = b.getPixel(U2, V2);
				// bilinear interpolation
				const vec4 color = A * (1 - s) * (1 - t) + B * (s) * (1 - t) + C * (1 - s) * t + D * (s) * (t);
				result.setPixel(i + kFaceOffsets[ face ].x, j + kFaceOffsets[ face ].y, color);
			}
		};
	}

	return result;
}


Bitmap convertVerticalCrossToCubeMapFaces(const Bitmap& b)
{
	const int faceWidth = b.w_ / 3;
	const int faceHeight = b.h_ / 4;

	Bitmap cubemap(faceWidth, faceHeight, 6, b.comp_, b.fmt_);
	cubemap.type_ = eBitmapType_Cube;

	const uint8_t* src = b.data_.data();
	uint8_t* dst = cubemap.data_.data();

	/*
			------
			| +Y |
	 ----------------
	 | -X | -Z | +X |
	 ----------------
			| -Y |
			------
			| +Z |
			------
	*/

	const int pixelSize = cubemap.comp_ * Bitmap::getBytesPerComponent(cubemap.fmt_);



	for(int face = 0; face != 6; ++face)
	{
		for(int j = 0; j != faceHeight; ++j)
		{
			for(int i = 0; i != faceWidth; ++i)
			{
				int x = 0;
				int y = 0;

				switch(face)
				{
					// GL_TEXTURE_CUBE_MAP_POSITIVE_X
				case 0:
					x = i;
					y = faceHeight + j;
					break;

					// GL_TEXTURE_CUBE_MAP_NEGATIVE_X
				case 1:
					x = 2 * faceWidth + i;
					y = 1 * faceHeight + j;
					break;

					// GL_TEXTURE_CUBE_MAP_POSITIVE_Y
				case 2:
					x = 2 * faceWidth - (i + 1);
					y = 1 * faceHeight - (j + 1);
					break;

					// GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
				case 3:
					x = 2 * faceWidth - (i + 1);
					y = 3 * faceHeight - (j + 1);
					break;

					// GL_TEXTURE_CUBE_MAP_POSITIVE_Z
				case 4:
					x = 2 * faceWidth - (i + 1);
					y = b.h_ - (j + 1);
					break;

					// GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
				case 5:
					x = faceWidth + i;
					y = faceHeight + j;
					break;
				}

				memcpy(dst, src + (y * b.w_ + x) * pixelSize, pixelSize);

				dst += pixelSize;
			}
		}
	}

	return cubemap;
}

IFNITY_END_NAMESPACE
