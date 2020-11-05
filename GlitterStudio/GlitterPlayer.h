#pragma once
#include "EffectNode.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "Renderer.h"

class GlitterPlayer
{
private:
	float time;
	float firstFrame;
	float lastFrame;
	float playbackSpeed;
	bool playing;
	bool loop;
	bool mouseInViewArea;
	ImRect viewArea;
	EffectNode* selectedEffect;
	RenderTarget* preview;
	Camera* camera;
	Renderer* renderer;

public:
	GlitterPlayer();
	~GlitterPlayer();

	void update(float deltaT);
	void updatePreview(float deltaT);
	void togglePlayback();
	void stopPlayback();
	void setEffect(EffectNode* node);
	bool isEffectLoop();
};

