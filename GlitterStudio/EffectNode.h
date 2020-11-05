#pragma once
#include "IGlitterNode.h"
#include "EmitterNode.h"
#include "ParticleNode.h"
#include "GlitterEffect.h"

class EffectNode : public IGlitterNode
{
private:
	std::shared_ptr<Glitter::GlitterEffect> effect;
	std::shared_ptr<AnimationNode> animationNode;
	std::vector<std::shared_ptr<EmitterNode>> emitterNodes;
	std::vector<std::shared_ptr<ParticleNode>> particleNodes;
	Transform transform;

public:
	EffectNode(std::shared_ptr<Glitter::GlitterEffect> &eff);

	std::shared_ptr<Glitter::GlitterEffect> getEffect();
	std::vector<std::shared_ptr<EmitterNode>>& getEmitterNodes();
	std::vector<std::shared_ptr<ParticleNode>>& getParticleNodes();

	void update(float time, Camera* camera);
	void kill();
	virtual NodeType getType() override;
	virtual void populateInspector() override;
	virtual std::shared_ptr<AnimationNode> getAnimationNode() override;

};

