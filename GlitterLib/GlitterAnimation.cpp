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

		float value = 0.0f;
		if ((size_t)type >= 6 && (size_t)type < 10)
		{
			// scale
			value = 1;
		}
		else if ((size_t)type >= 10 && (size_t)type < 14)
		{
			// color
			value = 255;
		}

		keys.push_back(GlitterKey{ start, value, InterpolationType::Linear, 0.0f, 0.0f, 0.0f });
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
		{
			keys.erase(keys.begin() + index);
		}
	}

	size_t GlitterAnimation::insertKey(GlitterKey key)
	{
		for (size_t index = 0; index < keys.size(); ++index)
		{
			if (keys[index].time > key.time)
			{
				keys.insert(keys.begin() + index, key);
				return index;
			}
		}

		keys.emplace_back(key);
		return keys.size() - 1;
	}

	void GlitterAnimation::verifyKeyOrder(unsigned int keyIndex)
	{
		if (keys.size() < 2)
			return;

		Glitter::GlitterKey& key = keys[keyIndex];
		if (keyIndex > 0)
		{
			if (key.time <= keys[keyIndex - 1].time)
				key.time = keys[keyIndex - 1].time + 1;

			if (keyIndex < keys.size() - 1)
			{
				if (key.time >= keys[keyIndex + 1].time)
					key.time = keys[keyIndex + 1].time - 1;
			}
		}
		else
		{
			if (keys.size() > 1)
			{
				if (key.time >= keys[keyIndex + 1].time)
					key.time = keys[keyIndex + 1].time - 1;
			}
		}
	}

	float GlitterAnimation::interpolate(float time, const Glitter::GlitterKey &k1, const Glitter::GlitterKey &k2)
	{
		float bias = (time - k1.time) / (k2.time - k1.time);

		if (k1.interpolationType == Glitter::InterpolationType::Hermite)
		{
			float factor = 1 - bias;
			return ((factor - 1.0f) * 2.0f - 1.0f) * (factor * factor) * (k2.value - k1.value) +
				((factor - 1.0f) * k2.inParam + factor * k1.outParam) *
				(factor - 1.0f) * (time - k2.time) + k2.value;
		}
		
		return k1.value + bias * (k2.value - k1.value);
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
			key.outParam	= BIXF::toFloat(keyElement->FirstChildElement("OutParam"));
			
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
