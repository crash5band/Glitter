#pragma once
#include "Emitter.h"
#include <memory>

namespace Glitter
{
	class GlitterEffect
	{
	private:
		std::string name;
		float startTime;
		float lifeTime;
		Color color;
		Vector3 translation;
		Vector3 rotation;
		unsigned int flags;
		std::vector<Animation> animations;
		std::vector<std::shared_ptr<Emitter>> emitters;
		std::vector<std::shared_ptr<Particle>> particles;
		std::string filename;

		void prepare(tinyxml2::XMLDocument* xml);

	public:
		GlitterEffect(const std::string& name, float life);
		GlitterEffect(const std::string& filepath);
		~GlitterEffect() { animations.clear(); }

		std::string getName() const;
		float getStartTime() const;
		float getLifeTime() const;
		Color getColor() const;
		Vector3 getTranslation() const;
		Vector3 getRotation() const;
		unsigned int getFlags() const;
		std::vector<Animation> &getAnimations();
		std::vector<std::shared_ptr<Emitter>> getEmitters() const;
		std::vector<std::shared_ptr<Particle>> getParticles() const;
		std::string getFilename() const;

		void setName(std::string n);
		void setStartTime(float time);
		void setLifeTime(float time);
		void setColor(Color c);
		void setTranslation(Vector3 v);
		void setRotation(Vector3 v);
		void setFlags(unsigned int f);
		void addEmitter(std::shared_ptr<Emitter> emitter, int pos = -1);
		void removeEmitter(unsigned int index);
		void removeEmitter(std::shared_ptr<Emitter> emitter);
		void removeEmitterByID(unsigned int id);
		void addParticle(std::shared_ptr<Particle> particle, int pos = -1);
		void removeParticle(unsigned int index);
		void removeParticle(std::shared_ptr<Particle> particle);
		void removeParticleByID(unsigned int id);

		void read(const std::string& filename);
		void write(const std::string& filename);
		void writeXML(const std::string& filename);
	};
}

