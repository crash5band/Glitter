#include "MathExtensions.h"
#include "Utilities.h"

namespace Glitter
{
	Vector3 MathExtensions::vector3Transform(const Vector3& v, const DirectX::XMMATRIX& m)
	{
		DirectX::XMVECTOR vTransform{ v.x, v.y, v.z };
		vTransform = DirectX::XMVector3Transform(vTransform, m);
		return Vector3(vTransform.m128_f32[0], vTransform.m128_f32[1], vTransform.m128_f32[2]);
	}

	Quaternion MathExtensions::fromRotationZYX(const Vector3& v)
	{
		Quaternion qX, qY, qZ, qR;
		qX.fromAngleAxis(Utilities::toRadians(v.x), Vector3(1, 0, 0));
		qY.fromAngleAxis(Utilities::toRadians(v.y), Vector3(0, 1, 0));
		qZ.fromAngleAxis(Utilities::toRadians(v.z), Vector3(0, 0, 1));
		qR = qZ * qY * qX;

		return qR;
	}
}