#include "ParticleEditor.h"
#include "EffectCommands.h"
#include "CommandManager.h"
#include "GTMManager.h"

namespace Glitter
{
	namespace Editor
	{
		void ParticleEditor::createEffect(const std::string& name)
		{
			auto eff = std::make_shared<Glitter::GlitterEffect>(name, 60.0f);
			effects.emplace_back(std::make_shared<EffectNode>(eff));
		}

		void ParticleEditor::createEmitter(std::shared_ptr<EffectNode> eff, const std::string& name)
		{
			auto emitter = std::make_shared<Glitter::Emitter>(name);
			auto emNode = std::make_shared<EmitterNode>(emitter, eff.get());
			CommandManager::pushNew(new CreateEmitterCommand(eff, emNode, eff->getEmitterNodes().size()));
		}

		void ParticleEditor::createParticle(std::shared_ptr<EffectNode> eff, const std::string& name)
		{
			auto p = std::make_shared<Glitter::Particle>(name);
			auto pNode = std::make_shared<ParticleNode>(p);
			CommandManager::pushNew(new CreateParticleCommand(eff, pNode, eff->getParticleNodes().size()));
		}

		void ParticleEditor::removeEmitter(std::shared_ptr<EffectNode> eff, int index)
		{
			CommandManager::pushNew(new DeleteEmitterCommand(eff, eff->getEmitterNodes()[index], index));
		}

		void ParticleEditor::removeParticle(std::shared_ptr<EffectNode> eff, int index)
		{
			CommandManager::pushNew(new DeleteParticleCommand(eff, eff->getParticleNodes()[index], index));
		}

		void ParticleEditor::addParticleInstance(std::shared_ptr<EmitterNode> em, std::shared_ptr<ParticleNode> p, int pos)
		{
			CommandManager::pushNew(new AddParticleCommand(em, p, pos));
		}

		void ParticleEditor::removeParticleInstance(std::shared_ptr<EmitterNode> em, std::shared_ptr<ParticleNode> p, int pos)
		{
			CommandManager::pushNew(new RemoveParticleCommand(em, p, pos));
		}

		void ParticleEditor::createMaterial(const std::string& name)
		{
			auto gMat = std::make_shared<GlitterMaterial>(name, BlendMode::Add);
			auto node = std::make_shared<MaterialNode>(gMat);
			GTMManager::add(node);
		}

		std::vector<std::weak_ptr<ParticleNode>> ParticleEditor::getAvailableParticles(std::shared_ptr<EffectNode> eff,
			std::shared_ptr<EmitterNode> em)
		{
			std::vector<std::shared_ptr<ParticleNode>> effectParticles = eff->getParticleNodes();
			std::vector<ParticleInstance> emitterParticles = em->getParticles();
			std::vector<std::weak_ptr<ParticleNode>> list;

			for (size_t i = 0; i < effectParticles.size(); ++i)
			{
				bool found = false;
				for (size_t j = 0; j < emitterParticles.size(); ++j)
				{
					if (emitterParticles[j].getReference() == effectParticles[i])
					{
						found = true;
						break;
					}
				}

				if (!found)
					list.push_back(effectParticles[i]);
			}

			return list;
		}
	}
}
