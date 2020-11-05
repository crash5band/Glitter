#include "Editor.h"
#include "EffectCommands.h"

using effPtr = std::shared_ptr<EffectNode>;
using emPtr = std::shared_ptr<EmitterNode>;
using pPtr = std::shared_ptr<Glitter::Particle>;

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

/*unsigned int Editor::firstAvailableID(std::vector<unsigned int>& idList)
{
	unsigned int id = 0;
	std::sort(idList.begin(), idList.end());
	for (int i = 0; i < idList.size(); ++i)
	{
		if (idList[i] == id)
			++id;
		else
			return id;
	}

	return id;
}*/

void Editor::createEmitter(effPtr eff, const std::string& name)
{
	/* get existing emitter IDs
	size_t size = eff->getEmitterNodes().size();
	std::vector<unsigned int> ids;
	for (int i = 0; i < size; ++i)
		ids.push_back(eff->getEmitterNodes()[i]->getEmitter()->getID());
		*/
	auto emitter = std::make_shared<Glitter::Emitter>(name);
	auto emNode = std::make_shared<EmitterNode>(emitter, eff.get());
	CommandManager::pushNew(new CreateEmitterCommand(eff, emNode, eff->getEmitterNodes().size()));
}

void Editor::createParticle(effPtr eff, const std::string& name)
{
	// get existing particle IDs
	/*size_t size = eff->getParticleNodes().size();
	std::vector<unsigned int> ids;
	for (int i = 0; i < size; ++i)
		ids.push_back(eff->getParticleNodes()[i]->getParticle()->getID());
		*/
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

void Editor::addParticleInstance(emPtr em, std::shared_ptr<ParticleNode> p, size_t pos)
{
	CommandManager::pushNew(new AddParticleCommand(em, p, pos));
}

void Editor::removeParticleInstance(emPtr em, std::shared_ptr<ParticleNode> p, size_t pos)
{
	CommandManager::pushNew(new RemoveParticleCommand(em, p, pos));
}

std::vector<std::weak_ptr<Glitter::Particle>> Editor::getAvailableParticles(effPtr eff, emPtr em)
{
	std::vector<pPtr> effectParticles = eff->getEffect()->getParticles();
	std::vector<std::weak_ptr<Glitter::Particle>> emitterParticles = em->getEmitter()->getParticles();
	std::vector<std::weak_ptr<Glitter::Particle>> list;

	for (size_t i = 0; i < effectParticles.size(); ++i)
	{
		bool found = false;
		for (size_t j = 0; j < emitterParticles.size(); ++j)
		{
			if (emitterParticles[j].lock().get() == effectParticles[i].get())
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