#include "Particle.h"
#include "BIXF.h"

namespace Glitter
{
	Particle::Particle() : ID{ 0 }, type{ ParticleType::Quad }, lifeTime{ 30.0f }, maxCount{ 5 }
	{
		pivotPosition = PivotPosition::MiddleCenter;
		directionType = ParticleDirectionType::Billboard;
		zOffset = 0.0f;
		size = Vector3(1.0f, 1.0f, 1.0f);
		speed = 0.0f;
		speedRandom = 0.0f;
		deceleration = 0.0f;
		decelrationRandom = 0.0f;
		emitterTranslationEffectRatio = 0.0f;
		followEmitterTranslationRatio = 0.0f;
		followEmitterTranslationYRatio = 0.0f;
		reflectionCoeff = 0.0f;
		reflectionCoeffRandom = 0.0f;
		reboundPlaneY = 0.0f;
		childEmitterTime = 0.0f;
		locusHistorySize = 4;
		locusHistorySizeRandom = 4;
		color = Color(1.0f, 1.0f, 1.0f, 1.0f);
		textureIndex = 0;
		uvIndexType = UVIndexType::Fixed;
		uvIndex = uvIndexStart = uvIndexEnd = 0;
		uvChangeInterval = 0.0f;
		colorScrollSpeed = secondaryColorScrollSpeed = 0.0f;
		alphaScrollSpeed = secondaryAlphaScrollSpeed = 0.0f;
		blendMode = secondaryBlendMode = BlendMode::Add;
		secondaryBlend = -1.0f;
		addressMode = AddressMode::Clamp;
		flags = 0;
	}

	Particle::Particle(const std::string& n) :
		ID{ 0 }, name{ n }, lifeTime{ 30 }, maxCount{ 5 }, type{ ParticleType::Quad }
	{
		pivotPosition = PivotPosition::MiddleCenter;
		directionType = ParticleDirectionType::Billboard;
		zOffset = 0.0f;
		size = Vector3(1.0f, 1.0f, 1.0f);
		speed = 0.0f;
		speedRandom = 0.0f;
		deceleration = 0.0f;
		decelrationRandom = 0.0f;
		emitterTranslationEffectRatio = 0.0f;
		followEmitterTranslationRatio = 0.0f;
		followEmitterTranslationYRatio = 0.0f;
		reflectionCoeff = 0.0f;
		reflectionCoeffRandom = 0.0f;
		reboundPlaneY = 0.0f;
		childEmitterTime = 0.0f;
		locusHistorySize = 4;
		locusHistorySizeRandom = 4;
		color = Color(1.0f, 1.0f, 1.0f, 1.0f);
		textureIndex = 0;
		uvIndexType = UVIndexType::Fixed;
		uvIndex = uvIndexStart = uvIndexEnd = 0;
		uvChangeInterval = 0.0f;
		colorScrollSpeed = secondaryColorScrollSpeed = 0.0f;
		alphaScrollSpeed = secondaryAlphaScrollSpeed = 0.0f;
		blendMode = secondaryBlendMode = BlendMode::Add;
		secondaryBlend = -1.0f;
		addressMode = AddressMode::Clamp;
		flags = 0;
	}

	unsigned int Particle::getID() const
	{
		return ID;
	}

	std::string Particle::getName() const
	{
		return name;
	}

	ParticleType Particle::getType() const
	{
		return type;
	}

	float Particle::getLifeTime() const
	{
		return lifeTime;
	}

	PivotPosition Particle::getPivotPosition() const
	{
		return pivotPosition;
	}

	ParticleDirectionType Particle::getDirectionType() const
	{
		return directionType;
	}

	float Particle::getZOffset() const
	{
		return zOffset;
	}

	Vector3 Particle::getSize() const
	{
		return size;
	}

	Vector3 Particle::getSizeRandom() const
	{
		return sizeRandom;
	}

	Vector3 Particle::getRotation() const
	{
		return rotation;
	}

	Vector3 Particle::getRotationAdd() const
	{
		return rotationAdd;
	}

