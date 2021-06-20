#pragma once
#include "GlitterAnimation.h"
#include "Emitter.h"
#include "MathGens.h"
#include "GlitterEnums.h"
#include "tinyxml2.h"
#include <string>
#include <vector>
#include <memory>

namespace Glitter
{
	class Emitter;

	class Particle
	{
	private:
		unsigned int ID;
		std::string name;
		ParticleType type;
		std::vector<GlitterAnimation> animations;
		std::vector<std::weak_ptr<Emitter>> childEmitters;
		std::vector<unsigned int> pendingChildEmitters;

		float lifeTime;
		PivotPosition pivotPosition;
		ParticleDirectionType directionType;
		float zOffset;
		Vector3 size;
		Vector3 sizeRandom;
		Vector3 rotation;
		Vector3 rotationRandom;
		Vector3 rotationAdd;
		Vector3 rotationAddRandom;
		Vector3 direction;
		Vector3 directionRandom;
		float speed;
		float speedRandom;
		Vector3 gravitationalAccel;
		Vector3 externalAccel;
		Vector3 externalAccelRandom;
		float deceleration;
		float decelrationRandom;
		float emitterTranslationEffectRatio;
		float followEmitterTranslationRatio;
		float followEmitterTranslationYRatio;
		float reflectionCoeff;
		float reflectionCoeffRandom;
		float reboundPlaneY;
		float childEmitterTime;
		int maxCount;
		std::string meshName; // mesh
		int locusHistorySize; // locus
		int locusHistorySizeRandom; // locus
		Color color;
		unsigned int textureIndex;
		UVIndexType uvIndexType;
		unsigned int uvIndex;
		unsigned int uvIndexStart;
		unsigned int uvIndexEnd;
		float uvChangeInterval;
		Vector2 colorScroll;
		Vector2 colorScrollRandom;
		float colorScrollSpeed;
		Vector2 alphaScroll;
		Vector2 alphaScrollRandom;
		float alphaScrollSpeed;
		Vector2 secondaryColorScroll;
		Vector2 secondaryColorScrollRandom;
		float secondaryColorScrollSpeed;
		Vector2 secondaryAlphaScroll;
		Vector2 secondaryAlphaScrollRandom;
		float secondaryAlphaScrollSpeed;
		std::string material;
		BlendMode blendMode;
		BlendMode secondaryBlendMode;
		float secondaryBlend;
		AddressMode addressMode;
		unsigned int flags;

	public:
		Particle();
		Particle(const std::string& name);

		unsigned int getID() const;
		std::string getName() const;
		ParticleType getType() const;
		float getLifeTime() const;
		PivotPosition getPivotPosition() const;
		ParticleDirectionType getDirectionType() const;
		float getZOffset() const;
		Vector3 getSize() const;
		Vector3 getSizeRandom() const;
		Vector3 getRotation() const;
		Vector3 getRotationAdd() const;
		Vector3 getRotationRandom() const;
		Vector3 getRotationAddRandom() const;
		Vector3 getDirection() const;
		Vector3 getDirectionRandom() const;
		float getSpeed() const;
		float getSpeedRandom() const;
		Vector3 getGravitationalAccel() const;
		Vector3 getExternalAccel() const;
		Vector3 getExternalAccelRandom() const;
		float getDeceleration() const;
		float getDecelerationRandom() const;
		float getEmitterTranslationEffectRatio() const;
		float getFollowEmitterTranslationRatio() const;
		float getFollowEmitterTranslationYRatio() const;
		float getReflectionCoeff() const;
		float getReflectionCoeffRandom() const;
		float getReboundPlaneY() const;
		float getChildEmitterTime() const;
		std::string getMeshName() const;
		int getLocusHistorySize() const;
		int getLocusHistorySizeRandom() const;
		int getMaxCount() const;
		Color getColor() const;
		unsigned int getTextureIndex() const;
		UVIndexType getUVIndexType() const;
		unsigned int getUVIndex() const;
		float getUVChangeInterval() const;
		Vector2 getColorScroll() const;
		Vector2 getColorScrollRandom() const;
		float getColorScrollSpeed() const;
		Vector2 getAlphaScroll() const;
		Vector2 getAlphaScrollRandom() const;
		float getAlphaScrollSpeed() const;
		Vector2 getSecondaryColorScroll() const;
		Vector2 getSecondaryColorScrollRandom() const;
		float getSecondaryColorScrollSpeed() const;
		Vector2 getSecondaryAlphaScroll() const;
		Vector2 getSecondaryAlphaScrollRandom() const;
		float getSecondaryAlphaScrollSpeed() const;
		std::string getMaterial() const;
		BlendMode getBlendMode() const;
		BlendMode getSecondaryBlendMode() const;
		float getSecondaryBlend() const;
		AddressMode getAddressMode() const;
		unsigned int getFlags() const;
		std::vector<std::weak_ptr<Emitter>> getChildEmitters() const;
		std::vector<unsigned int> getPendingChildEmitters() const;
		std::vector<GlitterAnimation>& getAnimations();

