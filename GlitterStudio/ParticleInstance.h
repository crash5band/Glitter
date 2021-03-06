#pragma once
#include "Transform.h"
#include "ParticleNode.h"
#include "Camera.h"

struct ParticleStatus
{
	Transform transform;
	Glitter::Color color;
	Glitter::Vector3 basePos;
	Glitter::Vector3 direction;
	Glitter::Vector3 acceleration;
	Glitter::Vector3 rotation;
	Glitter::Vector3 scale;
	int UVIndex;
	float lastUVChange;
	float startTime;
	float time;
	bool dead;

	ParticleStatus() : dead{ true }
	{
	}
};

class ParticleInstance
{
private:
	std::shared_ptr<ParticleNode> reference;
	std::vector<ParticleStatus> pool;
	size_t rotationAddCount;
	size_t aliveCount;
	bool visible;

	void verifyPoolSize();

public:
	ParticleInstance(std::weak_ptr<ParticleNode> ref);

	void update(float time, Camera* camera, Transform& emitterTransform);
	void create(int count, float startTime, Glitter::EmissionDirectionType dir, std::vector<Glitter::Vector3>& pos, Glitter::Vector3& origin);
	void changeDirection(Glitter::ParticleDirectionType type, Camera* cam, Glitter::Vector3& pos, Glitter::Vector3 v, Glitter::Vector3& rot, Glitter::Vector3& o);
	void kill();
	void setVisible(bool val);
	bool isVisible() const;

	std::vector<ParticleStatus> &getPool();
	std::shared_ptr<ParticleNode> getReference() const;
	std::shared_ptr<Glitter::Particle> getParticle() const;
	size_t getAliveCount() const;
};