	Vector3 Particle::getRotationRandom() const
	{
		return rotationRandom;
	}

	Vector3 Particle::getRotationAddRandom() const
	{
		return rotationAddRandom;
	}

	Vector3 Particle::getDirection() const
	{
		return direction;
	}

	Vector3 Particle::getDirectionRandom() const
	{
		return directionRandom;
	}

	float Particle::getSpeed() const
	{
		return speed;
	}

	float Particle::getSpeedRandom() const
	{
		return speedRandom;
	}

	Vector3 Particle::getGravitationalAccel() const
	{
		return gravitationalAccel;
	}

	Vector3 Particle::getExternalAccel() const
	{
		return externalAccel;
	}

	Vector3 Particle::getExternalAccelRandom() const
	{
		return externalAccelRandom;
	}

	float Particle::getDeceleration() const
	{
		return deceleration;
	}

	float Particle::getDecelerationRandom() const
	{
		return decelrationRandom;
	}

	float Particle::getEmitterTranslationEffectRatio() const
	{
		return emitterTranslationEffectRatio;
	}

	float Particle::getFollowEmitterTranslationRatio() const
	{
		return followEmitterTranslationRatio;
	}

	float Particle::getFollowEmitterTranslationYRatio() const
	{
		return followEmitterTranslationYRatio;
	}

	float Particle::getReflectionCoeff() const
	{
		return reflectionCoeff;
	}

	float Particle::getReflectionCoeffRandom() const
	{
		return reflectionCoeff;
	}

	float Particle::getReboundPlaneY() const
	{
		return reboundPlaneY;
	}

	float Particle::getChildEmitterTime() const
	{
		return childEmitterTime;
	}

	std::string Particle::getMeshName() const
	{
		return type == ParticleType::Mesh ? meshName : "";
	}

	int Particle::getLocusHistorySize() const
	{
		return locusHistorySize;
	}

	int Particle::getLocusHistorySizeRandom() const
	{
		return locusHistorySizeRandom;
	}

	int Particle::getMaxCount() const
	{
		return maxCount;
	}

	Color Particle::getColor() const
	{
		return color;
	}

	unsigned int Particle::getTextureIndex() const
	{
		return textureIndex;
	}

	UVIndexType Particle::getUVIndexType() const
	{
		return uvIndexType;
	}

	unsigned int Particle::getUVIndex() const
	{
		return uvIndex;
	}

	float Particle::getUVChangeInterval() const
	{
		return uvChangeInterval;
	}

	Vector2 Particle::getColorScroll() const
	{
		return colorScroll;
	}

	Vector2 Particle::getColorScrollRandom() const
	{
		return colorScrollRandom;
	}

	float Particle::getColorScrollSpeed() const
	{
		return colorScrollSpeed;
	}

	Vector2 Particle::getAlphaScroll() const
	{
		return alphaScroll;
	}

	Vector2 Particle::getAlphaScrollRandom() const
	{
		return alphaScrollRandom;
	}

	float Particle::getAlphaScrollSpeed() const
	{
		return alphaScrollSpeed;
	}

	Vector2 Particle::getSecondaryColorScroll() const
	{
		return secondaryColorScroll;
	}

	Vector2 Particle::getSecondaryColorScrollRandom() const
	{
		return secondaryColorScrollRandom;
	}

	float Particle::getSecondaryColorScrollSpeed() const
	{
		return secondaryColorScrollSpeed;
	}

	Vector2 Particle::getSecondaryAlphaScroll() const
	{
		return secondaryAlphaScroll;
	}

	Vector2 Particle::getSecondaryAlphaScrollRandom() const
	{
		return secondaryAlphaScrollRandom;
	}

	float Particle::getSecondaryAlphaScrollSpeed() const
	{
		return alphaScrollSpeed;
	}

	std::string Particle::getMaterial() const
	{
		return material;
	}

	BlendMode Particle::getBlendMode() const
	{
		return blendMode;
	}

