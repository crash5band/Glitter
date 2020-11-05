#include "EffectNode.h"
#include "UiHelper.h"

EffectNode::EffectNode(std::shared_ptr<Glitter::GlitterEffect> &eff) :
	effect{ eff }
{
	animationNode = std::make_shared<AnimationNode>(&eff->getAnimations());

	for (auto& particle : eff->getParticles())
		particleNodes.push_back(std::make_shared<ParticleNode>(particle));

	for (auto& emitter : eff->getEmitters())
		emitterNodes.emplace_back(std::make_shared<EmitterNode>(emitter, this));
}

std::shared_ptr<Glitter::GlitterEffect> EffectNode::getEffect()
{
	return effect;
}

std::vector<std::shared_ptr<EmitterNode>> &EffectNode::getEmitterNodes()
{
	return emitterNodes;
}

std::vector<std::shared_ptr<ParticleNode>> &EffectNode::getParticleNodes()
{
	return particleNodes;
}

std::shared_ptr<AnimationNode> EffectNode::getAnimationNode()
{
	return animationNode;
}

NodeType EffectNode::getType()
{
	return NodeType::Effect;
}

void EffectNode::update(float time, Camera* camera)
{
	float effectTime = time - effect->getStartTime();
	transform.position = effect->getTranslation() + animationNode->tryGetTranslation(effectTime);
	transform.rotation = effect->getRotation() + animationNode->tryGetRotation(effectTime);
	Glitter::Color color = effect->getColor() * animationNode->tryGetColor(effectTime);


	for (auto& particle : particleNodes)
		particle->setBaseColor(color);

	for (auto& emitter : emitterNodes)
		emitter->update(time, camera, transform);
}

void EffectNode::kill()
{
	for (auto& emitter : emitterNodes)
		emitter->kill();
}

void EffectNode::populateInspector()
{
	Glitter::GlitterEffect* eff = effect.get();
	using Effect = Glitter::GlitterEffect;
	ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;

	if (ImGui::TreeNodeEx("Effect", treeFlags))
	{
		addTextProperty("Name", eff->getName(), eff, std::mem_fn(&Effect::setName));
		addFloatProperty("Start", eff->getStartTime(), eff, std::mem_fn(&Effect::setStartTime));
		addFloatProperty("Life", eff->getLifeTime(), eff, std::mem_fn(&Effect::setLifeTime));

		addColorProperty("Color", eff->getColor(), eff, std::mem_fn(&Effect::setColor));
		addVector3Property("Translation", eff->getTranslation(), eff, std::mem_fn(&Effect::setTranslation));
		addVector3Property("Rotation", eff->getRotation(), eff, std::mem_fn(&Effect::setRotation));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (ImGui::TreeNodeEx("Flags", treeFlags))
	{
		addFlagsProperty("Loop", eff->getFlags(), 1, eff, std::mem_fn(&Effect::setFlags));
		ImGui::Text("Flags: %d", eff->getFlags());

		ImGui::TreePop();
	}
	endPropertyColumn();
}
