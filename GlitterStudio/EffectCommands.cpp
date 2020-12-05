#include "EffectCommands.h"

CreateEmitterCommand::CreateEmitterCommand(std::shared_ptr<EffectNode> &eff, std::shared_ptr<EmitterNode> &em, size_t pos) :
	effect{ eff }, emitter{ em }, position{ pos }
{

}

void CreateEmitterCommand::execute()
{
	effect.lock()->getEffect()->addEmitter(emitter->getEmitter(), position);
	effect.lock()->getEmitterNodes().insert(effect.lock()->getEmitterNodes().begin() + position, emitter);
}

void CreateEmitterCommand::undo()
{
	effect.lock()->getEffect()->removeEmitter(position);
	effect.lock()->getEmitterNodes().erase(effect.lock()->getEmitterNodes().begin() + position);
}

DeleteEmitterCommand::DeleteEmitterCommand(std::shared_ptr<EffectNode> &eff, std::shared_ptr<EmitterNode> &em, size_t pos) :
	effect{ eff }, emitter{ em }, position{ pos }
{

}

void DeleteEmitterCommand::execute()
{
	effect.lock()->getEffect()->removeEmitter(position);
	effect.lock()->getEmitterNodes().erase(effect.lock()->getEmitterNodes().begin() + position);
}

void DeleteEmitterCommand::undo()
{
	effect.lock()->getEffect()->addEmitter(emitter->getEmitter(), position);
	effect.lock()->getEmitterNodes().insert(effect.lock()->getEmitterNodes().begin() + position, emitter);
}

CreateParticleCommand::CreateParticleCommand(std::shared_ptr<EffectNode> &eff, std::shared_ptr<ParticleNode> &p, size_t pos) :
	effect{ eff }, particle{ p }, position{ pos }
{

}

void CreateParticleCommand::execute()
{
	effect.lock()->getEffect()->addParticle(particle->getParticle(), position);
	effect.lock()->getParticleNodes().insert(effect.lock()->getParticleNodes().begin() + position, particle);
}

void CreateParticleCommand::undo()
{
	effect.lock()->getEffect()->removeParticle(position);
	effect.lock()->getParticleNodes().erase(effect.lock()->getParticleNodes().begin() + position);
}

DeleteParticleCommand::DeleteParticleCommand(std::shared_ptr<EffectNode> &eff, std::shared_ptr<ParticleNode> &p, size_t pos) :
	effect{ eff }, particle { p }, position{ pos }
{

}

void DeleteParticleCommand::execute()
{
	effect.lock()->getEffect()->removeParticle(position);
	for (int i = 0; i < effect.lock()->getEmitterNodes().size(); ++i)
	{
		EmitterNode &em = *effect.lock()->getEmitterNodes()[i];
		size_t size = em.getParticles().size();
		for (int j = 0; j < size; ++j)
		{
			if (em.getParticles()[j].getParticle() == particle->getParticle())
			{
				// save which emitters had their particles removed
				em.getParticles().erase(em.getParticles().begin() + j);
				std::pair<size_t, size_t> indexPair{ i, j };
				emitterParticles.insert(indexPair);
				break;
			}
		}
	}

	effect.lock()->getParticleNodes().erase(effect.lock()->getParticleNodes().begin() + position);
}

void DeleteParticleCommand::undo()
{
	effect.lock()->getEffect()->addParticle(particle->getParticle(), position);
	for (std::unordered_map<size_t, size_t>::iterator it = emitterParticles.begin(); it != emitterParticles.end(); ++it)
	{
		effect.lock()->getEmitterNodes()[(*it).first]->getParticles().insert
		(effect.lock()->getEmitterNodes()[(*it).first]->getParticles().begin() + (*it).second,
			ParticleInstance(particle));
	}

	effect.lock()->getParticleNodes().insert( effect.lock()->getParticleNodes().begin() + position, particle);
}


AddParticleCommand::AddParticleCommand(std::shared_ptr<EmitterNode> &em, std::shared_ptr<ParticleNode> p, size_t pos) :
	emitter{ em }, particle{ p }, position{ pos }
{

}

void AddParticleCommand::execute()
{
	emitter.lock()->getEmitter()->addParticle(particle->getParticle());
	emitter.lock()->getParticles().insert(emitter.lock()->getParticles().begin() + position, ParticleInstance(particle));
}

void AddParticleCommand::undo()
{
	emitter.lock()->getEmitter()->removeParticle(particle->getParticle());
	emitter.lock()->getParticles().erase(emitter.lock()->getParticles().begin() + position);
}


RemoveParticleCommand::RemoveParticleCommand(std::shared_ptr<EmitterNode> &em, std::shared_ptr<ParticleNode> p, size_t pos) :
	emitter{ em }, particle{ p }, position{ pos }
{

}

void RemoveParticleCommand::execute()
{
	emitter.lock()->getEmitter()->removeParticle(particle->getParticle());
	emitter.lock()->getParticles().erase(emitter.lock()->getParticles().begin() + position);
}

void RemoveParticleCommand::undo()
{
	emitter.lock()->getEmitter()->addParticle(particle->getParticle());
	emitter.lock()->getParticles().insert(emitter.lock()->getParticles().begin() + position, ParticleInstance(particle));
}