	BlendMode Particle::getSecondaryBlendMode() const
	{
		return secondaryBlendMode;
	}

	float Particle::getSecondaryBlend() const
	{
		return secondaryBlend;
	}

	AddressMode Particle::getAddressMode() const
	{
		return addressMode;
	}

	unsigned int Particle::getFlags() const
	{
		return flags;
	}

	std::vector<Animation>& Particle::getAnimations()
	{
		return animations;
	}

	std::vector<std::weak_ptr<Emitter>> Particle::getChildEmitters() const
	{
		return childEmitters;
	}

	std::vector<unsigned int> Particle::getPendingChildEmitters() const
	{
		return pendingChildEmitters;
	}

	void Particle::setID(unsigned int id)
	{
		ID = id;
	}

	void Particle::setName(std::string n)
	{
		if (n.size())
			name = n;
	}

	void Particle::setType(ParticleType t)
	{
		type = t;
	}

	void Particle::setLifeTime(float time)
	{
		if (time >= 0)
			lifeTime = time;
	}

	void Particle::setPivotPosition(PivotPosition position)
	{
		pivotPosition = position;
	}

	void Particle::setDirectionType(ParticleDirectionType type)
	{
		directionType = type;
	}

	void Particle::setZOffset(float offset)
	{
		zOffset = offset;
	}

	void Particle::setSize(Vector3 v)
	{
		size = v;
	}

	void Particle::setSizeRandom(Vector3 v)
	{
		sizeRandom = v;
	}

	void Particle::setRotation(Vector3 v)
	{
		rotation = v;
	}

	void Particle::setRotationRandom(Vector3 v)
	{
		rotationRandom = v;
	}

	void Particle::setRotationAdd(Vector3 v)
	{
		rotationAdd = v;
	}

	void Particle::setRotationAddRandom(Vector3 v)
	{
		rotationAddRandom = v;
	}

	void Particle::setDirection(Vector3 v)
	{
		direction = v;
	}

	void Particle::setDirectionRandom(Vector3 v)
	{
		directionRandom = v;
	}

	void Particle::setEmitterTranslationEffectRatio(float ratio)
	{
		emitterTranslationEffectRatio = ratio;
	}

	void Particle::setFollowEmitterTranslationRatio(float ratio)
	{
		followEmitterTranslationRatio = ratio;
	}

	void Particle::setFollowEmitterTranslationYRatio(float ratio)
	{
		followEmitterTranslationYRatio = ratio;
	}

	void Particle::setSpeed(float s)
	{
		speed = s;
	}

	void Particle::setSpeedRandom(float s)
	{
		speedRandom = s;
	}

	void Particle::setGravitationalAccel(Vector3 v)
	{
		gravitationalAccel = v;
	}

	void Particle::setExternalAccel(Vector3 v)
	{
		externalAccel = v;
	}

	void Particle::setExternalAccelRandom(Vector3 v)
	{
		externalAccelRandom = v;
	}

	void Particle::setDeceleration(float decel)
	{
		deceleration = decel;
	}

	void Particle::setDecelerationRandom(float decel)
	{
		decelrationRandom = decel;
	}

	void Particle::setReflectionCoeff(float coeff)
	{
		reflectionCoeff = coeff;
	}

	void Particle::setReflectionCoeffRandom(float coeff)
	{
		reflectionCoeffRandom = coeff;
	}

	void Particle::setReboundPlaneY(float rebound)
	{
		reboundPlaneY = rebound;
	}

	void Particle::setChildEmitterTime(float time)
	{
		childEmitterTime = time;
	}

	void Particle::setMaxCount(int count)
	{
		if (count >= 0)
			maxCount = count;
	}

	void Particle::setMeshName(std::string name)
	{
		if (type == ParticleType::Mesh)
			meshName = name;
	}

	void Particle::setLocusHistorySize(unsigned int size)
	{
		if (size >= locusHistorySizeRandom)
			locusHistorySize = size;
		else
			locusHistorySize = locusHistorySizeRandom;
	}

