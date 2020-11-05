#pragma once
#include "GlitterEnums.h"
#include "AnimationNode.h"
#include <memory>

enum class NodeType
{
	Effect		= 0,
	Emitter		= 1,
	Particle	= 2,
	Material	= 4
};

class IGlitterNode
{
public:
	IGlitterNode() {}
	virtual ~IGlitterNode() {}

	virtual NodeType getType() = 0;
	virtual void populateInspector() = 0;
	virtual std::shared_ptr<AnimationNode> getAnimationNode() = 0;
};

