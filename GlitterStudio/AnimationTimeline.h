#pragma once
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "AnimationNode.h"
#include <memory>

class AnimationTimeline
{
public:
	enum class AnimationGraphType
	{
		Translate,
		Rotate,
		Scale,
		UV,
		Color,
		Integer
	};

private:
	const int frameWidth;
	int frameStart;
	int frameEnd;
	float zoom;
	const float minZoom;
	const float maxZoom;
	int currentFrame;
	int selectedKey;
	bool hovered;
	bool holdingTan;
	std::weak_ptr<AnimationNode> node;
	Glitter::Animation* animation;
	size_t animationIndex;
	size_t limitIndex;
	std::array<float, 6> lowerLimits;
	std::array<float, 6> higherLimits;

	ImGuiWindow* window;
	ImDrawList* drawList;
	ImGuiIO* io;
	ImVec2 timelinePosOffset;
	ImVec2 timelineMinOffset;
	ImVec2 timelineMaxOffset;
	ImRect boundaries;
	ImRect scrollBoundaries;

	ImVec2 canvasPos;
	ImVec2 canvasSize;
	ImVec2 scrollbarPos;
	ImVec2 scrollbarSize;
	float effectiveFrameWidth;

	void initLimits();
	void updateMarker();
	void updateControls();
	void updateCurveEdit();
	void updateCurveScroll();
	void updateTimelineKeys();
	void updateTimelineContents();
	void drawAnimationCurve(const Glitter::Key &k1, const Glitter::Key &k2);
	void onHandleDrag(Glitter::Key &k, int index, ImVec2& pos);
	float heightToValue(float height);
	float valueToHeight(float val);

public:
	AnimationTimeline();

	int getCurrentFrame() const;
	int getSelectedKey() const;
	void setCurrentFrame(int frame);
	void newKey();
	void deleteKey();
	void nextKey();
	void previousKey();
	void firstKey();
	void lastKey();
	AnimationGraphType getGraphType(Glitter::AnimationType type) const;
	void update(std::weak_ptr<AnimationNode> &node, size_t animation);
};