	void Particle::setLocusHistorySizeRandom(unsigned int size)
	{
		if (size <= locusHistorySize)
			locusHistorySizeRandom = size;
		else
			locusHistorySizeRandom = locusHistorySize;
	}

	void Particle::setColor(Color c)
	{
		color = c;
	}

	void Particle::setTextureIndex(unsigned int index)
	{
		textureIndex = index;
	}

	void Particle::setUVIndexType(UVIndexType type)
	{
		uvIndexType = type;
	}
	
	void Particle::setUVIndex(unsigned int index)
	{
		uvIndex = index;
	}

	void Particle::setUVChangeInterval(float interval)
	{
		uvChangeInterval = interval;
	}

	void Particle::setColorScroll(Vector2 scroll)
	{
		colorScroll = scroll;
	}

	void Particle::setColorScrollRandom(Vector2 scroll)
	{
		colorScrollRandom = scroll;
	}

	void Particle::setColorScrollSpeed(float speed)
	{
		colorScrollSpeed = speed;
	}

	void Particle::setAlphaScroll(Vector2 v)
	{
		alphaScroll = v;
	}

	void Particle::setAlphaScrollRandom(Vector2 v)
	{
		alphaScrollRandom = v;
	}

	void Particle::setAlphaScrollSpeed(float speed)
	{
		alphaScrollSpeed = speed;
	}

	void Particle::setSecondaryColorScroll(Vector2 v)
	{
		secondaryColorScroll = v;
	}

	void Particle::setSecondaryColorScrollRandom(Vector2 v)
	{
		secondaryColorScrollRandom = v;
	}

	void Particle::setSecondaryColorScrollSpeed(float speed)
	{
		secondaryColorScrollSpeed = speed;
	}

	void Particle::setSecondaryAlphaScroll(Vector2 v)
	{
		secondaryAlphaScroll = v;
	}

	void Particle::setSecondaryAlphaScrollRandom(Vector2 v)
	{
		secondaryAlphaScrollRandom = v;
	}

	void Particle::setSecondaryAlphaScrollSpeed(float speed)
	{
		secondaryAlphaScrollSpeed = speed;
	}

	void Particle::setMaterial(std::string mat)
	{
		material = mat;
	}

	void Particle::setBlendMode(BlendMode mode)
	{
		blendMode = mode;
	}

	void Particle::setSecondaryBlendMode(BlendMode mode)
	{
		secondaryBlendMode = mode;
	}

	void Particle::setSecondaryBlend(float blend)
	{
		secondaryBlend = blend;
	}

	void Particle::setAddressMode(AddressMode mode)
	{
		addressMode = mode;
	}

	void Particle::setFlags(unsigned int f)
	{
		flags = f;
	}

	void Particle::addChildEmitter(std::weak_ptr<Emitter> emitter)
	{
		childEmitters.push_back(emitter);
	}

	void Particle::removeChildEmitter(unsigned int index)
	{
		if (index >= childEmitters.size())
			return;

		childEmitters.erase(childEmitters.begin() + index);
	}

	void Particle::clearPendingChildEmitters()
	{
		pendingChildEmitters.clear();
	}

