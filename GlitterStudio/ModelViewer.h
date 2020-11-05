#pragma once
#include "RenderTarget.h"
#include "ModelData.h"
#include "Camera.h"
#include "Light.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

class ModelViewer
{
private:
	ImRect viewArea;
	RenderTarget* framebuffer;
	ModelData* model;
	Shader* shader;
	Camera* camera;
	Light* light;

	bool camWindowOpen;
	bool lightWindowOpen;
	bool mouseInViewArea;

	void updatePreview();

public:
	ModelViewer();
	~ModelViewer();

	void setModel(ModelData* model);
	void setShader(Shader* shader);
	void update();

	ModelData* activeModel() const;
	Shader* activeShader() const;
	RenderTarget* getFramebuffer() const;
	glm::mat4 getProjectionMatrix() const;
};

