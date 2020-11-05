#include "EffectCommands.h"

CreateEmitterCommand::CreateEmitterCommand(std::shared_ptr<EffectNode> &eff, std::shared_ptr<EmitterNode> &em, size_t pos) :
	effect{ eff }, emitter{ em }, position{ pos }
{

}

void CreateEmitterCommand::execute()
{
	effect->getEffect()->addEmitter(emitter->getEmitter(), position);
	effect->getEmitterNodes().insert(effect->getEmitterNodes().begin() + position, emitter);
}

void CreateEmitterCommand::undo()
{
	effect->getEffect()->removeEmitter(position);
	effect->getEmitterNodes().erase(effect->getEmitterNodes().begin() + position);
}

DeleteEmitterCommand::DeleteEmitterCommand(std::shared_ptr<EffectNode> &eff, std::shared_ptr<EmitterNode> &em, size_t pos) :
	effect{ eff }, emitter{ em }, position{ pos }
{

}

void DeleteEmitterCommand::execute()
{
	effect->getEffect()->removeEmitter(position);
	effect->getEmitterNodes().erase(effect->getEmitterNodes().begin() + position);
}

void DeleteEmitterCommand::undo()
{
	effect->getEffect()->addEmitter(emitter->getEmitter(), position);
	effect->getEmitterNodes().insert(effect->getEmitterNodes().begin() + position, emitter);
}

CreateParticleCommand::CreateParticleCommand(std::shared_ptr<EffectNode> &eff, std::shared_ptr<ParticleNode> &p, size_t pos) :
	effect{ eff }, particle{ p }, position{ pos }
{

}

void CreateParticleCommand::execute()
{
	effect->getEffect()->addParticle(particle->getParticle(), position);
	effect->getParticleNodes().insert(effect->getParticleNodes().begin() + position, particle);
}

void CreateParticleCommand::undo()
{
	effect->getEffect()->removeParticle(position);
	effect->getParticleNodes().erase(effect->getParticleNodes().begin() + position);
}

DeleteParticleCommand::DeleteParticleCommand(std::shared_ptr<EffectNode> &eff, std::shared_ptr<ParticleNode> &p, size_t pos) :
	effect{ eff }, particle { p }, position{ pos }
{

}

void DeleteParticleCommand::execute()
{
	effect->getEffect()->removeParticle(position);
	for (int i = 0; i < effect->getEmitterNodes().size(); ++i)
	{
		EmitterNode &em = *effect->getEmitterNodes()[i];
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

	effect->getParticleNodes().erase(effect->getParticleNodes().begin() + position);
}

void DeleteParticleCommand::undo()
{
	effect->getEffect()->addParticle(particle->getParticle(), position);
	for (std::unordered_map<size_t, size_t>::iterator it = emitterParticles.begin(); it != emitterParticles.end(); ++it)
	{
		effect->getEmitterNodes()[(*it).first]->getParticles().insert
		(effect->getEmitterNodes()[(*it).first]->getParticles().begin() + (*it).second,
			ParticleInstance(particle));
	}

	effect->getParticleNodes().insert( effect->getParticleNodes().begin() + position, particle);
}


AddParticleCommand::AddParticleCommand(std::shared_ptr<EmitterNode> &em, std::shared_ptr<ParticleNode> p, size_t pos) :
	emitter{ em }, particle{ p }, position{ pos }
{

}

void AddParticleCommand::execute()
{
	emitter->getEmitter()->addParticle(particle->getParticle());
	emitter->getParticles().insert(emitter->getParticles().begin() + position, ParticleInstance(particle));
}

void AddParticleCommand::undo()
{
	emitter->getEmitter()->removeParticle(particle->getParticle());
	emitter->getParticles().erase(emitter->getParticles().begin() + position);
}


RemoveParticleCommand::RemoveParticleCommand(std::shared_ptr<EmitterNode> &em, std::shared_ptr<ParticleNode> p, size_t pos) :
	emitter{ em }, particle{ p }, position{ pos }
{

}

void RemoveParticleCommand::execute()
{
	emitter->getEmitter()->removeParticle(particle->getParticle());
	emitter->getParticles().erase(emitter->getParticles().begin() + position);
}

void RemoveParticleCommand::undo()
{
	emitter->getEmitter()->addParticle(particle->getParticle());
	emitter->getParticles().insert(emitter->getParticles().begin() + position, ParticleInstance(particle));
}