	void Particle::read(tinyxml2::XMLElement* element)
	{
		ID = element->FindAttribute("Id")->FloatValue();
		name = element->FindAttribute("Name")->Value();
		type = (ParticleType)glitterStringToEnum(particleTypeTable, particleTypeTableSize, element->FindAttribute("Type")->Value());

		lifeTime = BIXF::toFloat(element->FirstChildElement("LifeTime"));
		
		std::string pivotStr = BIXF::toString(element->FirstChildElement("PivotPosition"));
		pivotPosition = (PivotPosition)glitterStringToEnum(pivotPositionTable, pivotPositionTableSize, pivotStr);

		std::string dirStr = BIXF::toString(element->FirstChildElement("DirectionType"));
		directionType = (ParticleDirectionType)(glitterStringToEnum(PdirectionTypeTable, PdirectionTypeTableSize, dirStr));

		zOffset					= BIXF::toFloat(element->FirstChildElement("ZOffset"));
		size					= BIXF::toVector3(element->FirstChildElement("Size"));
		sizeRandom				= BIXF::toVector3(element->FirstChildElement("SizeRandom"));
		rotation				= BIXF::toVector3(element->FirstChildElement("Rotation"));
		rotationRandom			= BIXF::toVector3(element->FirstChildElement("RotationRandom"));
		rotationAdd				= BIXF::toVector3(element->FirstChildElement("RotationAdd"));
		rotationAddRandom		= BIXF::toVector3(element->FirstChildElement("RotationAddRandom"));
		direction				= BIXF::toVector3(element->FirstChildElement("Direction"));
		directionRandom			= BIXF::toVector3(element->FirstChildElement("DirectionRandom"));
		speed					= BIXF::toFloat(element->FirstChildElement("Speed"));
		speedRandom				= BIXF::toFloat(element->FirstChildElement("SpeedRandom"));
		gravitationalAccel		= BIXF::toVector3(element->FirstChildElement("GravitationalAccel"));
		externalAccel			= BIXF::toVector3(element->FirstChildElement("ExternalAccel"));
		externalAccelRandom		= BIXF::toVector3(element->FirstChildElement("ExternalAccelRandom"));
		deceleration			= BIXF::toFloat(element->FirstChildElement("Deceleration"));
		decelrationRandom		= BIXF::toFloat(element->FirstChildElement("DecelerationRandom"));
		reflectionCoeff			= BIXF::toFloat(element->FirstChildElement("ReflectionCoeff"));
		reflectionCoeffRandom	= BIXF::toFloat(element->FirstChildElement("ReflectionCoeffRandom"));
		reboundPlaneY			= BIXF::toFloat(element->FirstChildElement("ReboundPlaneY"));
		maxCount				= BIXF::toInt(element->FirstChildElement("MaxCount"));
		color					= BIXF::toColor(element->FirstChildElement("Color"));
		textureIndex			= BIXF::toUInt(element->FirstChildElement("TextureIndex"));

		std::string uvTypeStr	= BIXF::toString(element->FirstChildElement("UvIndexType"));
		uvIndexType = (UVIndexType)glitterStringToEnum(uvIndexTypeTable, uvIndexTypeTableSize, uvTypeStr);

		uvIndex							= BIXF::toUInt(element->FirstChildElement("UvIndex"));
		uvChangeInterval				= BIXF::toFloat(element->FirstChildElement("UvChangeInterval"));
		colorScroll						= BIXF::toUV(element->FirstChildElement("ColorScroll"));
		colorScrollRandom				= BIXF::toUV(element->FirstChildElement("ColorScrollRandom"));
		colorScrollSpeed				= BIXF::toFloat(element->FirstChildElement("ColorScrollSpeed"));
		alphaScroll						= BIXF::toUV(element->FirstChildElement("AlphaScroll"));
		alphaScrollRandom				= BIXF::toUV(element->FirstChildElement("AlphaScrollRandom"));
		alphaScrollSpeed				= BIXF::toFloat(element->FirstChildElement("AlphaScrollSpeed"));
		secondaryColorScroll			= BIXF::toUV(element->FirstChildElement("SecondaryColorScroll"));
		secondaryColorScrollRandom		= BIXF::toUV(element->FirstChildElement("SecondaryColorScrollRandom"));
		secondaryColorScrollSpeed		= BIXF::toFloat(element->FirstChildElement("SecondaryColorScrollSpeed"));
		secondaryAlphaScroll			= BIXF::toUV(element->FirstChildElement("SecondaryAlphaScroll"));
		secondaryAlphaScrollRandom		= BIXF::toUV(element->FirstChildElement("SecondaryAlphaScrollRandom"));
		secondaryAlphaScrollSpeed		= BIXF::toFloat(element->FirstChildElement("SecondaryAlphaScrollSpeed"));
		material						= BIXF::toString(element->FirstChildElement("Material"));
		emitterTranslationEffectRatio	= BIXF::toFloat(element->FirstChildElement("EmitterTranslationEffectRatio"));
		followEmitterTranslationRatio	= BIXF::toFloat(element->FirstChildElement("FollowEmitterTranslationRatio")); // flag 2048?
		followEmitterTranslationYRatio	= BIXF::toFloat(element->FirstChildElement("FollowEmitterTranslationYRatio"));
		
		std::string blendStr = BIXF::toString(element->FirstChildElement("BlendMode"));
		blendMode = (BlendMode)glitterStringToEnum(blendModeTable, blendModeTableSize, blendStr);
		std::string secondBlenStr = BIXF::toString(element->FirstChildElement("SecondaryBlendMode"));
		secondaryBlendMode = (BlendMode)glitterStringToEnum(blendModeTable, blendModeTableSize, secondBlenStr);

		std::string addressModeStr = BIXF::toString(element->FirstChildElement("AddressMode"));
		addressMode = (AddressMode)glitterStringToEnum(addressModeTable, addressModeTableSize, addressModeStr);

		secondaryBlend = BIXF::toFloat(element->FirstChildElement("SecondaryBlend"));
		flags = BIXF::toUInt(element->FirstChildElement("Flags"));

		switch (type)
		{
		case ParticleType::Mesh:
			meshName = BIXF::toString(element->FirstChildElement("MeshName"));
			break;

		case Glitter::ParticleType::Locus:
			locusHistorySize = BIXF::toUInt(element->FirstChildElement("LocusHistorySize"));
			locusHistorySizeRandom = BIXF::toUInt(element->FirstChildElement("LocusHistorySizeRandom"));
			break;

		default:
			break;
		}

		tinyxml2::XMLElement* animationElement = element->FirstChildElement("Animation");
		while (animationElement)
		{
			Animation animation;
			animation.read(animationElement);
			animations.emplace_back(animation);

			animationElement = animationElement->NextSiblingElement("Animation");
		}

		tinyxml2::XMLElement* childEmitterElement = element->FirstChildElement("ChildEmitter");
		while (childEmitterElement)
		{
			unsigned int id = childEmitterElement->FindAttribute("Id")->UnsignedValue();
			pendingChildEmitters.push_back(id);

			childEmitterElement = childEmitterElement->NextSiblingElement("ChildEmitter");
		}
	}

