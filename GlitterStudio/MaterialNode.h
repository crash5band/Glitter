#pragma once
#include "IGlitterNode.h"
#include "GlitterMaterial.h"
#include "TextureData.h"

class MaterialNode : public IGlitterNode
{
private:
	std::shared_ptr<Glitter::GlitterMaterial> material;
	std::shared_ptr<TextureData> texture;

public:
	MaterialNode(std::shared_ptr<Glitter::GlitterMaterial> &mat);

	std::shared_ptr<Glitter::GlitterMaterial> getMaterial();
	std::shared_ptr<TextureData> getTexture();
	void changeTexture(const std::string& filepath);

	virtual NodeType getType() override;
	virtual void populateInspector() override;
	virtual std::shared_ptr<AnimationNode> getAnimationNode() override;
	virtual float getLife() override;
};
