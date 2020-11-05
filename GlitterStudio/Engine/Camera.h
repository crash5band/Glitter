#pragma once
#include "..\DirectXMath-master\Inc\DirectXMath.h"
#include <string>

enum class CameraMode
{
	Normal,
	Orbit
};

class Camera
{
private:
	float defaultYaw = -90;
	float defaultPitch = 0;
	float defaultSpeed = 2.5f;
	float defaultSensitivity = 0.1f;
	float defaultZoom = 45.0f;
	float defaultRadius = 3.0f;
	DirectX::XMVECTOR defaultPosition;

	std::string modes[2];

	CameraMode mode;
	DirectX::XMVECTOR target;
	DirectX::XMVECTOR position;
	DirectX::XMVECTOR front;
	DirectX::XMVECTOR right;
	DirectX::XMVECTOR up;
	DirectX::XMVECTOR worldUp;
	float yaw;
	float pitch;
	float speed;
	float sensitivity;
	float fovy;
	float radius;

	void updateCameraVectors();
	void move(float x, float y);
	void rotate(float x, float y);
	void pollMouseWheel(float yOffset, float r);

public:
	Camera(CameraMode mode, DirectX::XMVECTOR pos = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f });

	void update(bool l, bool r, float x, float y, float w);
	void reset();

	inline float getYaw() const { return yaw; }
	inline float getPitch() const { return pitch; }
	DirectX::XMMATRIX getViewMatrix() const;
	DirectX::XMMATRIX getProjectionMatrix(float aspect) const;
	DirectX::XMVECTOR getPosition() const;
	DirectX::XMVECTOR getFront() const;
};
