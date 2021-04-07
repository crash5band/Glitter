#include "GlitterEffect.h"
#include "BIXF.h"

namespace Glitter
{
	GlitterEffect::GlitterEffect(const std::string& name, float life) :
		name{ name }, startTime{ 0.0f }, lifeTime{ life }, flags{ 0 }
	{
	}

	GlitterEffect::GlitterEffect(const std::string& filepath) :
		filename{ filepath }
	{
		read(filepath);
	}

	std::string GlitterEffect::getFilename() const
	{
		return filename;
	}

	std::string GlitterEffect::getName() const
	{
		return name;
	}

	float GlitterEffect::getStartTime() const
	{
		return startTime;
	}

	float GlitterEffect::getLifeTime() const
	{
		return lifeTime;
	}

	Color GlitterEffect::getColor() const
	{
		return color;
	}

	Vector3 GlitterEffect::getTranslation() const
	{
		return translation;
	}

	Vector3 GlitterEffect::getRotation() const
	{
		return rotation;
	}

	unsigned int GlitterEffect::getFlags() const
	{
		return flags;
	}

	std::vector<Animation> &GlitterEffect::getAnimations()
	{
		return animations;
	}

	std::vector<std::shared_ptr<Emitter>> GlitterEffect::getEmitters() const
	{
		return emitters;
	}

	std::vector<std::shared_ptr<Particle>> GlitterEffect::getParticles() const
	{
		return particles;
	}

	void GlitterEffect::setName(std::string n)
	{
		if (n.size())
			name = n;
	}

	void GlitterEffect::setStartTime(float time)
	{
		if (time >= 0)
			startTime = time;
	}

	void GlitterEffect::setLifeTime(float time)
	{
		if (time >= 0)
			lifeTime = time;
	}

	void GlitterEffect::setColor(Color c)
	{
		color = c;
	}

	void GlitterEffect::setTranslation(Vector3 v)
	{
		translation = v;
	}

	void GlitterEffect::setRotation(Vector3 v)
	{
		rotation = v;
	}

	void GlitterEffect::setFlags(unsigned int f)
	{
		flags = f;
	}

	void GlitterEffect::addEmitter(std::shared_ptr<Emitter> emitter, int pos)
	{
		if (pos < 0)
			emitters.push_back(emitter);
		else
			emitters.insert(emitters.begin() + pos, emitter);
	}

	void GlitterEffect::removeEmitter(unsigned int index)
	{
		if (index >= emitters.size())
			return;

		emitters.erase(emitters.begin() + index);
	}

	void GlitterEffect::removeEmitterByID(unsigned int id)
	{
		for (std::vector<std::shared_ptr<Emitter>>::iterator it = emitters.begin(); it != emitters.end(); ++it)
		{
			if ((*it)->getID() == id)
			{
				emitters.erase(it);
				return;
			}
		}
	}

	void GlitterEffect::removeEmitter(std::shared_ptr<Emitter> emitter)
	{
		for (std::vector<std::shared_ptr<Emitter>>::iterator it = emitters.begin(); it != emitters.end(); ++it)
		{
			if ((*it) == emitter)
			{
				emitters.erase(it);
				return;
			}
		}
	}

	void GlitterEffect::addParticle(std::shared_ptr<Particle> particle, int pos)
	{
		if (pos < 0)
			particles.push_back(particle);
		else
			particles.insert(particles.begin() + pos, particle);

	}

	void GlitterEffect::removeParticle(unsigned int index)
	{
		if (index >= particles.size())
			return;

		std::vector<std::shared_ptr<Particle>>::iterator it = particles.begin() + index;
		for (std::vector<std::shared_ptr<Emitter>>::iterator eIt = emitters.begin(); eIt != emitters.end(); ++eIt)
		{
			(*eIt)->removeParticle((*it));
		}

		particles.erase(it);
	}

	void GlitterEffect::removeParticleByID(unsigned int id)
	{
		std::vector<std::shared_ptr<Particle>>::iterator it;
		for (it = particles.begin(); it != particles.end(); ++it)
		{
			if ((*it)->getID() == id)
			{
				unsigned int particleId = (*it)->getID();
				for (std::vector<std::shared_ptr<Emitter>>::iterator eIt = emitters.begin(); eIt != emitters.end(); ++eIt)
				{
					(*eIt)->removeParticleByID(particleId);
				}

				break;
			}
		}

		particles.erase(it);
	}

	void GlitterEffect::removeParticle(std::shared_ptr<Particle> particle)
	{
		std::vector<std::shared_ptr<Particle>>::iterator it;
		for (it = particles.begin(); it != particles.end(); ++it)
		{
			if ((*it) == particle)
			{
				for (std::vector<std::shared_ptr<Emitter>>::iterator eIt = emitters.begin(); eIt != emitters.end(); ++eIt)
				{
					(*eIt)->removeParticle(particle);
				}

				break;
			}
		}

		particles.erase(it);
	}

