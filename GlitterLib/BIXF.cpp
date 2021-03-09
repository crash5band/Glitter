#include "BIXF.h"
#include "File.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"
#include <sstream>

namespace Glitter
{
	const size_t BIXF::IDTableSize = 104;
	const size_t BIXF::valueTableSize = 81;

	const std::string BIXF::IDTable[] = 
	{
		"X",
		"Y",
		"Z",
		"R",
		"G",
		"B",
		"A",
		"U",
		"V",
		"Id",
		"AddressMode",
		"AlphaScroll",
		"AlphaScrollRandom",
		"AlphaScrollSpeed",
		"Animation",
		"BlendMode",
		"ChildEmitter",
		"ChildEmitterTime",
		"Color",
		"ColorScroll",
		"ColorScrollRandom",
		"ColorScrollSpeed",
		"Deceleration",
		"DecelerationRandom",
		"Direction",
		"DirectionRandom",
		"DirectionType",
		"Effect",
		"EmissionDirectionType",
		"EmissionInterval",
		"EmitCondition",
		"Emitter",
		"EndAngle",
		"EndTime",
		"ExternalAccel",
		"ExternalAccelRandom",
		"Flags",
		"GravitationalAccel",
		"Height",
		"InParam",
		"InterpolationType",
		"Key",
		"Latitude",
		"LifeTime",
		"LocusHistorySize",
		"LocusHistorySizeRandom",
		"Longitude",
		"LoopStartTime",
		"LoopEndTime",
		"Material",
		"MaxCount",
		"Mesh",
		"MeshName",
		"Name",
		"OutParam",
		"Parameter",
		"Particle",
		"ParticlePerEmission",
		"PivotPosition",
		"PointCount",
		"Radius",
		"RandomFlags",
		"RandomRange",
		"ReboundPlaneY",
		"ReflectionCoeff",
		"ReflectionCoeffRandom",
		"RepeatType",
		"Rotation",
		"RotationAdd",
		"RotationAddRandom",
		"RotationRandom",
		"Scaling",
		"SecondaryAlphaScroll",
		"SecondaryAlphaScrollRandom",
		"SecondaryAlphaScrollSpeed",
		"SecondaryBlend",
		"SecondaryBlendMode",
		"SecondaryColorScroll",
		"SecondaryColorScrollRandom",
		"SecondaryColorScrollSpeed",
		"SecondaryTexture",
		"Shader",
		"Size",
		"SizeRandom",
		"Speed",
		"SpeedRandom",
		"Split",
		"StartAngle",
		"StartTime",
		"Texture",
		"TextureIndex",
		"Time",
		"Translation",
		"Type",
		"UvChangeInterval",
		"UvIndex",
		"UvIndexType",
		"Value",
		"ZOffset",
		"EmitterTranslationEffectRatio",
		"FollowEmitterTranslationRatio",
		"FollowEmitterTranslationYRatio",
		"UvIndexStart",
		"UvIndexEnd"
	};

	const std::string BIXF::valueTable[] =
	{
		"Box",
		"Cylinder",
		"Polygon",
		"Sphere",
		"Time",
		"MovingDistance",
		"ParentAxis",
		"Billboard",
		"XAxis",
		"YAxis",
		"ZAxis",
		"YRotationOnly",
		"Inward",
		"Outward",
		"Particle",
		"ParticleVelocity",
		"Deflection",
		"Layered",
		"Quad",
		"Mesh",
		"Locus",
		"Line",
		"TopLeft",
		"TopCenter",
		"TopRight",
		"MiddleLeft",
		"MiddleCenter",
		"MiddleRight",
		"BottomLeft",
		"BottomCenter",
		"BottomRight",
		"DirectionalAngle",
		"DirectionalAngleBillboard",
		"EmittedEmitterAxis",
		"EmitterAxis",
		"EmitterDirection",
		"Fixed",
		"InitialRandom",
		"InitialRandomReverseOrder",
		"InitialRandomSequentialOrder",
		"RandomOrder",
		"ReverseOrder",
		"SequentialOrder",
		"User",
		"Tx",
		"Ty",
		"Tz",
		"Rx",
		"Ry",
		"Rz",
		"Sx",
		"Sy",
		"Sz",
		"SAll",
		"ColorR",
		"ColorG",
		"ColorB",
		"ColorA",
		"UScroll",
		"VScroll",
		"UScrollAlpha",
		"VScrollAlpha",
		"SecondaryUScroll",
		"SecondaryVScroll",
		"SecondaryUScrollAlpha",
		"SecondaryVScrollAlpha",
		"EmissionInterval",
		"ParticlePerEmission",
		"Constant",
		"Hermite",
		"Linear",
		"Add",
		"Multiply",
		"Opaque",
		"PunchThrough",
		"Subtract",
		"Typical",
		"UseMaterial",
		"Zero",
		"Clamp",
		"Wrap"
	};

