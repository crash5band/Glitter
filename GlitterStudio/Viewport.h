#pragma once
#include "Camera.h"
#include "Light.h"
#include "RenderTarget.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

class Viewport
{
private:
	Camera camera;
	Light light;
	RenderTarget fBuffer;
	ImRect viewArea;
	Glitter::Vector2 size;
	bool mouseInViewArea;
	bool lightEnabled;

public:
	Viewport();

	void use();
	void end();
	void cameraControl();
	void setLight(Light l);
	void resetCamera();
	void setLightEnabled(bool v);

	Glitter::Vector2 getSize() const;
	Camera getCamera() const;
	Light getLight() const;
	bool isLightEnabled() const;
};