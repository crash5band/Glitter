#include "EffectNode.h"
#include "UiHelper.h"
#include "Utilities.h"
#include "FileGUI.h"
#include "ResourceManager.h"
#include "IconsFontAwesome5.h"
#include "File.h"

namespace Glitter
{
	namespace Editor
	{
		EffectNode::EffectNode(std::shared_ptr<GlitterEffect>& effect) :
			effect{ effect }
		{
			animSet = std::make_shared<EditorAnimationSet>(effect->getAnimations());

			for (auto& particle : effect->getParticles())
				particleNodes.push_back(std::make_shared<ParticleNode>(particle));

			for (auto& emitter : effect->getEmitters())
				emitterNodes.emplace_back(std::make_shared<EmitterNode>(emitter, this));

			animationCache.buildCache(animSet);
		}

		std::shared_ptr<GlitterEffect> EffectNode::getEffect()
		{
			return effect;
		}

		std::vector<std::shared_ptr<EmitterNode>>& EffectNode::getEmitterNodes()
		{
			return emitterNodes;
		}

		std::vector<std::shared_ptr<ParticleNode>>& EffectNode::getParticleNodes()
		{
			return particleNodes;
		}

		std::shared_ptr<EditorAnimationSet> EffectNode::getAnimationSet()
		{
			return animSet;
		}

		NodeType EffectNode::getNodeType()
		{
			return NodeType::Effect;
		}

		void EffectNode::update(float time, const Camera& camera)
		{
			float effectTime = time - effect->getStartTime();
			float effectLife = fmodf(effectTime, effect->getLifeTime() + 1);

			if (animSet->isDirty())
			{
				animationCache.buildCache(animSet);
				animSet->markDirty(false);
			}

			// effect started playing
			if (effectTime >= 0.0f)
			{
				Vector3 position = effect->getTranslation() + animationCache.tryGetTranslation(effectLife);
				Vector3 rotation = effect->getRotation() + animationCache.tryGetRotation(effectLife);
				Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

				Quaternion qX, qY, qZ, qR;
				qX.fromAngleAxis(Utilities::toRadians(rotation.x), Vector3(1, 0, 0));
				qY.fromAngleAxis(Utilities::toRadians(rotation.y), Vector3(0, 1, 0));
				qZ.fromAngleAxis(Utilities::toRadians(rotation.z), Vector3(0, 0, 1));
				qR = qZ * qY * qX;

				mat4 = DirectX::XMMatrixIdentity();
				mat4 *= DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
				mat4 *= DirectX::XMMatrixRotationQuaternion(DirectX::XMVECTOR{ qR.x, qR.y, qR.z, qR.w });
				mat4 *= DirectX::XMMatrixTranslation(position.x, position.y, position.z);

				for (auto& emitter : emitterNodes)
					emitter->update(effectTime, effectLife, camera, mat4, qR);
			}
		}

		void EffectNode::kill()
		{
			for (auto& emitter : emitterNodes)
				emitter->kill();
		}

		void EffectNode::save(const std::string& filename)
		{
			// write animations
			effect->getAnimations().clear();
			for (const EditorAnimation& animation : animSet->animations)
				effect->getAnimations().push_back(animation.toGlitterAnimation());

			for (auto& node : emitterNodes)
				node->saveAnimations();

			for (auto& node : particleNodes)
				node->saveAnimations();

			// write GTE
			effect->write(filename);
		}

		void EffectNode::populateInspector()
		{
			using Effect = GlitterEffect;
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
	}
}
