#pragma once
#include "ICommand.h"
#include "EffectNode.h"
#include <unordered_map>

class CreateEmitterCommand : public ICommand
{
private:
	std::weak_ptr<EffectNode> effect;
	std::weak_ptr<EmitterNode> emitter;
	size_t position;

public:
	CreateEmitterCommand(std::shared_ptr<EffectNode> &eff, std::shared_ptr<EmitterNode> &em, size_t pos);

	void execute() override;
	void undo() override;
	const char* getDescription() override { return "Create Emitter"; }
	bool isValid() override { return !effect.expired(); }
};

class DeleteEmitterCommand : public ICommand
{
private:
	std::weak_ptr<EffectNode> effect;
	std::weak_ptr<EmitterNode> emitter;
	size_t position;

public:
	DeleteEmitterCommand(std::shared_ptr<EffectNode> &eff, std::shared_ptr<EmitterNode> &em, size_t pos);

	void execute() override;
	void undo() override;
	const char* getDescription() override { return "Delete Emitter"; }
	bool isValid() override { return !effect.expired(); }
};

class CreateParticleCommand : public ICommand
{
private:
	std::weak_ptr<EffectNode> effect;
	std::weak_ptr<ParticleNode> particle;
	size_t position;

public:
	CreateParticleCommand(std::shared_ptr<EffectNode> &eff, std::shared_ptr<ParticleNode> &p, size_t pos);

	void execute() override;
	void undo() override;
	const char* getDescription() override { return "Create Particle"; }
	bool isValid() override { return !effect.expired(); }
};

class DeleteParticleCommand : public ICommand
{
private:
	std::weak_ptr<EffectNode> effect;
	std::weak_ptr<ParticleNode> particle;
	size_t position;
	std::unordered_map<size_t, size_t> emitterParticles;

public:
	DeleteParticleCommand(std::shared_ptr<EffectNode> &eff, std::shared_ptr<ParticleNode> &p, size_t pos);

	void execute() override;
	void undo() override;
	const char* getDescription() override { return "Delete Particle"; }
	bool isValid() override { return !effect.expired(); }
};

class AddParticleCommand : public ICommand
{
private:
	std::weak_ptr<EmitterNode> emitter;
	std::weak_ptr<ParticleNode> particle;
	size_t position;

public:
	AddParticleCommand(std::shared_ptr<EmitterNode> &em, std::shared_ptr<ParticleNode> p, size_t pos);

	void execute() override;
	void undo() override;
	const char* getDescription() override { return "Add Particle"; }
	bool isValid() override { return !emitter.expired(); }
};

class RemoveParticleCommand : public ICommand
{
private:
	std::weak_ptr<EmitterNode> emitter;
	std::weak_ptr<ParticleNode> particle;
	size_t position;

public:
	RemoveParticleCommand(std::shared_ptr<EmitterNode> &em, std::shared_ptr<ParticleNode> p, size_t pos);

	void execute() override;
	void undo() override;
	const char* getDescription() override { return "Remove Particle"; }
	bool isValid() override { return !emitter.expired(); }
};
