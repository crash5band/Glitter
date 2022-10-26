#include "Camera.h"
#include <algorithm>

Camera::Camera(DirectX::XMVECTOR pos) :
	position{ pos }, front{ DirectX::XMVECTOR{0.0f, 0.0f, -1.0f, 1.0f} },
	yaw{ defaultYaw }, pitch{ defaultPitch }, mode{ CameraMode::Normal }
{
	defaultRadius = 5.0f;
	defaultPosition = position;
	radius = defaultRadius;
	target = DirectX::XMVECTOR{ 0 };

	setDistance(radius);
	positionCamOrbit();
}

void Camera::reset()
{
	yaw			= defaultYaw;
	pitch		= defaultPitch;
	radius		= defaultRadius;
	positionCamOrbit();
}

DirectX::XMMATRIX Camera::getViewMatrix() const
{
	DirectX::XMVECTOR tgt = front;
	if (mode == CameraMode::Normal)
		tgt = DirectX::XMVectorAdd(tgt, position);

	return DirectX::XMMatrixLookAtRH(position, tgt, DirectX::XMVECTOR{0.0f, 1.0f, 0.0f, 1.0f});
}

DirectX::XMMATRIX Camera::getProjectionMatrix(float aspect) const
{
	return DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(45), aspect, 0.1, 150.0f);
}

void Camera::positionCamOrbit()
{
	float rYaw = DirectX::XMConvertToRadians(yaw);
	float rPitch = DirectX::XMConvertToRadians(pitch);

	position.m128_f32[0] = cos(rYaw) * cos(rPitch) * radius;
	position.m128_f32[1] = sin(rPitch) * radius;
	position.m128_f32[2] = sin(rYaw) * cos(rPitch) * radius;

	updateCameraVectorsOrbit();
}

void Camera::positionCamNormal()
{
	float rYaw = DirectX::XMConvertToRadians(yaw);
	float rPitch = DirectX::XMConvertToRadians(pitch);

	DirectX::XMVECTOR _front{ 0.0f, 0.0f, 0.0f, 1.0f };
	_front.m128_f32[0] = cos(rYaw) * cos(rPitch);
	_front.m128_f32[1] = -sin(rPitch);
	_front.m128_f32[2] = -sin(rYaw) * cos(rPitch);
	front = DirectX::XMVector3Normalize(_front);
}

void Camera::rotate(float x, float y)
{
	float factor = (mode == CameraMode::Orbit ? 1 : -1);
	yaw		+= x * 0.1f * factor;
	pitch	+= y * 0.1f;
	pitch	 = std::clamp(pitch, -89.0f, 89.0f);
}

void Camera::setAngle(float y, float p)
{
	yaw	= y;
	pitch = std::clamp(p, -89.0f, 89.0f);

	radius = DirectX::XMVector3Length(DirectX::XMVectorSubtract(target, position)).m128_f32[0];
	switch (mode)
	{
	case CameraMode::Orbit:
		positionCamOrbit();
		break;
	case CameraMode::Normal:
		positionCamNormal();
		break;
	default:
		break;
	}
}

void Camera::setDistance(float d)
{
	radius = d < 1.0f ? 1.0f : d;
	positionCamOrbit();
}

void Camera::pollMouseWheel(float yOffset)
{
	position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(front, yOffset));
}

void Camera::updateCameraVectorsOrbit()
{
	DirectX::XMVECTOR direction = DirectX::XMVectorSubtract(target, position);
	front = DirectX::XMVector3Normalize(direction);
}

void Camera::updateOrbit(bool lBtn, bool rBtn, float x, float y, float wheel)
{
	radius = DirectX::XMVector3Length(DirectX::XMVectorSubtract(target, position)).m128_f32[0];

	if (lBtn)
	{
		rotate(x, y);
		positionCamOrbit();
	}

	if (radius <= 1.0f && wheel > 0)
		return;

	pollMouseWheel(wheel);
}

void Camera::updateNormal(bool lBtn, bool rBtn, float x, float y, float wheel)
{
	if (lBtn)
	{
		rotate(x, y);
		positionCamNormal();
	}

	pollMouseWheel(wheel);
}

void Camera::update(bool leftBtn, bool rightBtn, float x, float y, float wheel)
{
	switch (mode)
	{
	case CameraMode::Orbit:
		updateOrbit(leftBtn, rightBtn, x, y, wheel);
		break;
	case CameraMode::Normal:
		updateNormal(leftBtn, rightBtn, x, y, wheel);
		break;
	default:
		break;
	}
}
