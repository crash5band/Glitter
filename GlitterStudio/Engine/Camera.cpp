#include "Camera.h"
#include <algorithm>

Camera::Camera(CameraMode _mode, DirectX::XMVECTOR pos) :
	position{ pos }, front{ DirectX::XMVECTOR{0.0f, 0.0f, -1.0f, 1.0f} }, speed{ defaultSpeed }, sensitivity{ defaultSensitivity }, fovy{ defaultZoom },
	worldUp{ DirectX::XMVECTOR{0.0f, 1.0f, 0.0f, 1.0f} }, yaw{ 90 }, pitch{ 0 }, mode{ _mode }
{
	defaultRadius = 5.0f;
	radius = defaultRadius;

	if (_mode == CameraMode::Orbit)
	{
		position = DirectX::XMVECTOR{ 0.0f, 0.0f, -radius, 1.0f };
	}

	target = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 0.0f };
	defaultPosition = position;
	updateCameraVectors();

	modes[0] = "Normal";
	modes[1] = "Orbit";
}

void Camera::reset()
{
	position	= defaultPosition;
	yaw			= defaultYaw;
	pitch		= defaultPitch;
	fovy		= 45;
	updateCameraVectors();
}

DirectX::XMMATRIX Camera::getViewMatrix() const
{
	return DirectX::XMMatrixLookAtRH(position, front, worldUp);
}

DirectX::XMMATRIX Camera::getProjectionMatrix(float aspect) const
{
	return DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(fovy), aspect, 0.1f, 150.0f);
}

DirectX::XMVECTOR Camera::getPosition() const
{
	return position;
}

DirectX::XMVECTOR Camera::getFront() const
{
	return front;
}

void Camera::move(float x, float y)
{
	position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(right, x * sensitivity * 0.1f));
	position = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(up, y * sensitivity * 0.1f));
}

void Camera::rotate(float x, float y)
{
	x *= sensitivity;
	y *= sensitivity;

	yaw		+= -x;
	pitch	+= y;
	pitch	 = std::clamp(pitch, -89.0f, 89.0f);
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
		float r =  DirectX::XMVector3Length(DirectX::XMVectorSubtract(target, position)).m128_f32[0];

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