		void setID(unsigned int id);
		void setName(std::string n);
		void setType(ParticleType type);
		void setLifeTime(float time);
		void setPivotPosition(PivotPosition position);
		void setDirectionType(ParticleDirectionType type);
		void setZOffset(float offset);
		void setSize(Vector3 s);
		void setSizeRandom(Vector3 s);
		void setRotation(Vector3 r);
		void setRotationRandom(Vector3 r);
		void setRotationAdd(Vector3 r);
		void setRotationAddRandom(Vector3 r);
		void setDirection(Vector3 d);
		void setDirectionRandom(Vector3 d);
		void setEmitterTranslationEffectRatio(float ratio);
		void setFollowEmitterTranslationRatio(float ratio);
		void setFollowEmitterTranslationYRatio(float ratio);
		void setSpeed(float s);
		void setSpeedRandom(float s);
		void setGravitationalAccel(Vector3 g);
		void setExternalAccel(Vector3 a);
		void setExternalAccelRandom(Vector3 a);
		void setDeceleration(float d);
		void setDecelerationRandom(float d);
		void setReflectionCoeff(float coeff);
		void setReflectionCoeffRandom(float coeff);
		void setReboundPlaneY(float rebound);
		void setChildEmitterTime(float time);
		void setMaxCount(int max);
		void setMeshName(std::string name);
		void setLocusHistorySize(unsigned int size);
		void setLocusHistorySizeRandom(unsigned int size);
		void setColor(Color c);
		void setTextureIndex(unsigned int index);
		void setUVIndexType(UVIndexType type);
		void setUVIndex(unsigned int index);
		void setUVChangeInterval(float interval);
		void setColorScroll(Vector2 scroll);
		void setColorScrollRandom(Vector2 scroll);
		void setColorScrollSpeed(float speed);
		void setAlphaScroll(Vector2 v);
		void setAlphaScrollRandom(Vector2 v);
		void setAlphaScrollSpeed(float speed);
		void setSecondaryColorScroll(Vector2 v);
		void setSecondaryColorScrollRandom(Vector2 v);
		void setSecondaryColorScrollSpeed(float speed);
		void setSecondaryAlphaScroll(Vector2 v);
		void setSecondaryAlphaScrollRandom(Vector2 v);
		void setSecondaryAlphaScrollSpeed(float speed);
		void setMaterial(std::string mat);
		void setBlendMode(BlendMode mode);
		void setSecondaryBlendMode(BlendMode mode);
		void setSecondaryBlend(float blend);
		void setAddressMode(AddressMode mode);
		void setFlags(unsigned int f);

		void addChildEmitter(std::weak_ptr<Emitter> emitter);
		void removeChildEmitter(unsigned int index);
		void clearPendingChildEmitters();

		void read(tinyxml2::XMLElement* element);
		void write(tinyxml2::XMLElement* element);
	};
}
