#include "Camera.h"
#include <algorithm>

Camera::Camera(CameraMode _mode, DirectX::XMVECTOR pos) :
	position{ pos }, front{ DirectX::XMVECTOR{0.0f, 0.0f, -1.0f, 1.0f} }, sensitivity{ defaultSensitivity }, fov{ defaultZoom },
	worldUp{ DirectX::XMVECTOR{0.0f, 1.0f, 0.0f, 1.0f} }, yaw{ defaultYaw }, pitch{ defaultPitch }, mode{ _mode }
{
	defaultRadius = 5.0f;
	radius = defaultRadius;

	if (_mode == CameraMode::Orbit)
	{
		setDistance(radius);
	}

	target = DirectX::XMVECTOR{ 0 };
	defaultPosition = position;
	updateCameraVectors();
}

void Camera::reset()
{
	position	= defaultPosition;
	yaw			= defaultYaw;
	pitch		= defaultPitch;
	fov		= defaultZoom;
	updateCameraVectors();
}

DirectX::XMMATRIX Camera::getViewMatrix() const
{
	return DirectX::XMMatrixLookAtRH(position, front, worldUp);
}

DirectX::XMMATRIX Camera::getProjectionMatrix(float aspect) const
{
	return DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(fov), aspect, 0.1f, 150.0f);
}

DirectX::XMVECTOR Camera::getPosition() const
{
	return position;
}

DirectX::XMVECTOR Camera::getFront() const
{
	return front;
}

DirectX::XMVECTOR Camera::getTarget() const
{
	return target;
}

void Camera::move(float x, float y)
{
	if (x != 0 || y != 0)
	{
		DirectX::XMVECTOR h = DirectX::XMVectorScale(right, x * sensitivity);
		DirectX::XMVECTOR v = DirectX::XMVectorScale(front, y * sensitivity);

		position = DirectX::XMVectorAdd(position, h);
		position = DirectX::XMVectorAdd(position, v);
	}
}

void Camera::rotate(float x, float y)
{
	x *= sensitivity;
	y *= sensitivity;

	yaw		+= -x;
	pitch	+= y;
	pitch	 = std::clamp(pitch, -89.0f, 89.0f);
}

void Camera::setAngle(float y, float p)
{
	yaw	= y;
	pitch = std::clamp(p, -89.0f, 89.0f);

	if (mode == CameraMode::Orbit)
	{
		float r = DirectX::XMVector3Length(DirectX::XMVectorSubtract(target, position)).m128_f32[0];
		float rYaw = DirectX::XMConvertToRadians(yaw);
		float rPitch = DirectX::XMConvertToRadians(pitch);

		position.m128_f32[0] = cos(rYaw) * cos(rPitch) * r;
		position.m128_f32[1] = sin(rPitch) * r;
		position.m128_f32[2] = sin(rYaw) * cos(rPitch) * r;
	}

	updateCameraVectors();
}

void Camera::setFOV(float angle)
{
	fov = angle;
}

void Camera::setMode(CameraMode m)
{
	mode = m;
}

void Camera::setDistance(float d)
{
	float radius = d;
	if (radius < 1.0f)
		radius = 1.0f;
	position = target;

	DirectX::XMVECTOR direction = DirectX::XMVectorScale(front, -radius);
	position = DirectX::XMVectorAdd(position, direction);
}

void Camera::setTarget(DirectX::XMVECTOR tgt)
{
	target = tgt;
	setDistance(radius);
	updateCameraVectors();
}

void Camera::pollMouseWheel(float yOffset, float r)
{
	if (r <= 1.0f && yOffset > 0)
		return;

	DirectX::XMVECTOR direction = DirectX::XMVectorScale(front, yOffset);
	position = DirectX::XMVectorAdd(position, direction);
}

void Camera::updateCameraVectors()
{
	if (mode == CameraMode::Normal)
	{
		DirectX::XMVECTOR _front;
		float rYaw = DirectX::XMConvertToRadians(yaw);
		float rPitch = DirectX::XMConvertToRadians(pitch);

		_front.m128_f32[0] = cos(rYaw) * cos(rPitch);
		_front.m128_f32[1] = sin(-rPitch);
		_front.m128_f32[2] = sin(rYaw) * cos(rPitch);

		front = DirectX::XMVector3Normalize(_front);
	}
	else
	{
		DirectX::XMVECTOR direction = DirectX::XMVectorSubtract(target, position);
		front = DirectX::XMVector3Normalize(direction);
	}

	right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(front, worldUp));
	up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(right, front));
}

void Camera::update(bool leftBtn, bool rightBtn, float x, float y, float wheel)
{
	if (mode == CameraMode::Normal)
	{
		if (leftBtn)
			move(x, y);

		if (rightBtn)
		{
			rotate(x, y);
			updateCameraVectors();
		}

		pollMouseWheel(wheel, 1.0f);
	}
	else
	{
		float r = DirectX::XMVector3Length(DirectX::XMVectorSubtract(target, position)).m128_f32[0];

		if (leftBtn)
		{
			rotate(x, y);

			float rYaw = DirectX::XMConvertToRadians(yaw);
			float rPitch = DirectX::XMConvertToRadians(pitch);

			position.m128_f32[0] = cos(rYaw) * cos(rPitch) * r;
			position.m128_f32[1] = sin(rPitch) * r;
			position.m128_f32[2] = sin(rYaw) * cos(rPitch) * r;
		}
		
		pollMouseWheel(wheel, r);
		updateCameraVectors();
	}
}
