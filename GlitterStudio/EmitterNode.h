#pragma once
#include "IGlitterNode.h"
#include "ParticleNode.h"
#include "Transform.h"
#include "Emitter.h"
#include "ParticleInstance.h"
#include "ModelData.h"

class EffectNode;

class EmitterNode : public IGlitterNode
{
private:
	std::shared_ptr<Glitter::Emitter> emitter;
	std::shared_ptr<AnimationNode> animationNode;
	Transform transform;
	std::shared_ptr<ModelData> mesh;
	std::vector<ParticleInstance> particleInstances;

	bool visible;
	int emissionCount;
	float emissionInterval;
	float lastEmissionTime;
	Glitter::Vector3 lastEmissionPosition;

public:
	EmitterNode(std::shared_ptr<Glitter::Emitter> &em, EffectNode* parent);

	std::shared_ptr<Glitter::Emitter> getEmitter();
	std::vector<ParticleInstance>& getParticles();

	virtual NodeType getType() override;
	virtual void populateInspector() override;
	virtual std::shared_ptr<AnimationNode> getAnimationNode() override;

	void update(float time, Camera* camera, Transform& transform);
	void emit(float time, Transform& baseTransform);
	void kill();
	void changeMesh(const std::string& filename);
	void changeDirection(Glitter::EmitterDirectionType type, Camera* camera);
	void setVisible(bool val);
	void setVisibleAll(bool val);
	bool isVisible() const;

	Glitter::Vector3 getPosition(float time);
	Glitter::Vector3 getRotation(float time);
	Glitter::Vector3 getScaling(float time);
};