	std::string BIXF::nodeIDToString(unsigned int id)
	{
		if (id >= IDTableSize)
		{
			return "Node #" + std::to_string(id);
		}
		else
		{
			return IDTable[id];
		}
	}

	std::string BIXF::valueIDToString(unsigned int id)
	{
		if (id >= valueTableSize)
		{
			return "Value #" + std::to_string(id);
		}
		else
		{
			return valueTable[id];
		}
	}

	bool BIXF::isInNodeTable(const std::string& str, unsigned char& id)
	{
		for (size_t i = 0; i < IDTableSize; ++i)
		{
			if (str == IDTable[i])
			{
				id = i;
				return true;
			}
		}

		return false;
	}

	bool BIXF::isInValueTable(const std::string& str, unsigned char& id)
	{
		for (size_t i = 0; i < valueTableSize; ++i)
		{
			if (str == valueTable[i])
			{
				id = i;
				return true;
			}
		}

		return false;
	}

	tinyxml2::XMLDocument* BIXF::parseBIXF(const std::string& filepath)
	{
		tinyxml2::XMLDocument* xml = new tinyxml2::XMLDocument();
		if (!File::exists(filepath))
			return xml;

		BinaryReader reader(filepath, Endianness::LITTLE);
		
		reader.gotoAddress(8);
		unsigned int firstSectionSize = reader.readInt32();
		reader.gotoAddress(16);
		unsigned int stringCount = reader.readInt32();

		reader.gotoAddress(23 + firstSectionSize);
		std::vector<std::string> strTable;
		strTable.reserve(stringCount);
		for (unsigned int i = 0; i < stringCount; ++i)
		{
			strTable.emplace_back(reader.readString());
		}

		reader.gotoAddress(20);

		tinyxml2::XMLDeclaration *decleration = xml->NewDeclaration();
		xml->LinkEndChild(decleration);
		tinyxml2::XMLElement* currentElement = NULL;
		std::string currentParam;

		for (size_t i = 0; i < firstSectionSize; ++i)
		{
			unsigned char byte = reader.readChar();
			switch (byte)
			{
			case BIXF_GOTO_PARENT:
			{
				currentElement = currentElement->Parent()->ToElement();
				break;
			}
			case BIXF_NEW_NODE:
			{
				++i;
				byte = reader.readChar();
				if (!currentElement)
				{
					currentElement = xml->NewElement(strTable[byte].c_str());
					xml->InsertEndChild(currentElement);
				}
				else
				{
					currentElement = currentElement->InsertNewChildElement(strTable[byte].c_str());
				}
				break;
			}
			case BIXF_NEW_NODE_TABLE:
			{
				++i;
				byte = reader.readChar();
				if (!currentElement)
				{
					currentElement = xml->NewElement(nodeIDToString(byte).c_str());
					xml->InsertEndChild(currentElement);
				}
				else
				{
					currentElement = currentElement->InsertNewChildElement(nodeIDToString(byte).c_str());
				}
				break;
			}
			case BIXF_NEW_PARAMETER:
			{
				++i;
				byte = reader.readChar();
				currentParam = strTable[byte];
				break;
			}
			case BIXF_NEW_PARAMETER_TABLE:
			{
				++i;
				byte = reader.readChar();
				currentParam = nodeIDToString(byte);
				break;
			}
			case BIXF_NEW_VALUE:
			{
				++i;
				byte = reader.readChar();
				currentElement->SetAttribute(currentParam.c_str(), strTable[byte].c_str());
				break;
			}
			case BIXF_NEW_VALUE_TABLE:
			{
				++i;
				byte = reader.readChar();
				currentElement->SetAttribute(currentParam.c_str(), valueIDToString(byte).c_str());
				break;
			}
			case BIXF_NEW_VALUE_BOOL:
			{
				++i;
				byte = reader.readChar();
				currentElement->SetAttribute(currentParam.c_str(), byte ? "true" : "false");
				break;
			}
			case BIXF_NEW_VALUE_INT:
			{
				i += 4;
				int value = (int)reader.readInt32();
				currentElement->SetAttribute(currentParam.c_str(), value);
				break;
			}
			case BIXF_NEW_VALUE_UINT:
			{
				i += 4;
				unsigned int value = reader.readInt32();
				currentElement->SetAttribute(currentParam.c_str(), value);
				break;
			}
			case BIXF_NEW_VALUE_FLOAT:
			{
				i += 4;
				float value = reader.readSingle();
				std::stringstream ss;
				ss << value;
				currentElement->SetAttribute(currentParam.c_str(), ss.str().c_str());
				break;
			}
			default:
				break;
			}
		}

		reader.close();
		return xml;
	}

