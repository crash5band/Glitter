#include "GlitterMaterial.h"
#include "BIXF.h"

namespace Glitter
{
	GlitterMaterial::GlitterMaterial(const std::string& name, BlendMode blend) :
		name{ name }, blendMode{ blend }, addressMode{ AddressMode::Clamp }
	{
		split = Vector2(1.0f, 1.0f);
		shader.name = defaultShader;
	}

	GlitterMaterial::GlitterMaterial(const std::string& filepath)
	{
		read(filepath);
		filename = filepath;
	}

	std::string GlitterMaterial::getFilename() const
	{
		return filename;
	}

	std::string GlitterMaterial::getName() const
	{
		return name;
	}

	std::string GlitterMaterial::getTexture() const
	{
		return texture;
	}

	std::string GlitterMaterial::getSecondaryTexture() const
	{
		return secondaryTexture;
	}

	BlendMode GlitterMaterial::getBlendMode() const
	{
		return blendMode;
	}

	AddressMode GlitterMaterial::getAddressMode() const
	{
		return addressMode;
	}

	Vector2 GlitterMaterial::getSplit() const
	{
		return split;
	}

	Shader &GlitterMaterial::getShader()
	{
		return shader;
	}

	void GlitterMaterial::setName(std::string n)
	{
		if (n.size())
			name = n;
	}

	void GlitterMaterial::setTexture(std::string tex)
	{
		if (tex.size())
			texture = tex;
	}

	void GlitterMaterial::setSecondaryTexture(std::string tex)
	{
		if (tex.size())
			secondaryTexture = tex;
	}

	void GlitterMaterial::setBlendMode(BlendMode mode)
	{
		blendMode = mode;
	}

	void GlitterMaterial::setAddressMode(AddressMode mode)
	{
		addressMode = mode;
	}

	void GlitterMaterial::setSplit(Vector2 uv)
	{
		if (uv.x >= 1.0f && uv.y >= 1.0f)
			split = uv;
	}

	void GlitterMaterial::read(const std::string& filename)
	{
		tinyxml2::XMLDocument* xml = BIXF::parseBIXF(filename);
		tinyxml2::XMLElement* element = xml->FirstChildElement("Material");

		name = element->FindAttribute("Name")->Value();

		texture	= BIXF::toString(element->FirstChildElement("Texture"));
		secondaryTexture = BIXF::toString(element->FirstChildElement("SecondaryTexture"));

		std::string blendStr = BIXF::toString(element->FirstChildElement("BlendMode"));
		blendMode = (BlendMode)glitterStringToEnum(blendModeTable, blendModeTableSize, blendStr);

		std::string addressStr = BIXF::toString(element->FirstChildElement("AddressMode"));
		addressMode = (AddressMode)glitterStringToEnum(addressModeTable, addressModeTableSize, addressStr);

		split = BIXF::toUV(element->FirstChildElement("Split"));

		Shader s;
		element = element->FirstChildElement("Shader");
		s.name = element->FindAttribute("Name")->Value();

		tinyxml2::XMLElement* parameterElement = element->FirstChildElement("Parameter");
		unsigned int count = 0;
		while (parameterElement)
		{
			s.parameters[count].id = parameterElement->FindAttribute("Id")->UnsignedValue();
			s.parameters[count].value = parameterElement->FindAttribute("Value")->FloatValue();
			
			++count;
			parameterElement = parameterElement->NextSiblingElement("Parameter");
		}
		shader = s;

		delete xml;
	}

	void GlitterMaterial::prepare(tinyxml2::XMLDocument* xml)
	{
		tinyxml2::XMLElement* materialElement = xml->NewElement("Material");
		materialElement->SetAttribute("Name", name.c_str());

		BIXF::createChildValue(materialElement, "Texture", texture);
		if (secondaryTexture.size())
		{
			BIXF::createChildValue(materialElement, "SecondaryTexture", secondaryTexture);
		}

		std::string blendStr = glitterEnumToString(blendModeTable, blendModeTableSize, (size_t)blendMode);
		BIXF::createChildValue(materialElement, "BlendMode", blendStr);

		std::string addressStr = glitterEnumToString(addressModeTable, addressModeTableSize, (size_t)addressMode);
		BIXF::createChildValue(materialElement, "AddressMode", addressStr);

		BIXF::createChildUV(materialElement, "Split", split);

		tinyxml2::XMLElement* shaderElement = materialElement->InsertNewChildElement("Shader");
		shaderElement->SetAttribute("Name", shader.name.c_str());

		for (unsigned int count = 0; count < 4; ++count)
		{
			tinyxml2::XMLElement* parameterElement = shaderElement->InsertNewChildElement("Parameter");
			parameterElement->SetAttribute("Id", shader.parameters[count].id);
			parameterElement->SetAttribute("Value", shader.parameters[count].value);
		}

		xml->InsertEndChild(materialElement);
	}

	void GlitterMaterial::write(const std::string& filename)
	{
		tinyxml2::XMLDocument* xml = new tinyxml2::XMLDocument();
		prepare(xml);
		this->filename = filename;

		BIXF::convertToBIXF(xml, filename);
		delete xml;
	}

	void GlitterMaterial::writeXML(const std::string& filename)
	{
		tinyxml2::XMLDocument* xml = new tinyxml2::XMLDocument();
		prepare(xml);

		xml->SaveFile(filename.c_str());
		delete xml;
	}
}