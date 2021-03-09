#pragma once
#include <string>
#include <tinyxml2.h>
#include "MathGens.h"

namespace Glitter
{
	constexpr uint8_t		BIXF_GOTO_PARENT			= 0;
	constexpr uint8_t		BIXF_NEW_NODE				= 0x21;
	constexpr uint8_t		BIXF_NEW_NODE_TABLE			= 0x29;
	constexpr uint8_t		BIXF_NEW_PARAMETER			= 0x41; // new parameter
	constexpr uint8_t		BIXF_NEW_PARAMETER_TABLE	= 0x49; // get parameter (next byte) from value table
	constexpr uint8_t		BIXF_NEW_VALUE				= 0x61; // gets value (next byte) from string table
	constexpr uint8_t		BIXF_NEW_VALUE_TABLE		= 0x69; // get value (next byte) from value table
	constexpr uint8_t		BIXF_NEW_VALUE_BOOL			= 0x70;
	constexpr uint8_t		BIXF_NEW_VALUE_INT			= 0x74;
	constexpr uint8_t		BIXF_NEW_VALUE_UINT			= 0x75;
	constexpr uint8_t		BIXF_NEW_VALUE_FLOAT		= 0x76;

	class BIXF
	{
	public:
		static const size_t IDTableSize;
		static const size_t valueTableSize;
		static const std::string IDTable[];
		static const std::string valueTable[];
		static std::string nodeIDToString(unsigned int id);
		static std::string valueIDToString(unsigned int id);
		static bool isInNodeTable(const std::string& str, unsigned char& id);
		static bool isInValueTable(const std::string& str, unsigned char& id);
		static tinyxml2::XMLDocument* parseBIXF(const std::string& filepath);
		static void convertToXML(std::string inputFilename, std::string outputFilename = "");
		static void convertToBIXF(std::string inputFilename, std::string outputFilename = "");
		static void convertToBIXF(tinyxml2::XMLDocument* xml, std::string outputFilename);
		static void convertToBIXF(tinyxml2::XMLElement* element, std::vector<std::string>& strTable, std::vector<unsigned char>& data);
		static size_t createBIXFString(const std::string& str, std::vector<std::string>& strTable);

		static std::string toString(tinyxml2::XMLElement* element);
		static float toFloat(tinyxml2::XMLElement* element);
		static unsigned int toUInt(tinyxml2::XMLElement* element);
		static int toInt(tinyxml2::XMLElement* element);
		static Color toColor(tinyxml2::XMLElement* element);
		static Vector2 toVector2(tinyxml2::XMLElement* element);
		static Vector2 toUV(tinyxml2::XMLElement* element);
		static Vector3 toVector3(tinyxml2::XMLElement* element);

		static void createChildValue(tinyxml2::XMLElement* parent, const char* name, const std::string& val);
		static void createChildValue(tinyxml2::XMLElement* parent, const char* name, const float& val);
		static void createChildValue(tinyxml2::XMLElement* parent, const char* name, const int& val);
		static void createChildValue(tinyxml2::XMLElement* parent, const char* name, const unsigned int& val);
		static void createChildColor(tinyxml2::XMLElement* parent, const char* name, const Color& val);
		static void createChildVector2(tinyxml2::XMLElement* parent, const char* name, const Vector2& val);
		static void createChildUV(tinyxml2::XMLElement* parent, const const char* name, const Vector2& val);
		static void createChildVector3(tinyxml2::XMLElement* parent, const char* name, const Vector3& val);
	};
}