	void BIXF::convertToXML(std::string inputFilename, std::string outputFilename)
	{
		if (!outputFilename.size())
			outputFilename = inputFilename + ".xml";

		tinyxml2::XMLDocument* xml = parseBIXF(inputFilename);
		xml->SaveFile(outputFilename.c_str());

		delete xml;
	}

	void BIXF::convertToBIXF(std::string inputFilename, std::string outputFilename)
	{
		if (!outputFilename.size())
			outputFilename = inputFilename + ".xml";

		if (!File::exists(inputFilename))
		{
			printf("BIXF::ERROR: File %s not found\n", inputFilename.c_str());
			return;
		}

		tinyxml2::XMLDocument* xml = new tinyxml2::XMLDocument();
		xml->LoadFile(inputFilename.c_str());
		convertToBIXF(xml, outputFilename);

		delete xml;
	}

	void BIXF::convertToBIXF(tinyxml2::XMLDocument* xml, std::string outputFilename)
	{
		std::vector<unsigned char> data;
		std::vector<std::string> strTable;

		tinyxml2::XMLElement* element = xml->FirstChildElement();
		for (element; element; element = element->NextSiblingElement())
			convertToBIXF(element, strTable, data);

		BinaryWriter writer(outputFilename, Endianness::LITTLE);
		if (!writer.valid())
		{
			printf("BIXF::ERROR: Failed to write to file %s\n", outputFilename.c_str());
			return;
		}

		unsigned char header = 0x01;
		writer.writeString("BIXF", false);
		writer.writeChar(header);
		writer.fixPadding(20);

		unsigned int totalDataSize = data.size();
		unsigned int totalStringCount = strTable.size();
		unsigned int totalStringSize = 0;

		for (size_t i = 0; i < totalDataSize; ++i)
			writer.writeChar(data[i]);

		writer.writeNull(3);
		totalStringSize += 3;

		for (size_t i = 0; i < totalStringCount; ++i)
		{
			writer.writeString(strTable[i].c_str());
			totalStringSize += strTable[i].size() + 1;
		}

		writer.gotoAddress(8);
		writer.writeInt32(totalDataSize);
		writer.writeInt32(totalStringSize);
		writer.writeInt32(totalStringCount);

		writer.close();
	}

	void BIXF::convertToBIXF(tinyxml2::XMLElement* element, std::vector<std::string>& strTable, std::vector<unsigned char>& data)
	{
		unsigned char id = 0;

		// Node declaration
		if (isInNodeTable(element->Value(), id))
		{
			data.push_back(BIXF_NEW_NODE_TABLE);
			data.push_back(id);
		}
		else
		{
			data.push_back(BIXF_NEW_NODE);
			data.push_back(createBIXFString(element->Value(), strTable));
		}

		// Attributes
		for (const tinyxml2::XMLAttribute* attrib = element->FirstAttribute(); attrib; attrib = attrib->Next())
		{
			if (isInNodeTable(attrib->Name(), id))
			{
				data.push_back(BIXF_NEW_PARAMETER_TABLE);
				data.push_back(id);
			}
			else
			{
				data.push_back(BIXF_NEW_PARAMETER);
				data.push_back(createBIXFString(attrib->Name(), strTable));
			}

			if (isInValueTable(attrib->Value(), id))
			{
				data.push_back(BIXF_NEW_VALUE_TABLE);
				data.push_back(id);
			}
			else
			{
				if (attrib->Value() == "true")
				{
					data.push_back(BIXF_NEW_VALUE_BOOL);
					data.push_back(1);
				}
				else if (attrib->Value() == "false")
				{
					data.push_back(BIXF_NEW_VALUE_BOOL);
					data.push_back(0);
				}
				else
				{
					data.push_back(BIXF_NEW_VALUE);
					data.push_back(createBIXFString(attrib->Value(), strTable));
				}
			}
		}

		// Traverse children nodes and call the method recursively
		tinyxml2::XMLElement* siblingElement = element->FirstChildElement();
		while (siblingElement)
		{
			convertToBIXF(siblingElement, strTable, data);
			siblingElement = siblingElement->NextSiblingElement();
		}

		// Close node and go to parent
		data.push_back(BIXF_GOTO_PARENT);
	}