	void GlitterEffect::read(const std::string& filename)
	{
		tinyxml2::XMLDocument* xml = BIXF::parseBIXF(filename);
		if (!xml)
		{
			delete xml;
			return;
		}

		tinyxml2::XMLElement* effectElement = xml->FirstChildElement("Effect");

		name		= effectElement->FirstAttribute()->Value();
		startTime	= BIXF::toFloat(effectElement->FirstChildElement("StartTime"));
		lifeTime	= BIXF::toFloat(effectElement->FirstChildElement("LifeTime"));
		color		= BIXF::toColor(effectElement->FirstChildElement("Color"));
		translation = BIXF::toVector3(effectElement->FirstChildElement("Translation"));
		rotation	= BIXF::toVector3(effectElement->FirstChildElement("Rotation"));
		flags		= BIXF::toUInt(effectElement->FirstChildElement("Flags"));

		tinyxml2::XMLElement* animationElement = effectElement->FirstChildElement("Animation");
		while (animationElement)
		{
			Animation animation;
			animation.read(animationElement);
			animations.push_back(animation);
			animationElement = animationElement->NextSiblingElement("Animation");
		}

		tinyxml2::XMLElement* particleElement = effectElement->FirstChildElement("Particle");
		while (particleElement)
		{
			std::shared_ptr<Particle> particle = std::make_shared<Particle>();
			particle->read(particleElement);
			particles.push_back(particle);
			particleElement = particleElement->NextSiblingElement("Particle");
		}

		tinyxml2::XMLElement* emitterElement = effectElement->FirstChildElement("Emitter");
		while (emitterElement)
		{
			std::shared_ptr<Emitter> emitter = std::make_shared<Emitter>();
			emitter->read(emitterElement);

			// add references to the particles used by each emitter to the emitter
			std::vector<unsigned int> pIds = emitter->getPendingParticleIDs();
			for (unsigned int count = 0; count < pIds.size(); ++count)
			{
				for (std::vector<std::shared_ptr<Particle>>::iterator it = particles.begin(); it != particles.end(); ++it)
					if ((*it)->getID() == pIds[count])
						emitter->addParticle((*it));
			}

			emitter->clearPendingParticleIDs();

			emitters.push_back(emitter);
			emitterElement = emitterElement->NextSiblingElement("Emitter");
		}

		for (unsigned int count = 0; count < particles.size(); ++count)
		{
			std::vector<unsigned int> children = particles[count]->getPendingChildEmitters();
			for (int i = 0; i < children.size(); ++i)
			{
				for (int j = 0; j < emitters.size(); ++j)
				{
					if (emitters[j]->getID() == children[i])
						particles[count]->addChildEmitter(emitters[j]);
				}
			}

			particles[count]->clearPendingChildEmitters();
		}

		delete xml;
	}

	void GlitterEffect::prepare(tinyxml2::XMLDocument* xml)
	{
		tinyxml2::XMLElement* effectElement = xml->NewElement("Effect");

		effectElement->SetAttribute("Name", name.c_str());
		BIXF::createChildValue(effectElement, "StartTime", startTime);
		BIXF::createChildValue(effectElement, "LifeTime", lifeTime);
		BIXF::createChildColor(effectElement, "Color", color);
		BIXF::createChildVector3(effectElement, "Translation", translation);
		BIXF::createChildVector3(effectElement, "Rotation", rotation);
		BIXF::createChildValue(effectElement, "Flags", flags);

		unsigned int count;
		for (count = 0; count < animations.size(); ++count)
		{
			tinyxml2::XMLElement* animationElement = effectElement->InsertNewChildElement("Animation");
			animations[count].write(animationElement);
		}

		for (count = 0; count < emitters.size(); ++count)
		{
			emitters[count]->setID(count);
			tinyxml2::XMLElement* emitterElement = effectElement->InsertNewChildElement("Emitter");
			emitters[count]->write(emitterElement);
		}

		for (count = 0; count < particles.size(); ++count)
		{
			particles[count]->setID(count);
			tinyxml2::XMLElement* particleElement = effectElement->InsertNewChildElement("Particle");
			particles[count]->write(particleElement);
		}

		xml->InsertEndChild(effectElement);
	}

	void GlitterEffect::write(const std::string& filename)
	{
		tinyxml2::XMLDocument* xml = new tinyxml2::XMLDocument();
		prepare(xml);
		this->filename = filename;

		BIXF::convertToBIXF(xml, filename);
		delete xml;
	}

	void GlitterEffect::writeXML(const std::string& filename)
	{
		tinyxml2::XMLDocument* xml = new tinyxml2::XMLDocument();
		prepare(xml);

		xml->SaveFile(filename.c_str());
		delete xml;
	}
}