	void Particle::write(tinyxml2::XMLElement* element)
	{
		element->SetAttribute("Id", ID);
		element->SetAttribute("Name", name.c_str());
		element->SetAttribute("Type", glitterEnumToString(particleTypeTable, particleTypeTableSize, (size_t)type).c_str());

		BIXF::createChildValue(element, "LifeTime", lifeTime);
		BIXF::createChildValue(element, "PivotPosition", glitterEnumToString(pivotPositionTable, pivotPositionTableSize, (size_t)pivotPosition));
		BIXF::createChildValue(element, "DirectionType", glitterEnumToString(PdirectionTypeTable, PdirectionTypeTableSize, (size_t)directionType));
		BIXF::createChildValue(element, "ZOffset", zOffset);

		BIXF::createChildVector3(element, "Size", size);
		BIXF::createChildVector3(element, "SizeRandom", sizeRandom);
		BIXF::createChildVector3(element, "Rotation", rotation);
		BIXF::createChildVector3(element, "RotationRandom", rotationRandom);
		BIXF::createChildVector3(element, "RotationAdd", rotationAdd);
		BIXF::createChildVector3(element, "RotationAddRandom", rotationAddRandom);
		BIXF::createChildVector3(element, "Direction", direction);
		BIXF::createChildVector3(element, "DirectionRandom", directionRandom);
		BIXF::createChildValue(element, "Speed", speed);
		BIXF::createChildValue(element, "SpeedRandom", speedRandom);
		BIXF::createChildVector3(element, "GravitationalAccel", gravitationalAccel);
		BIXF::createChildVector3(element, "ExternalAccel", externalAccel);
		BIXF::createChildVector3(element, "ExternalAccelRandom", externalAccelRandom);
		BIXF::createChildValue(element, "Deceleration", deceleration);
		BIXF::createChildValue(element, "DecelerationRandom", decelrationRandom);
		BIXF::createChildValue(element, "ReflectionCoeff", reflectionCoeff);
		BIXF::createChildValue(element, "ReflectionCoeffRandom", reflectionCoeffRandom);
		BIXF::createChildValue(element, "ReboundPlaneY", reboundPlaneY);
		BIXF::createChildValue(element, "MaxCount", maxCount);

		if (type == ParticleType::Mesh)
			BIXF::createChildValue(element, "MeshName", meshName);

		if (type == ParticleType::Locus)
		{
			BIXF::createChildValue(element, "LocusHistorySize", locusHistorySize);
			BIXF::createChildValue(element, "LocusHistorySizeRandom", locusHistorySizeRandom);
		}

		BIXF::createChildColor(element, "Color", color);
		BIXF::createChildValue(element, "TextureIndex", textureIndex);
		BIXF::createChildValue(element, "UvIndexType", glitterEnumToString(uvIndexTypeTable, uvIndexTypeTableSize, (size_t)uvIndexType));

		BIXF::createChildValue(element, "UvIndex", uvIndex);
		BIXF::createChildValue(element, "UvChangeInterval", uvChangeInterval);
		BIXF::createChildUV(element, "ColorScroll", colorScroll);
		BIXF::createChildUV(element, "ColorScrollRandom", colorScrollRandom);
		BIXF::createChildValue(element, "ColorScrollSpeed", colorScrollSpeed);
		BIXF::createChildUV(element, "AlphaScroll", alphaScroll);
		BIXF::createChildUV(element, "AlphaScrollRandom", alphaScrollRandom);
		BIXF::createChildValue(element, "AlphaScrollSpeed", alphaScrollSpeed);
		BIXF::createChildUV(element, "SecondaryColorScroll", secondaryColorScroll);
		BIXF::createChildUV(element, "SecondaryColorScrollRandom", secondaryColorScrollRandom);
		BIXF::createChildValue(element, "SecondaryColorScrollSpeed", secondaryColorScrollSpeed);
		BIXF::createChildUV(element, "SecondaryAlphaScroll", secondaryAlphaScroll);
		BIXF::createChildUV(element, "SecondaryAlphaScrollRandom", secondaryAlphaScrollRandom);
		BIXF::createChildValue(element, "SecondaryAlphaScrollSpeed", secondaryAlphaScrollSpeed);
		BIXF::createChildValue(element, "Material", material);
		BIXF::createChildValue(element, "EmitterTranslationEffectRatio", emitterTranslationEffectRatio);
		BIXF::createChildValue(element, "FollowEmitterTranslationRatio", followEmitterTranslationRatio);
		BIXF::createChildValue(element, "FollowEmitterTranslationYRatio", followEmitterTranslationYRatio);

		BIXF::createChildValue(element, "SecondaryBlendMode", glitterEnumToString(blendModeTable, blendModeTableSize, (size_t)secondaryBlendMode));
		BIXF::createChildValue(element, "SecondaryBlend", secondaryBlend);
		BIXF::createChildValue(element, "Flags", flags);

		unsigned int count;
		for (count = 0; count < animations.size(); ++count)
		{
			tinyxml2::XMLElement* animationElement = element->InsertNewChildElement("Animation");
			animations[count].write(animationElement);
		}

		for (count = 0; count < childEmitters.size(); ++count)
		{
			// THIS IS PURE GUESS!!! 
			tinyxml2::XMLElement* childEmitterElement = element->InsertNewChildElement("ChildEmitter");
			childEmitterElement->SetAttribute("Id", childEmitters[count].lock()->getID());
			childEmitterElement->SetAttribute("Name", childEmitters[count].lock()->getName().c_str());
		}
	}
}