	size_t BIXF::createBIXFString(const std::string& str, std::vector<std::string>& strTable)
	{
		for (size_t index = 0; index < strTable.size(); ++index)
		{
			if (strTable[index] == str)
				return index;
		}

		strTable.push_back(str);
		return strTable.size() - 1;
	}

	std::string BIXF::toString(tinyxml2::XMLElement* element)
	{
		std::string str;
		if (element)
			str = element->FindAttribute("Value")->Value();

		return str;
	}

	float BIXF::toFloat(tinyxml2::XMLElement* element)
	{
		float f = 0;
		if (element)
			f = element->FindAttribute("Value")->FloatValue();

		return f;
	}

	unsigned int BIXF::toUInt(tinyxml2::XMLElement* element)
	{
		unsigned int i = 0;
		if (element)
			i = element->FindAttribute("Value")->UnsignedValue();

		return i;
	}

	int BIXF::toInt(tinyxml2::XMLElement* element)
	{
		int i = 0;
		if (element)
			i = element->FindAttribute("Value")->IntValue();

		return i;
	}

	Color BIXF::toColor(tinyxml2::XMLElement* element)
	{
		float r = 0; 
		float g = 0; 
		float b = 0; 
		float a = 0;

		if (element)
		{
			r = element->FindAttribute("R")->FloatValue();
			g = element->FindAttribute("G")->FloatValue();
			b = element->FindAttribute("B")->FloatValue();
			a = element->FindAttribute("A")->FloatValue();
		}

		return Color(r, g, b, a);
	}

	Vector2 BIXF::toVector2(tinyxml2::XMLElement* element)
	{
		float x = 0; 
		float y = 0;
		
		if (element)
		{
			x = element->FindAttribute("X")->FloatValue();
			y = element->FindAttribute("Y")->FloatValue();
		}

		return Vector2(x, y);
	}

	Vector2 BIXF::toUV(tinyxml2::XMLElement* element)
	{
		float u = 0;
		float v = 0;

		if (element)
		{
			u = element->FindAttribute("U")->FloatValue();
			v = element->FindAttribute("V")->FloatValue();
		}

		return Vector2(u, v);
	}

	Vector3 BIXF::toVector3(tinyxml2::XMLElement* element)
	{
		float x = 0; 
		float y = 0; 
		float z = 0;

		if (element)
		{
			x = element->FindAttribute("X")->FloatValue();
			y = element->FindAttribute("Y")->FloatValue();
			z = element->FindAttribute("Z")->FloatValue();
		}

		return Vector3(x, y, z);
	}

	void BIXF::createChildValue(tinyxml2::XMLElement* parent, const char* name, const std::string& val)
	{
		if (!parent)
			return;

		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("Value", val.c_str());
	}

	void BIXF::createChildValue(tinyxml2::XMLElement* parent, const char* name, const float& val)
	{
		if (!parent)
			return;

		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("Value", val);
	}

	void BIXF::createChildValue(tinyxml2::XMLElement* parent, const char* name, const int& val)
	{
		if (!parent)
			return;

		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("Value", val);
	}

	void BIXF::createChildValue(tinyxml2::XMLElement* parent, const char* name, const unsigned int& val)
	{
		if (!parent)
			return;

		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("Value", val);
	}

	void BIXF::createChildColor(tinyxml2::XMLElement* parent, const char* name, const Color& val)
	{
		if (!parent)
			return;

		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("R", val.r);
		element->SetAttribute("G", val.g);
		element->SetAttribute("B", val.b);
		element->SetAttribute("A", val.a);
	}

	void BIXF::createChildVector2(tinyxml2::XMLElement* parent, const char* name, const Vector2& val)
	{
		if (!parent)
			return;

		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("X", val.x);
		element->SetAttribute("Y", val.y);
	}

	void BIXF::createChildUV(tinyxml2::XMLElement* parent, const char* name, const Vector2& val)
	{
		if (!parent)
			return;

		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("U", val.x);
		element->SetAttribute("V", val.y);
	}

	void BIXF::createChildVector3(tinyxml2::XMLElement* parent, const char* name, const Vector3& val)
	{
		if (!parent)
			return;

		tinyxml2::XMLElement* element = parent->InsertNewChildElement(name);
		element->SetAttribute("X", val.x);
		element->SetAttribute("Y", val.y);
		element->SetAttribute("Z", val.z);
	}
}