#include "Emitter.h"
#include "BIXF.h"

namespace Glitter
{
	Emitter::Emitter() : ID{ 0 }, name{ "" }, type{ EmitterType::Box }, flags{ 0 }
	{
		startTime = 0.0f;
		lifeTime = 60.0f;
		loopStartTime = 0.0f;
		loopEndTime = -1.0f;
		scaling = Vector3(1.0f, 1.0f, 1.0f);
		emitCondition = EmitCondition::Time;
		directionType = EmitterDirectionType::ParentAxis;
		emissionInterval = 1.0f;
		particlesPerEmission = 1;
		radius = 0.5f;
		height = 0.1f;
		startAngle = 0.0f;
		endAngle = 360.0f;
		longitude = 180.0f;
		latitude = 360.0f;
		emissionDirectionType = EmissionDirectionType::Outward;
		pointCount = 3;
	}

	Emitter::Emitter(const std::string& n) :
		ID{ 0 }, name{ n }, startTime{ 0 }, lifeTime{ 60 }, loopStartTime{ 0 }, loopEndTime{ -1 }
	{
		type = EmitterType::Box;
		scaling = Vector3(1.0f, 1.0f, 1.0f);
		emitCondition = EmitCondition::Time;
		directionType = EmitterDirectionType::ParentAxis;
		emissionInterval = 1.0f;
		particlesPerEmission = 1;
		emissionDirectionType = EmissionDirectionType::Outward;
		radius = 0.5f;
		height = 0.1f;
		startAngle = 0.0f;
		endAngle = 360.0f;
		longitude = 180.0f;
		latitude = 360.0f;
		pointCount = 3;
		flags = 0;
	}

	unsigned int Emitter::getID() const
	{
		return ID;
	}

	std::string Emitter::getName() const
	{
		return name;
	}

	EmitterType Emitter::getType() const
	{
		return type;
	}

	float Emitter::getStartTime() const
	{
		return startTime;
	}

	float Emitter::getLifeTime() const
	{
		return lifeTime;
	}

	float Emitter::getLoopStartTime() const
	{
		return loopStartTime;
	}

	float Emitter::getLoopEndTime() const
	{
		return loopEndTime;
	}

	Vector3 Emitter::getTranslation() const
	{
		return translation;
	}

	Vector3 Emitter::getRotation() const
	{
		return rotation;
	}

	Vector3 Emitter::getRotationAdd() const
	{
		return rotationAdd;
	}

	Vector3 Emitter::getRotationAddRandom() const
	{
		return rotationAddRandom;
	}

	Vector3 Emitter::getScaling() const
	{
		return scaling;
	}

	EmitCondition Emitter::getEmitCondition() const
	{
		return emitCondition;
	}

	EmitterDirectionType Emitter::getDirectionType() const
	{
		return directionType;
	}

	float Emitter::getEmissionInterval() const
	{
		return emissionInterval;
	}

	int Emitter::getParticlesPerEmission() const
	{
		return particlesPerEmission;
	}

	Vector3 Emitter::getSize() const
	{
		return size;
	}

	unsigned int Emitter::getFlags() const
	{
		return flags;
	}

	float Emitter::getRadius() const
	{
		return radius;
	}

	float Emitter::getHeight() const
	{
		return height;
	}

	float Emitter::getStartAngle() const
	{
		return startAngle;
	}

	float Emitter::getEndAngle() const
	{
		return endAngle;
	}

	float Emitter::getLatitude() const
	{
		return latitude;
	}

	float Emitter::getLongitude() const
	{
		return longitude;
	}

	std::string Emitter::getMeshName() const
	{
		return meshName;
	}

	int Emitter::getPointCount() const
	{
		return pointCount;
	}

	EmissionDirectionType Emitter::getEmissionDirectionType() const
	{
		return emissionDirectionType;
	}

	std::vector<std::weak_ptr<Particle>> Emitter::getParticles() const
	{
		return particles;
	}

	std::vector<Animation>& Emitter::getAnimations()
	{
		return animations;
	}

	void Emitter::setID(unsigned int id)
	{
		ID = id;
	}

	void Emitter::setName(std::string n)
	{
		if (n.size())
			name = n;
	}

	void Emitter::setType(EmitterType t)
	{
		type = t;
	}

	void Emitter::setStartTime(float time)
	{
		if (time >= 0)
			startTime = time;
	}

	void Emitter::setLifeTime(float time)
	{
		if (time >= 0)
			lifeTime = time;
	}

	void Emitter::setLoopStartTime(float time)
	{
		loopStartTime = time;
	}

	void Emitter::setLoopEndTime(float time)
	{
		loopEndTime = time;
	}

	void Emitter::setTranslation(Vector3 v)
	{
		translation = v;
	}

	void Emitter::setRotation(Vector3 v)
	{
		rotation = v;
	}

	void Emitter::setRotationAdd(Vector3 v)
	{
		rotationAdd = v;
	}

