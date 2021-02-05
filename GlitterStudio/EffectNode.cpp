#include "EffectNode.h"
#include "UiHelper.h"

EffectNode::EffectNode(std::shared_ptr<Glitter::GlitterEffect> &effect) :
	effect{ effect }
{
	animationNode = std::make_shared<AnimationNode>(&effect->getAnimations());

	for (auto& particle : effect->getParticles())
		particleNodes.push_back(std::make_shared<ParticleNode>(particle));

	for (auto& emitter : effect->getEmitters())
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

float EffectNode::getLife()
{
	return effect->getLifeTime();
}

void EffectNode::update(float time, Camera* camera)
{
	float effectTime = time - effect->getStartTime();
	float effectLife = fmodf(effectTime, effect->getLifeTime() + 1);

	// effect started playing
	if (effectTime >= 0.0f)
	{
		transform.position = effect->getTranslation() + animationNode->tryGetTranslation(effectLife);
		transform.rotation.fromEulerDegrees(effect->getRotation() + animationNode->tryGetRotation(effectLife));
		Glitter::Color color = effect->getColor() * animationNode->tryGetColor(effectLife);

		for (auto& particle : particleNodes)
			particle->setBaseColor(color);

		for (auto& emitter : emitterNodes)
			emitter->update(effectTime, camera, transform);
	}
}

void EffectNode::kill()
{
	for (auto& emitter : emitterNodes)
		emitter->kill();
}

void EffectNode::buildAnimations()
{
	animationNode->buildCache();
	
	for (auto& emitter : emitterNodes)
		emitter->getAnimationNode()->buildCache();

	for (auto& particle : particleNodes)
		particle->getAnimationNode()->buildCache();
}

void EffectNode::populateInspector()
{
	using Effect = Glitter::GlitterEffect;
	ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;

	if (ImGui::TreeNodeEx("Effect", treeFlags))
	{
		addTextProperty("Name", effect->getName(), effect, std::mem_fn(&Effect::setName));
		addFloatProperty("Start", effect->getStartTime(), effect, std::mem_fn(&Effect::setStartTime));
		addFloatProperty("Life", effect->getLifeTime(), effect, std::mem_fn(&Effect::setLifeTime));

		addColorProperty("Color", effect->getColor(), effect, std::mem_fn(&Effect::setColor));
		addVector3Property("Translation", effect->getTranslation(), effect, std::mem_fn(&Effect::setTranslation));
		addVector3Property("Rotation", effect->getRotation(), effect, std::mem_fn(&Effect::setRotation));

		ImGui::TreePop();
	}
	endPropertyColumn();

	if (ImGui::TreeNodeEx("Flags", treeFlags))
	{
		addFlagsProperty("Loop", effect->getFlags(), 1, effect, std::mem_fn(&Effect::setFlags));
		ImGui::Text("Flags: %d", effect->getFlags());

		ImGui::TreePop();
	}
	endPropertyColumn();
}
