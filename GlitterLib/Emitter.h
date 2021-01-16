#pragma once
#include "Particle.h"
#include "Animation.h"
#include "MathGens.h"
#include "GlitterEnums.h"
#include <memory>

namespace Glitter
{
	class Particle;

	class Emitter
	{
	private:
		unsigned int ID;
		std::string name;
		EmitterType type;

		float startTime;
		float lifeTime;
		float loopStartTime;
		float loopEndTime;
		Vector3 translation;
		Vector3 rotation;
		Vector3 rotationAdd;
		Vector3 rotationAddRandom;
		Vector3 scaling;
		EmitCondition emitCondition;
		EmitterDirectionType directionType;
		float emissionInterval;
		int particlesPerEmission;
		EmissionDirectionType emissionDirectionType;
		std::string mesh;
		int pointCount;
		unsigned int flags;
		std::vector<Animation> animations;
		std::vector<std::weak_ptr<Particle>> particles;

		// box only
		Vector3 size;

		// cylinder/sphere
		float radius;

		// cylinder only
		float height;
		float startAngle;
		float endAngle;

		// sphere only
		float latitude;
		float longitude;

		// mesh only
		std::string meshName;

		std::vector<unsigned int> pendingParticleIDs;

	public:
		Emitter();
		Emitter(const std::string& n);
		~Emitter() { animations.clear(); }

		unsigned int getID() const;
		std::string getName() const;
		EmitterType getType() const;
		float getStartTime() const;
		float getLifeTime() const;
		float getLoopStartTime() const;
		float getLoopEndTime() const;
		Vector3 getTranslation() const;
		Vector3 getRotation() const;
		Vector3 getRotationAdd() const;
		Vector3 getRotationAddRandom() const;
		Vector3 getScaling() const;
		EmitCondition getEmitCondition() const;
		EmitterDirectionType getDirectionType() const;
		float getEmissionInterval() const;
		int getParticlesPerEmission() const;
		Vector3 getSize() const;
		unsigned int getFlags() const;
		float getRadius() const;
		float getHeight() const;
		float getStartAngle() const;
		float getEndAngle() const;
		float getLatitude() const;
		float getLongitude() const;
		int getPointCount() const;
		EmissionDirectionType getEmissionDirectionType() const;
		std::string getMeshName() const;
		std::vector<std::weak_ptr<Particle>> getParticles() const;
		std::vector<Animation>& getAnimations();

		void setID(unsigned int id);
		void setName(std::string n);
		void setType(EmitterType t);
		void setStartTime(float time);
		void setLifeTime(float time);
		void setLoopStartTime(float time);
		void setLoopEndTime(float time);
		void setTranslation(Vector3 v);
		void setRotation(Vector3 v);
		void setRotationAdd(Vector3 v);
		void setRotationAddRandom(Vector3 v);
		void setScaling(Vector3 v);
		void setEmitCondition(EmitCondition condition);
		void setDirectionType(EmitterDirectionType type);
		void setEmissionInterval(float interval);
		void setParticlePerEmission(int count);
		void setSize(Vector3 v);
		void setFlags(unsigned int f);
		void setRadius(float r);
		void setHeight(float h);
		void setStartAngle(float a);
		void setEndAngle(float a);
		void setLatitude(float l);
		void setLongitude(float l);
		void setMeshName(std::string name);
		void setPointCount(int count);
		void setEmissionDirectionType(EmissionDirectionType type);
		void addParticle(std::weak_ptr<Particle> particle);
		void removeParticle(unsigned int index);
		void removeParticle(std::weak_ptr<Particle> particle);
		void removeParticleByID(unsigned int id);

		std::vector<unsigned int> getPendingParticleIDs() const;
		void clearPendingParticleIDs();

		void read(tinyxml2::XMLElement* element);
		void write(tinyxml2::XMLElement* element);
	};
}

