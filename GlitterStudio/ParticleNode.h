#pragma once
#include "IGlitterNode.h"
#include "Particle.h"
#include "MaterialNode.h"
#include "ModelData.h"

class ParticleNode : public IGlitterNode
{
private:
	std::shared_ptr<Glitter::Particle> particle;
	std::shared_ptr<AnimationNode> animationNode;
	std::shared_ptr<MaterialNode> materialNode;
	std::shared_ptr<ModelData> mesh;
	Glitter::Color baseColor;

public:
	ParticleNode(std::shared_ptr<Glitter::Particle>& p);

	std::shared_ptr<Glitter::Particle> &getParticle();
	std::shared_ptr<MaterialNode> &getMaterialNode();
	std::shared_ptr<ModelData> getMesh();
	Glitter::Color getBaseColor() const;
	void setBaseColor(Glitter::Color color);
	void changeMesh(const std::string& filepath);
	void setMaterial(std::shared_ptr<MaterialNode>& matNode);
	void changeMaterial(std::shared_ptr<MaterialNode>& matNode);
	void initMesh();

	virtual NodeType getType() override;
	virtual void populateInspector() override;
	virtual std::shared_ptr<AnimationNode> getAnimationNode() override;
	virtual float getLife() override;
};

