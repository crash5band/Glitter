#pragma once
#include "IGlitterNode.h"
#include "AnimationNode.h"
#include "AnimationTimeline.h"

class Inspector
{
private:
	std::weak_ptr<IGlitterNode> node;
	std::weak_ptr<AnimationNode> animationNode;
	AnimationTimeline timeline;
	std::vector<std::string_view> available;
	int selectedAnimation;

public:
	Inspector();

	void inspectAnimation(int index);
	void inspectKey(int index);
	void setNode(std::shared_ptr<IGlitterNode> node);
	std::vector<std::string_view> getAvailableAnimations();
	void update();
};
