#include "ParticleInstance.h"
#include "Utilities.h"

ParticleInstance::ParticleInstance(std::weak_ptr<ParticleNode> nodeRef) :
	reference{ nodeRef }, aliveCount{ 0 }, rotationAddCount{ 0 }, visible{ true }
{
	size_t count = reference->getParticle()->getMaxCount();
	pool.reserve(count);
	for (size_t i = 0; i < count; ++i)
		pool.emplace_back();
}

std::vector<ParticleStatus> &ParticleInstance::getPool()
{
	return pool;
}

std::shared_ptr<ParticleNode> ParticleInstance::getReference() const
{
	return reference;
}

std::shared_ptr<Glitter::Particle> ParticleInstance::getParticle() const
{
	return reference->getParticle();
}

size_t ParticleInstance::getAliveCount() const
{
	return aliveCount;
}

void ParticleInstance::verifyPoolSize()
{
	// resize pool if MaxCount is changed
	auto &particle = reference->getParticle();
	size_t size = pool.size();

	if (particle->getMaxCount() > size)
	{
		pool.reserve(particle->getMaxCount());

		for (size_t i = 0; i < particle->getMaxCount() - size; ++i)
			pool.emplace_back();
	}
	else if (particle->getMaxCount() < pool.size())
	{
		for (size_t i = 0; i < size - particle->getMaxCount(); ++i)
			pool.pop_back();

		//pool.shrink_to_fit();
	}
}

void ParticleInstance::kill()
{
	for (auto& p : pool)
		p.dead = true;
}

void ParticleInstance::setVisible(bool val)
{
	visible = val;
}

bool ParticleInstance::isVisible() const
{
	return visible;
}

void ParticleInstance::changeDirection(Glitter::ParticleDirectionType type, Camera* camera, Transform& t, Glitter::Vector3& origin)
{
	switch (type)
	{
	case Glitter::ParticleDirectionType::Billboard:
		t.rotation.x = -camera->getPitch();
		t.rotation.y = 90.0f - camera->getYaw();
		break;

	case Glitter::ParticleDirectionType::DirectionalAngle:
		break;

	case Glitter::ParticleDirectionType::DirectionalAngleBillboard:
		t.rotation.x = -camera->getPitch();
		t.rotation.y = 90.0f - camera->getYaw();
		break;

	case Glitter::ParticleDirectionType::EmitterDirection:
	{
		Glitter::Vector3 pPos = t.position - origin;
		pPos.normalise();

		t.rotation.x = Utilities::toDegrees(asinf(-pPos.y));
		t.rotation.y = Utilities::toDegrees(atan2f(pPos.x, pPos.z));
	}
	break;

	case Glitter::ParticleDirectionType::XAxis:
		t.rotation.y = 90.0f;
		t.rotation.z = 0.0f;
		break;

	case Glitter::ParticleDirectionType::YAxis:
		t.rotation.x = -90.0f;
		t.rotation.z = 0.0f;
		break;

	case Glitter::ParticleDirectionType::ZAxis:
		t.rotation.x = 0.0f;
		t.rotation.y = 0.0f;
		break;

	case Glitter::ParticleDirectionType::YRotationOnly:
		t.rotation.y = 90.0f - camera->getYaw();
		break;
	}
}

