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
	std::shared_ptr<ModelData> refModel;
	Transform transform;

public:
	EffectNode(std::shared_ptr<Glitter::GlitterEffect> &eff);

	std::shared_ptr<Glitter::GlitterEffect> getEffect();
	std::vector<std::shared_ptr<EmitterNode>>& getEmitterNodes();
	std::vector<std::shared_ptr<ParticleNode>>& getParticleNodes();
	std::shared_ptr<ModelData> getRefModel();

	void update(float time, const Camera &camera);
	void kill();
	void buildAnimations();
	void changeMesh(const std::string &name);
	virtual NodeType getType() override;
	virtual void populateInspector() override;
	virtual std::shared_ptr<AnimationNode> getAnimationNode() override;
	virtual float getLife() override;

};

