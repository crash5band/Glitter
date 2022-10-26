#pragma once
#include "..\DirectXMath-master\Inc\DirectXMath.h"
#include <string>

enum class CameraMode
{
	Orbit,
	Normal
};

static const std::string cameraModes[] =
{
	"Orbit",
	"Normal"
};

class Camera
{
private:
	CameraMode mode;
	float defaultYaw = 90;
	float defaultPitch = 5;
	float defaultRadius = 3.0f;
	DirectX::XMVECTOR defaultPosition;

	DirectX::XMVECTOR target;
	DirectX::XMVECTOR position;
	DirectX::XMVECTOR front;
	DirectX::XMVECTOR right;
	float yaw;
	float pitch;
	float radius;

	void positionCamOrbit();
	void updateCameraVectorsOrbit();
	void positionCamNormal();
	void rotate(float x, float y);
	void pollMouseWheel(float yOffset);

	void updateOrbit(bool lBtn, bool rBtn, float x, float y, float wheel);
	void updateNormal(bool lBtn, bool rBtn, float x, float y, float wheel);

public:
	Camera(DirectX::XMVECTOR pos = DirectX::XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f });

	void update(bool l, bool r, float x, float y, float w);
	void reset();
	void setAngle(float yaw, float pitch);
	void setDistance(float d);

	inline float getYaw() const { return yaw; }
	inline float getPitch() const { return pitch; }
	inline float getDistance() const { return radius; }

	DirectX::XMMATRIX getViewMatrix() const;
	DirectX::XMMATRIX getProjectionMatrix(float aspect) const;

	inline CameraMode getMode() const { return mode; }
	inline void setMode(CameraMode m) { mode = m; if (mode == CameraMode::Orbit) reset(); }
};