void ParticleInstance::update(float time, Camera* camera, Transform& baseTransform, Transform& emitterTransform)
{
	verifyPoolSize();
	auto &particle = reference->getParticle();
	auto animNode = reference->getAnimationNode();

	unsigned int maxUV = 0;
	int interval = particle->getUVChangeInterval();
	Glitter::UVIndexType type = particle->getUVIndexType();
	if (reference->getMaterialNode().get())
	{
		Glitter::Vector2 uvSplit = reference->getMaterialNode()->getMaterial()->getSplit();
		maxUV = ((int)uvSplit.x * (int)uvSplit.y) - 1;
	}

	// We need the emitter's rotation here to be able to transform the particle's velocity
	// according to the direction the emitter is facing if flag 4 (emitter local) is enabled
	float rX1 = Utilities::toRadians(baseTransform.rotation.x);
	float rY1 = Utilities::toRadians(baseTransform.rotation.y);
	float rZ1 = Utilities::toRadians(baseTransform.rotation.z);
	float rX2 = Utilities::toRadians(emitterTransform.rotation.x);
	float rY2 = Utilities::toRadians(emitterTransform.rotation.y);
	float rZ2 = Utilities::toRadians(emitterTransform.rotation.z);

	Glitter::Matrix3 m3;
	Glitter::Matrix3 m3L;
	Glitter::Matrix4 m4;
	Glitter::Matrix4 m4L;
	m3.fromEulerAnglesZYX(rZ1, rY1, rX1);
	m3L.fromEulerAnglesZYX(rZ2, rY2, rX2);

	Glitter::Vector3 pos;

	aliveCount = 0;
	for (auto& p : pool)
	{
		if (p.time > particle->getLifeTime() || p.time < 0.0f)
		{
			p.dead = true;
			continue;
		}

		if (p.dead)
			continue;
	
		p.time = time - p.startTime;

		// calculate base parameters
		p.velocity = p.direction + (p.acceleration * p.time);
		p.transform = baseTransform;
		Glitter::Vector3 basePos = p.basePos;

		// Update Position
		if (particle->getFlags() & 4)
		{
			basePos += -p.origin + emitterTransform.position + animNode->tryGetTranslation(p.time);

			m4L.makeTransform(pos, baseTransform.scale, m3L);
			p.velocity = m4L * p.velocity;
		}

		p.transform.position += basePos;
		m4.makeTransform(pos, baseTransform.scale, m3);
		p.transform.position = (m4 * p.transform.position) + ((p.velocity + p.direactionAdd) * p.time);
		if (!(particle->getFlags() & 4))
			p.transform.position += animNode->tryGetTranslation(p.time);

		// Update Rotation
		p.transform.rotation += p.rotation + animNode->tryGetRotation(p.time);
		changeDirection(particle->getDirectionType(), camera, p.transform, emitterTransform.position);

		// Update scale
		p.transform.scale *= animNode->tryGetScale(p.time);
		if (particle->getType() != Glitter::ParticleType::Mesh)
		{
			// FLAGS: Uniform Scale
			if (particle->getFlags() & 16)
				p.transform.scale *= p.scale.x;
			else
				p.transform.scale *= p.scale;
		}

		// Update Color
		p.color = particle->getColor() * reference->getBaseColor() * animNode->tryGetColor(p.time);

		// Update UVs
		if (particle->getUVIndexType() == Glitter::UVIndexType::Fixed)
		{
			p.UVIndex = particle->getUVIndex();
		}
		else if (maxUV && interval && !((int)p.time % interval) && (round(p.lastUVChange) != round(p.time)))
		{
			switch (type)
			{
			case Glitter::UVIndexType::ReverseOrder:
			case Glitter::UVIndexType::InitialRandomReverseOrder:
				--p.UVIndex;
				if (p.UVIndex < 0)
					p.UVIndex = maxUV;
				break;

			case Glitter::UVIndexType::SequentialOrder:
			case Glitter::UVIndexType::InitialRandomSequentialOrder:
				p.UVIndex = (p.UVIndex + 1) % maxUV;
				break;

			case Glitter::UVIndexType::RandomOrder:
				p.UVIndex = Utilities::random(0, maxUV);
				break;
			}

			p.lastUVChange = p.time;
		}

		++aliveCount;
	}
}

void ParticleInstance::create(int n, float startTime, Glitter::EmissionDirectionType dir, std::vector<Glitter::Vector3>& basePos, Glitter::Vector3& origin)
{	
	int count = 0;
	for (std::vector<ParticleStatus>::iterator it = pool.begin(); it != pool.end(); ++it)
	{
		if ((*it).dead)
		{
			ParticleStatus &p	= (*it);
			auto &particle		= reference->getParticle();
			p.startTime			= startTime;
			p.time				= 0.0f;
			p.dead				= false;
			p.origin			= origin;

			float speed					= Utilities::randomize(particle->getSpeed(), particle->getSpeedRandom());
			float deceleration			= Utilities::randomize(particle->getDeceleration(),	particle->getDecelerationRandom());
			Glitter::Vector3 direction	= Utilities::randomize(particle->getDirection(), particle->getDirectionRandom());
			Glitter::Vector3 accel		= Utilities::randomize(particle->getExternalAccel(), particle->getExternalAccelRandom());
			Glitter::Vector3 dirAdd		= Glitter::Vector3(0, 0, 0);

			switch (dir)
			{
			case Glitter::EmissionDirectionType::Inward:
				dirAdd = origin - basePos[count];
				break;

			case Glitter::EmissionDirectionType::Outward:
				dirAdd = basePos[count] - origin;
				break;

			default:
				break;
			}

			dirAdd.normalise();
			p.direction		= direction * speed;
			p.direactionAdd = dirAdd * speed;
			p.basePos		= basePos[count];
			p.acceleration	= (accel - deceleration + particle->getGravitationalAccel()) / 3600.0f;
			p.scale			= Utilities::randomize(particle->getSize(),	particle->getSizeRandom());	
			p.UVIndex		= particle->getUVIndex();
			p.lastUVChange	= p.time;

			Glitter::Vector3 rotation = Utilities::randomize(particle->getRotation(), particle->getRotationRandom());
			Glitter::Vector3 rotationAdd = Utilities::randomize(particle->getRotationAdd(), particle->getRotationAddRandom());
			p.rotation = rotation + rotationAdd * (rotationAddCount++ % particle->getMaxCount());

			// InitialRandom UVIndex Types
			if ((size_t)particle->getUVIndexType() >= 1 && (size_t)particle->getUVIndexType() < 5)
			{
				Glitter::Vector2 uvSplit = Glitter::Vector2(1.0f, 1.0f);
				if (reference->getMaterialNode())
					uvSplit = reference->getMaterialNode()->getMaterial()->getSplit();

				unsigned int maxUV = (uvSplit.x * uvSplit.y) - 1;
				p.UVIndex = Utilities::random(0, maxUV);
			}

			++count;
		}

		if (count >= n)
			return;
	}
}
