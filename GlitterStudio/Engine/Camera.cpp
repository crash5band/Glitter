#include "Camera.h"
#include <algorithm>

Camera::Camera(DirectX::XMVECTOR pos) :
	position{ pos }, front{ DirectX::XMVECTOR{0.0f, 0.0f, -1.0f, 1.0f} },
	yaw{ defaultYaw }, pitch{ defaultPitch }
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
	return DirectX::XMMatrixLookAtRH(position, front, DirectX::XMVECTOR{ 0.0f, 1.0f, 0.0f, 1.0f });
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

	updateCameraVectors();
}

void Camera::rotate(float x, float y)
{
	yaw		+= x * 0.1f;
	pitch	+= y * 0.1f;
	pitch	 = std::clamp(pitch, -89.0f, 89.0f);
}

void Camera::setAngle(float y, float p)
{
	yaw	= y;
	pitch = std::clamp(p, -89.0f, 89.0f);

	radius = DirectX::XMVector3Length(DirectX::XMVectorSubtract(target, position)).m128_f32[0];
	positionCamOrbit();
}

void Camera::setDistance(float d)
{
	radius = d < 1.0f ? 1.0f : d;
	positionCamOrbit();
}

void Camera::pollMouseWheel(float yOffset, float r)
{
	if (r <= 1.0f && yOffset > 0)
		return;

	position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(front, yOffset));
}

void Camera::updateCameraVectors()
{
	DirectX::XMVECTOR direction = DirectX::XMVectorSubtract(target, position);
	front = DirectX::XMVector3Normalize(direction);
}

void Camera::update(bool leftBtn, bool rightBtn, float x, float y, float wheel)
{
	radius = DirectX::XMVector3Length(DirectX::XMVectorSubtract(target, position)).m128_f32[0];

	if (leftBtn)
	{
		rotate(x, y);
		positionCamOrbit();
	}

	pollMouseWheel(wheel, radius);
}
