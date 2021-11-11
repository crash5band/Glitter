#include "GlitterAnimation.h"
#include "BIXF.h"

namespace Glitter
{
	GlitterAnimation::GlitterAnimation()
	{
		type = AnimationType::ColorA;
		startTime = endTime = 0.0f;
		repeatType = RepeatType::Constant;
		randomFlags = 0;
	}

	GlitterAnimation::GlitterAnimation(AnimationType type, float start) :
		type{ type }, startTime{ start }
	{
		endTime = startTime + 10;
		repeatType = RepeatType::Constant;
		randomFlags = 0;
		keys.push_back(GlitterKey{ start, 0, InterpolationType::Linear, 0.0f, 0.0f, 0.0f });
	}

	AnimationType GlitterAnimation::getType() const
	{
		return type;
	}

	float GlitterAnimation::getStartTime() const
	{
		return startTime;
	}

	float GlitterAnimation::getEndTime() const
	{
		return endTime;
	}

	RepeatType GlitterAnimation::getRepeatType() const
	{
		return repeatType;
	}

	unsigned int GlitterAnimation::getRandomFlags() const
	{
		return randomFlags;
	}

	std::vector<GlitterKey> &GlitterAnimation::getKeys()
	{
		return keys;
	}

	void GlitterAnimation::setAnimationType(AnimationType type)
	{
		this->type = type;
	}

	void GlitterAnimation::setStartTime(float time)
	{
		if (time <= endTime && time >= 0.0f)
		{
			startTime = (int)time;
			if (keys.size() && time > keys[0].time)
				startTime = keys[0].time;
		}
	}

	void GlitterAnimation::setEndTime(float time)
	{
		if (time >= startTime && time >= 0.0f)
		{
			endTime = (int)time;
			if (keys.size() && time < keys[keys.size() - 1].time)
				endTime = keys[keys.size() - 1].time;
		}
	}

	void GlitterAnimation::setRepeatType(RepeatType type)
	{
		repeatType = type;
	}

	void GlitterAnimation::setRandomFlags(unsigned int flags)
	{
		randomFlags = flags;
	}

	void GlitterAnimation::addKey(GlitterKey key)
	{
		if (key.time >= startTime && key.time <= endTime)
			keys.push_back(key);
	}

	void GlitterAnimation::removeKey(unsigned int index)
	{
		if (index < keys.size())
			keys.erase(keys.begin() + index);
	}

	void GlitterAnimation::read(tinyxml2::XMLElement* element)
	{
		std::string typeStr = element->FirstAttribute()->Value();
		type = (AnimationType)glitterStringToEnum(animationTypeTable, animationTypeTableSize, typeStr);

		startTime	= BIXF::toFloat(element->FirstChildElement("StartTime"));
		endTime		= BIXF::toFloat(element->FirstChildElement("EndTime"));

		std::string repeatStr = BIXF::toString(element->FirstChildElement("RepeatType"));
		repeatType	= (RepeatType)glitterStringToEnum(repeatTypeTable, repeatTypeTableSize, repeatStr);

		randomFlags = BIXF::toUInt(element->FirstChildElement("RandomFlags"));

		tinyxml2::XMLElement* keyElement = element->FirstChildElement("Key");
		while (keyElement)
		{
			GlitterKey key;
			key.time = keyElement->FindAttribute("Time")->FloatValue();
			key.value = keyElement->FindAttribute("Value")->FloatValue();
			
			std::string interpolationStr = BIXF::toString(keyElement->FirstChildElement("InterpolationType"));
			key.interpolationType = (InterpolationType)glitterStringToEnum(interpolationTypeTable, interpolationTypeTableSize, interpolationStr);
			key.inParam	= BIXF::toFloat(keyElement->FirstChildElement("InParam"));
			key.outParam = BIXF::toFloat(keyElement->FirstChildElement("OutParam"));
			
			tinyxml2::XMLElement* randomRangeElement = keyElement->FirstChildElement("RandomRange");
			key.randomRange = randomRangeElement ? randomRangeElement->FirstAttribute()->FloatValue() : 0;

			keys.emplace_back(key);
			keyElement = keyElement->NextSiblingElement("Key");
		}
	}

	void GlitterAnimation::write(tinyxml2::XMLElement* element)
	{
		std::string typeStr = glitterEnumToString(animationTypeTable, animationTypeTableSize, (size_t)type);
		element->SetAttribute("Type", typeStr.c_str());

		BIXF::createChildValue(element, "StartTime", startTime);
		BIXF::createChildValue(element, "EndTime", endTime);

		std::string repeatStr = glitterEnumToString(repeatTypeTable, repeatTypeTableSize, (size_t)repeatType);
		BIXF::createChildValue(element, "RepeatType", repeatStr);
		BIXF::createChildValue(element, "RandomFlags", randomFlags);

		for (const GlitterKey& key : keys)
		{
			tinyxml2::XMLElement* keyElement = element->InsertNewChildElement("Key");
			keyElement->SetAttribute("Time", key.time);
			keyElement->SetAttribute("Value", key.value);

			std::string interpolationStr = glitterEnumToString(interpolationTypeTable, interpolationTypeTableSize, (size_t)key.interpolationType);
			BIXF::createChildValue(keyElement, "InterpolationType", interpolationStr);
			BIXF::createChildValue(keyElement, "InParam", key.inParam);
			BIXF::createChildValue(keyElement, "OutParam", key.outParam);
			
			if (key.randomRange != 0)
				BIXF::createChildValue(keyElement, "RandomRange", key.randomRange);
		}
	}
}
