#include "Editor.h"
#include "EffectCommands.h"
#include "MaterialEditor.h"
#include "CommandManager.h"

using effPtr = std::shared_ptr<EffectNode>;
using emPtr = std::shared_ptr<EmitterNode>;
using pPtr = std::shared_ptr<ParticleNode>;

void Editor::createEffect(const std::string& name)
{
	auto eff = std::make_shared<Glitter::GlitterEffect>(name, 60.0f);
	effectNodes.emplace_back(std::make_shared<EffectNode>(eff));
}

void Editor::createMaterial(const std::string& name)
{
	auto mat = std::make_shared<Glitter::GlitterMaterial>(name, Glitter::BlendMode::Add);
	auto node = std::make_shared<MaterialNode>(mat);
	MaterialEditor::add(node);
}

void Editor::createEmitter(effPtr eff, const std::string& name)
{
	auto emitter = std::make_shared<Glitter::Emitter>(name);
	auto emNode = std::make_shared<EmitterNode>(emitter, eff.get());
	CommandManager::pushNew(new CreateEmitterCommand(eff, emNode, eff->getEmitterNodes().size()));
}

void Editor::createParticle(effPtr eff, const std::string& name)
{
	auto p = std::make_shared<Glitter::Particle>(name);
	auto pNode = std::make_shared<ParticleNode>(p);
	CommandManager::pushNew(new CreateParticleCommand(eff, pNode, eff->getParticleNodes().size()));
}

void Editor::removeEmitter(effPtr eff, size_t index)
{
	CommandManager::pushNew(new DeleteEmitterCommand(eff, eff->getEmitterNodes()[index], index));
}

void Editor::removeParticle(effPtr eff, size_t index)
{
	CommandManager::pushNew(new DeleteParticleCommand(eff, eff->getParticleNodes()[index], index));
}

void Editor::addParticleInstance(emPtr em, pPtr p, size_t pos)
{
	CommandManager::pushNew(new AddParticleCommand(em, p, pos));
}

void Editor::removeParticleInstance(emPtr em, pPtr p, size_t pos)
{
	CommandManager::pushNew(new RemoveParticleCommand(em, p, pos));
}

std::vector<std::weak_ptr<ParticleNode>> Editor::getAvailableParticles(effPtr eff, emPtr em)
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