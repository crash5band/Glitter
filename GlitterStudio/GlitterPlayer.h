#pragma once
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "Renderer.h"
#include "Stopwatch.h"

class GlitterPlayer
{
private:
	float time;
	float maxTime;
	float playbackSpeed;
	bool playing;
	bool loop;
	bool mouseInViewArea;
	bool drawGrid;
	ImRect viewArea;
	Stopwatch timer;
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
	void replay();
	void setEffect(EffectNode* node);
	bool isEffectLoop();
};