	void Emitter::setRotationAddRandom(Vector3 v)
	{
		rotationAddRandom = v;
	}

	void Emitter::setScaling(Vector3 v)
	{
		scaling = v;
	}

	void Emitter::setEmitCondition(EmitCondition condition)
	{
		emitCondition = condition;
	}

	void Emitter::setDirectionType(EmitterDirectionType type)
	{
		directionType = type;
	}

	void Emitter::setEmissionInterval(float interval)
	{
		emissionInterval = interval;
	}

	void Emitter::setParticlePerEmission(int count)
	{
		particlesPerEmission = count;
	}

	void Emitter::setSize(Vector3 v)
	{
		size = v;
	}

	void Emitter::setFlags(unsigned int f)
	{
		flags = f;
	}

	void Emitter::setRadius(float r)
	{
		if (r >= 0)
			radius = r;
	}

	void Emitter::setHeight(float h)
	{
		if (h >= 0)
			height = h;
	}

	void Emitter::setStartAngle(float a)
	{
		if (a <= endAngle)
			startAngle = a;
		else
			startAngle = endAngle;
	}

	void Emitter::setEndAngle(float a)
	{
		if (a >= startAngle)
			endAngle = a;
		else
			endAngle = startAngle;
	}

	void Emitter::setLatitude(float l)
	{
		latitude = l;
	}

	void Emitter::setLongitude(float l)
	{
		longitude = l;
	}

	void Emitter::setMeshName(std::string name)
	{
		meshName = name;
	}

	void Emitter::setPointCount(int count)
	{
		pointCount = count;
	}

	void Emitter::setEmissionDirectionType(EmissionDirectionType type)
	{
		emissionDirectionType = type;
	}

	void Emitter::addParticle(std::weak_ptr<Particle> particle)
	{
		particles.push_back(particle);
	}

	void Emitter::removeParticle(unsigned int index)
	{
		if (index >= particles.size())
			return;

		particles.erase(particles.begin() + index);
	}

