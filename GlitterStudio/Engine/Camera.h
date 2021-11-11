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
	float defaultYaw = 90;
	float defaultPitch = 5;
	float defaultSensitivity = 0.1f;
	float defaultZoom = 45.0f;
	float defaultRadius = 3.0f;
	DirectX::XMVECTOR defaultPosition;

	CameraMode mode;
	DirectX::XMVECTOR target;
	DirectX::XMVECTOR position;
	DirectX::XMVECTOR front;
	DirectX::XMVECTOR right;
	DirectX::XMVECTOR up;
	float yaw;
	float pitch;
	float speed;
	float sensitivity;
	float fov;
	float radius;
	float near;
	float far;

	void positionCamOrbit();
	void updateCameraVectors();
	void move(float x, float y);
	void rotate(float x, float y);
	void pollMouseWheel(float yOffset, float r);

public:
	Camera(CameraMode mode, DirectX::XMVECTOR pos = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f });

	void update(bool l, bool r, float x, float y, float w);
	void reset();
	void setAngle(float yaw, float pitch);
	void setFOV(float angle);
	void setDistance(float d);
	void setMode(CameraMode mode);
	void setTarget(DirectX::XMVECTOR tgt);

	inline float getYaw() const { return yaw; }
	inline float getPitch() const { return pitch; }
	inline float getFOV() const { return fov; }
	inline float getDistance() const { return radius; }
	inline CameraMode getMode() const { return mode; }

	DirectX::XMMATRIX getViewMatrix() const;
	DirectX::XMMATRIX getProjectionMatrix(float aspect) const;
	DirectX::XMVECTOR getPosition() const;
	DirectX::XMVECTOR getFront() const;
	DirectX::XMVECTOR getTarget() const;
};
