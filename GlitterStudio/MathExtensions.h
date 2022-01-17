#pragma once
#include "MathGens.h"
#include "../DirectXMath-master/Inc/DirectXMath.h"

namespace Glitter
{
	class MathExtensions
	{
	public:
		static Vector3 vector3Transform(const Vector3& v, const DirectX::XMMATRIX& m);
		static Quaternion fromRotationZYX(const Vector3& v);
	};
}