


/*
* Copyright (c) 2014-2021, NVIDIA CORPORATION. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

#include "math.h"

namespace math
{
	// Projection matrix implementations

	float4x4 rotate(const float4x4& m, float angle, const vector<float, 3>& v)
	{
		float const a = angle;
		float const c = cos(a);
		float const s = sin(a);

		vector<float, 3> axis = normalize(v);
		vector<float, 3> temp = (1.0f - c) * axis;

		float4x4 Rotate;
		Rotate[ 0 ][ 0 ] = c + temp[ 0 ] * axis[ 0 ];
		Rotate[ 0 ][ 1 ] = temp[ 0 ] * axis[ 1 ] + s * axis[ 2 ];
		Rotate[ 0 ][ 2 ] = temp[ 0 ] * axis[ 2 ] - s * axis[ 1 ];
		Rotate[ 0 ][ 3 ] = 0.0f;

		Rotate[ 1 ][ 0 ] = temp[ 1 ] * axis[ 0 ] - s * axis[ 2 ];
		Rotate[ 1 ][ 1 ] = c + temp[ 1 ] * axis[ 1 ];
		Rotate[ 1 ][ 2 ] = temp[ 1 ] * axis[ 2 ] + s * axis[ 0 ];
		Rotate[ 1 ][ 3 ] = 0.0f;

		Rotate[ 2 ][ 0 ] = temp[ 2 ] * axis[ 0 ] + s * axis[ 1 ];
		Rotate[ 2 ][ 1 ] = temp[ 2 ] * axis[ 1 ] - s * axis[ 0 ];
		Rotate[ 2 ][ 2 ] = c + temp[ 2 ] * axis[ 2 ];
		Rotate[ 2 ][ 3 ] = 0.0f;

		Rotate[ 3 ][ 0 ] = 0.0f;
		Rotate[ 3 ][ 1 ] = 0.0f;
		Rotate[ 3 ][ 2 ] = 0.0f;
		Rotate[ 3 ][ 3 ] = 1.0f;

		float4x4 Result;
		Result[ 0 ] = m[ 0 ] * Rotate[ 0 ][ 0 ] + m[ 1 ] * Rotate[ 0 ][ 1 ] + m[ 2 ] * Rotate[ 0 ][ 2 ];
		Result[ 1 ] = m[ 0 ] * Rotate[ 1 ][ 0 ] + m[ 1 ] * Rotate[ 1 ][ 1 ] + m[ 2 ] * Rotate[ 1 ][ 2 ];
		Result[ 2 ] = m[ 0 ] * Rotate[ 2 ][ 0 ] + m[ 1 ] * Rotate[ 2 ][ 1 ] + m[ 2 ] * Rotate[ 2 ][ 2 ];
		Result[ 3 ] = m[ 3 ];

		return Result;
	}


	float4x4 orthoProjD3DStyle(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		float xScale = 1.0f / (right - left);
		float yScale = 1.0f / (top - bottom);
		float zScale = 1.0f / (zFar - zNear);
		return float4x4(
			2.0f * xScale, 0, 0, 0,
			0, 2.0f * yScale, 0, 0,
			0, 0, zScale, 0,
			-(left + right) * xScale, -(bottom + top) * yScale, -zNear * zScale, 1);
	}

	float4x4 orthoProjOGLStyle(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		float xScale = 1.0f / (right - left);
		float yScale = 1.0f / (top - bottom);
		float zScale = 1.0f / (zFar - zNear);
		return float4x4(
			2.0f * xScale, 0, 0, 0,
			0, 2.0f * yScale, 0, 0,
			0, 0, -2.0f * zScale, 0,
			-(left + right) * xScale, -(bottom + top) * yScale, -(zNear + zFar) * zScale, 1);
	}

	float4x4 perspProjD3DStyle(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		float xScale = 1.0f / (right - left);
		float yScale = 1.0f / (top - bottom);
		float zScale = 1.0f / (zFar - zNear);
		return float4x4(
			2.0f * xScale, 0, 0, 0,
			0, 2.0f * yScale, 0, 0,
			-(left + right) * xScale, -(bottom + top) * yScale, zFar * zScale, 1,
			0, 0, -zNear * zFar * zScale, 0);
	}

	float4x4 perspProjOGLStyle(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		float xScale = 1.0f / (right - left);
		float yScale = 1.0f / (top - bottom);
		float zScale = 1.0f / (zFar - zNear);
		return float4x4(
			2.0f * zNear * xScale, 0, 0, 0,
			0, 2.0f * zNear * yScale, 0, 0,
			(left + right) * xScale, (bottom + top) * yScale, -(zNear + zFar) * zScale, -1,
			0, 0, -2.0f * zNear * zFar * zScale, 0);
	}

	float4x4 perspProjD3DStyleReverse(float left, float right, float bottom, float top, float zNear)
	{
		float xScale = 1.0f / (right - left);
		float yScale = 1.0f / (top - bottom);

		return float4x4(
			2.0f * xScale, 0, 0, 0,
			0, 2.0f * yScale, 0, 0,
			-(left + right) * xScale, -(bottom + top) * yScale, 0, 1,
			0, 0, zNear, 0);
	}

	float4x4 perspProjD3DStyle(float verticalFOV, float aspect, float zNear, float zFar)
	{
		float yScale = 1.0f / tanf(0.5f * verticalFOV);
		float xScale = yScale / aspect;
		float zScale = 1.0f / (zFar - zNear);
		return float4x4(
			xScale, 0, 0, 0,
			0, yScale, 0, 0,
			0, 0, zFar * zScale, 1,
			0, 0, -zNear * zFar * zScale, 0);
	}

	float4x4 perspProjOGLStyle(float verticalFOV, float aspect, float zNear, float zFar)
	{
		float yScale = 1.0f / tanf(0.5f * verticalFOV);
		float xScale = yScale / aspect;
		float zScale = 1.0f / (zFar - zNear);
		return float4x4(
			xScale, 0, 0, 0,
			0, yScale, 0, 0,
			0, 0, -(zNear + zFar) * zScale, -1,
			0, 0, -2.0f * zNear * zFar * zScale, 0);
	}

	float4x4 perspProjD3DStyleReverse(float verticalFOV, float aspect, float zNear)
	{
		float yScale = 1.0f / tanf(0.5f * verticalFOV);
		float xScale = yScale / aspect;

		return float4x4(
			xScale, 0, 0, 0,
			0, yScale, 0, 0,
			0, 0, 0, 1,
			0, 0, zNear, 0);
	}

}
