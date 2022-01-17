#include "EffectNode.h"
#include "UiHelper.h"
#include "Utilities.h"
#include "FileGUI.h"
#include "ResourceManager.h"
#include "IconsFontAwesome5.h"
#include "File.h"
#include "MathExtensions.h"
#include <map>

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

			animSet->markDirty(true);
		}

		EffectNode::EffectNode(std::shared_ptr<EffectNode>& rhs)
		{
			effect = std::make_shared<GlitterEffect>(*rhs->effect);
			effect->setFilename("");
			std::map<int, std::vector<int>> emitterParticleIndices;

			int particleCount = effect->getParticles().size();
			int emitterCount = effect->getEmitters().size();

			// determine which particles each emitter references
			for (int i = 0; i < emitterCount; ++i)
			{
				emitterParticleIndices.insert(std::pair<int, std::vector<int>>{i, std::vector<int>()});
				auto emParticles = rhs->effect->getEmitters()[i]->getParticles();
				for (int j = 0; j < emParticles.size(); ++j)
				{
					for (int k = 0; k < particleCount; ++k)
					{
						if (emParticles[j].lock() == rhs->effect->getParticles()[k])
							emitterParticleIndices[i].push_back(k);
					}
				}
			}

			effect->clearParticles();
			effect->clearEmitters();

			for (int i = 0; i < particleCount; ++i)
				effect->addParticle(std::make_shared<Particle>(*rhs->effect->getParticles()[i]));

			// create copies of the emitters and particles and add them to the new effect
			for (int i = 0; i < emitterCount; ++i)
			{
				std::shared_ptr<Emitter> emitter = std::make_shared<Emitter>(*rhs->effect->getEmitters()[i]);
				emitter->clearParticles();

				effect->addEmitter(emitter);
				std::vector<int> indices = emitterParticleIndices[i];

				for (int j = 0; j < indices.size(); ++j)
					effect->getEmitters()[i]->addParticle(effect->getParticles()[indices[j]]);
			}

			// create emitter nodes and particles nodes
			for (int i = 0; i < particleCount; ++i)
			{
				auto pNode = std::make_shared<ParticleNode>(effect->getParticles()[i]);
				pNode->changeMaterial(rhs->getParticleNodes()[i]->getMaterialNode());
				pNode->changeMesh(rhs->getParticleNodes()[i]->getMesh());
				particleNodes.push_back(pNode);
			}

			for (int i = 0; i < emitterCount; ++i)
			{
				auto eNode = std::make_shared<EmitterNode>(effect->getEmitters()[i], this);
				eNode->changeMesh(rhs->getEmitterNodes()[i]->getMesh());
				emitterNodes.push_back(eNode);
			}

			animSet = std::make_shared<EditorAnimationSet>(effect->getAnimations());
			animSet->markDirty(true);
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

		void EffectNode::updateMatrix(const Vector3& pos, const Quaternion& rot, const Vector3& scale)
		{
			mat4  = DirectX::XMMatrixIdentity();
			mat4 *= DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
			mat4 *= DirectX::XMMatrixRotationQuaternion(DirectX::XMVECTOR{ rot.x, rot.y, rot.z, rot.w });
			mat4 *= DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
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
				Quaternion qR = MathExtensions::fromRotationZYX(rotation);

				updateMatrix(position, qR, scale);

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
				node->save();

			for (auto& node : particleNodes)
				node->save();

			// write GTE
			effect->write(filename);
		}

		void EffectNode::populateInspector()
		{
			using Effect = GlitterEffect;
			ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

			if (ImGui::TreeNodeEx("Effect", treeFlags))
			{
				beginPropertyColumn();

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
				beginPropertyColumn();

				addFlagsProperty("Loop", effect->getFlags(), 1, effect, std::mem_fn(&Effect::setFlags));
				ImGui::Text("Flags: %d", effect->getFlags());

				ImGui::TreePop();
			}
			endPropertyColumn();
		}
	}
}