	void Emitter::removeParticle(std::weak_ptr<Particle> particle)
	{
		for (std::vector<std::weak_ptr<Particle>>::iterator it = particles.begin(); it != particles.end();)
		{
			if ((*it).lock() == particle.lock())
			{
				it = particles.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void Emitter::removeParticleByID(unsigned int id)
	{
		for (std::vector<std::weak_ptr<Particle>>::iterator it = particles.begin(); it != particles.end(); ++it)
		{
			if ((*it).lock()->getID() == id)
			{
				particles.erase(it);
				return;
			}
		}
	}

	std::vector<unsigned int> Emitter::getPendingParticleIDs() const
	{
		return pendingParticleIDs;
	}

	void Emitter::clearPendingParticleIDs()
	{
		pendingParticleIDs.clear();
	}

	void Emitter::read(tinyxml2::XMLElement* element)
	{
		ID = element->FindAttribute("Id")->UnsignedValue();
		name = element->FindAttribute("Name")->Value();
		type = (EmitterType)glitterStringToEnum(emitterTypeTable, emitterTypeTableSize, element->FindAttribute("Type")->Value());

		startTime			= BIXF::toFloat(element->FirstChildElement("StartTime"));
		lifeTime			= BIXF::toFloat(element->FirstChildElement("LifeTime"));
		loopStartTime		= BIXF::toFloat(element->FirstChildElement("LoopStartTime"));
		loopEndTime			= BIXF::toFloat(element->FirstChildElement("LoopEndTime"));
		translation			= BIXF::toVector3(element->FirstChildElement("Translation"));
		rotation			= BIXF::toVector3(element->FirstChildElement("Rotation"));
		rotationAdd			= BIXF::toVector3(element->FirstChildElement("RotationAdd"));
		rotationAddRandom	= BIXF::toVector3(element->FirstChildElement("RotationAddRandom"));
		scaling				= BIXF::toVector3(element->FirstChildElement("Scaling"));
		
		std::string emitStr = BIXF::toString(element->FirstChildElement("EmitCondition"));
		emitCondition = (EmitCondition)glitterStringToEnum(emitConditionTable, emitConditionTableSize, emitStr);

		std::string dirStr	= BIXF::toString(element->FirstChildElement("DirectionType"));
		directionType = (EmitterDirectionType)glitterStringToEnum(EdirectionTypeTable, EdirectionTypeTableSize, dirStr);

		std::string emDirStr = BIXF::toString(element->FirstChildElement("EmissionDirectionType"));
		emissionDirectionType = (EmissionDirectionType)glitterStringToEnum(emissionDirectionTypeTable, emissionDirectionTypeTableSize, emDirStr);

		emissionInterval = BIXF::toFloat(element->FirstChildElement("EmissionInterval"));
		particlesPerEmission = BIXF::toInt(element->FirstChildElement("ParticlePerEmission"));

		switch (type)
		{
		case EmitterType::Box:
			size		= BIXF::toVector3(element->FirstChildElement("Size"));
			break;

		case EmitterType::Cylinder:
			radius		= BIXF::toFloat(element->FirstChildElement("Radius"));
			height		= BIXF::toFloat(element->FirstChildElement("Height"));
			startAngle	= BIXF::toFloat(element->FirstChildElement("StartAngle"));
			endAngle	= BIXF::toFloat(element->FirstChildElement("EndAngle"));
			break;

		case EmitterType::Sphere:
			radius		= BIXF::toFloat(element->FirstChildElement("Radius"));
			longitude	= BIXF::toFloat(element->FirstChildElement("Longitude"));
			latitude	= BIXF::toFloat(element->FirstChildElement("Latitude"));
			break;

		case EmitterType::Mesh:
			meshName	= BIXF::toString(element->FirstChildElement("MeshName"));
			break;

		case EmitterType::Polygon:
			pointCount	= BIXF::toInt(element->FirstChildElement("PointCount"));
			break;

		default:
			break;
		}

		flags = BIXF::toUInt(element->FirstChildElement("Flags"));
		
		tinyxml2::XMLElement* particleElement = element->FirstChildElement("Particle");
		while (particleElement)
		{
			// read the particle IDs and store them to add them later to the emitter
			pendingParticleIDs.push_back(particleElement->FindAttribute("Id")->UnsignedValue());
			particleElement = particleElement->NextSiblingElement("Particle");
		}

		tinyxml2::XMLElement* animationElement = element->FirstChildElement("Animation");
		while (animationElement)
		{
			Animation animation;
			animation.read(animationElement);
			animations.emplace_back(animation);
			animationElement = animationElement->NextSiblingElement("Animation");
		}
	}

	void Emitter::write(tinyxml2::XMLElement* element)
	{
		element->SetAttribute("Id", ID);
		element->SetAttribute("Name", name.c_str());
		element->SetAttribute("Type", glitterEnumToString(emitterTypeTable, emitterTypeTableSize, (size_t)type).c_str());

		BIXF::createChildValue(element, "StartTime", startTime);
		BIXF::createChildValue(element, "LifeTime", lifeTime);
		BIXF::createChildValue(element, "LoopStartTime", loopStartTime);
		BIXF::createChildValue(element, "LoopEndTime", loopEndTime);
		BIXF::createChildVector3(element, "Translation", translation);
		BIXF::createChildVector3(element, "Rotation", rotation);
		BIXF::createChildVector3(element, "RotationAdd", rotationAdd);
		BIXF::createChildVector3(element, "RotationAddRandom", rotationAddRandom);
		BIXF::createChildVector3(element, "Scaling", scaling);

		BIXF::createChildValue(element, "EmitCondition", glitterEnumToString(emitConditionTable, emitConditionTableSize, (size_t)emitCondition).c_str());
		BIXF::createChildValue(element, "DirectionType", glitterEnumToString(EdirectionTypeTable, EdirectionTypeTableSize, (size_t)directionType).c_str());
		BIXF::createChildValue(element, "EmissionInterval", emissionInterval);
		BIXF::createChildValue(element, "ParticlePerEmission", particlesPerEmission);

		std::string emDirStr = glitterEnumToString(emissionDirectionTypeTable, emissionDirectionTypeTableSize, (size_t)emissionDirectionType);

		switch (type)
		{
		case EmitterType::Box:
			BIXF::createChildVector3(element, "Size", size);
			break;

		case EmitterType::Cylinder:
			BIXF::createChildValue(element, "Radius", radius);
			BIXF::createChildValue(element, "Height", height);
			BIXF::createChildValue(element, "StartAngle", startAngle);
			BIXF::createChildValue(element, "EndAngle", endAngle);
			BIXF::createChildValue(element, "EmissionDirectionType", emDirStr);
			break;

		case EmitterType::Sphere:
			BIXF::createChildValue(element, "Radius", radius);
			BIXF::createChildValue(element, "Latitude", latitude);
			BIXF::createChildValue(element, "Longitude", longitude);
			BIXF::createChildValue(element, "EmissionDirectionType", emDirStr);
			break;

		case EmitterType::Mesh:
			BIXF::createChildValue(element, "MeshName", meshName);
			break;

		case EmitterType::Polygon:
			BIXF::createChildValue(element, "PointCount", pointCount);
			break;

		default:
			break;
		}

		BIXF::createChildValue(element, "Flags", flags);

		for (unsigned int count = 0; count < animations.size(); ++count)
		{
			tinyxml2::XMLElement* animationElement = element->InsertNewChildElement("Animation");
			animations[count].write(animationElement);
		}

		for (const std::weak_ptr<Particle>& particle : particles)
		{
			tinyxml2::XMLElement* particleElement = element->InsertNewChildElement("Particle");
			particleElement->SetAttribute("Id", particle.lock()->getID());
			particleElement->SetAttribute("Name", particle.lock()->getName().c_str());
		}
	}
}