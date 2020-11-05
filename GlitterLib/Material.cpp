#include "Material.h"
#include "Constants.h"
#include "File.h"

namespace Glitter
{
	const std::string Material::layerOpaque = "opaq";
	const std::string Material::layerTrans = "trans";
	const std::string Material::layerPunch = "punch";

	Material::Material()
	{
		shader = "Common_d";
		subShader = "Common_d";
		noCulling = false;
		colorBlend = false;
		extra = "";
		giExtra = "";
		materialFlag = 0x80;
		layer = layerOpaque;
	}

	Material::Material(std::string filename)
	{
		BinaryReader reader(filename, Endianness::BIG);

		name = filename;
		folder = "";
		layer = layerOpaque;

		name = File::getFileNameWithoutExtension(filename);
		folder = File::getFilePath(filename);

		extra = "";
		giExtra = "";
		noCulling = false;
		colorBlend = false;
		materialFlag = 0x80;

		if (reader.valid())
		{
			reader.readHeader();
			read(&reader);
			reader.close();
		}
	}

	Material::~Material()
	{
		for (std::vector<Parameter*>::iterator it = parameters.begin(); it != parameters.end(); ++it)
			delete (*it);

		for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end(); ++it)
			delete (*it);
	}

	void Material::save(std::string filename)
	{
		BinaryWriter writer(filename, Endianness::BIG);

		if (writer.valid())
		{
			writer.prepareHeader(MATERIAL_ROOT_GENERATIONS);
			write(&writer);
			writer.writeHeader(true);
			writer.close();
		}
	}

	std::string Material::getName() const
	{
		return name;
	}

	std::string Material::getFolder() const
	{
		return folder;
	}

	std::string Material::getShader() const
	{
		return shader;
	}

	std::string Material::getSubShader() const
	{
		return subShader;
	}

	std::string Material::getExtra() const
	{
		return extra;
	}

	std::string Material::getGiExtra() const
	{
		return giExtra;
	}

	std::string Material::getLayer() const
	{
		return layer;
	}

	std::vector<Texture*> Material::getTextures() const
	{
		return textures;
	}

	std::vector<Parameter*> Material::getParameters() const
	{
		return parameters;
	}

	Texture* Material::getTextureByUnit(std::string unit, size_t offset)
	{
		for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end(); ++it)
		{
			if ((*it)->getUnit() == unit)
			{
				if (offset)
					--offset;
				else
					return (*it);
			}
		}

		return nullptr;
	}

	Texture* Material::getTextureByIndex(size_t index)
	{
		if (index < textures.size())
			return textures[index];
		else
			return nullptr;
	}

	Parameter* Material::getParameterByName(std::string name)
	{
		for (std::vector<Parameter*>::iterator it = parameters.begin(); it != parameters.end(); ++it)
		{
			if ((*it)->name == name)
				return (*it);
		}

		return nullptr;
	}

	Parameter* Material::getParameterByIndex(size_t index)
	{
		if (index < parameters.size())
			return parameters[index];
		else
			return nullptr;
	}

	unsigned char Material::getMaterialFlag() const
	{
		return materialFlag;
	}

	bool Material::hasNoCulling() const
	{
		return noCulling;
	}

	bool Material::hasColorBlend() const
	{
		return colorBlend;
	}

	bool Material::hasExtra() const
	{
		return extra.size() > 0;
	}

	bool Material::hasGiExtra() const
	{
		return giExtra.size() > 0;
	}

	int Material::getTextureUnitsSize() const
	{
		return textures.size();
	}

	void Material::addParameter(Parameter* param)
	{
		if (param)
			parameters.push_back(param);
	}

	void Material::setParameter(std::string paramName, Color color)
	{
		Parameter* parameter = getParameterByName(paramName);
		if (parameter)
			parameter->color = color;
		else
			parameters.push_back(new Parameter(paramName, color));
	}

	void Material::removeParameter(std::string name)
	{
		for (std::vector<Parameter*>::iterator it = parameters.begin(); it != parameters.end(); ++it)
			if ((*it)->name == name)
				parameters.erase(it);
	}

	void Material::removeParameterByIndex(size_t index)
	{
		if (index < parameters.size())
			parameters.erase(parameters.begin() + index);
	}

	void Material::removeAllParameters()
	{
		parameters.clear();
	}

	void Material::addTextureUnit(Texture* texture)
	{
		textures.push_back(texture);
	}

	void Material::removeTextureUnit(std::string name)
	{
		for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end(); ++it)
			if ((*it)->getName() == name)
				textures.erase(it);
	}

	void Material::removeTextureUnitByIndex(size_t index)
	{
		if (index < textures.size())
			textures.erase(textures.begin() + index);
	}

	void Material::setName(std::string name)
	{
		this->name = name;
	}

	void Material::setShader(std::string shader)
	{
		this->shader = shader;
		this->subShader = shader;
	}

	void Material::setSubShader(std::string subshader)
	{
		subShader = subshader;
	}

	void Material::setExtra(std::string extra)
	{
		this->extra = extra;
	}

	void Material::setGiExtra(std::string extra)
	{
		giExtra = extra;
	}

	void Material::setMaterialFlag(unsigned char flag)
	{
		materialFlag = flag;
	}

	void Material::setCulling(bool culling)
	{
		noCulling = culling;
	}

	void Material::setColorBlend(bool blend)
	{
		colorBlend = blend;
	}

	void Material::setLayer(std::string lyr)
	{
		layer = lyr;
	}
	
	void Material::read(BinaryReader* reader)
	{
		if (!reader->valid())
			return;

		size_t headerAddress = reader->getCurrentAddress();

		size_t shaderAddress = reader->readAddress();
		size_t subshaderAddress = reader->readAddress();
		size_t texsetsAddress = reader->readAddress();
		size_t texturesAddress = reader->readAddress();

		materialFlag = reader->readChar();
		noCulling = reader->readChar();
		colorBlend = reader->readChar();

		reader->gotoAddress(headerAddress + 20);
		unsigned char parameterCount = reader->readChar();
		reader->gotoAddress(headerAddress + 23);
		unsigned char textureCount = reader->readChar();
		
		size_t parametersAddress = reader->readAddress();

		reader->gotoAddress(shaderAddress);
		shader = reader->readString();
		reader->gotoAddress(subshaderAddress);
		subShader = reader->readString();
		
		// HACK: We read the layer string if it's available right after the sub-shader one.
		layer = reader->readString();

		parameters.reserve(parameterCount);
		for (int i = 0; i < parameterCount; ++i)
		{
			reader->gotoAddress(parametersAddress + i * 4);
			size_t address = reader->readAddress();
			reader->gotoAddress(address);
			Parameter* parameter = new Parameter();
			parameter->read(reader);
			parameters.push_back(parameter);
		}

		textures.reserve(textureCount);
		for (int i = 0; i < textureCount; ++i)
		{
			reader->gotoAddress(texsetsAddress + i * 4);
			size_t texsetAddress = reader->readAddress();
			reader->gotoAddress(texsetAddress);
			std::string texset = reader->readString();

			reader->gotoAddress(texturesAddress + i * 4);
			size_t textureAddress = reader->readAddress();
			reader->gotoAddress(textureAddress);
			Texture* texture = new Texture();
			texture->read(reader, texset);
			textures.push_back(texture);
		}
	}

	void Material::write(BinaryWriter* writer)
	{
		if (!writer->valid())
			return;

		size_t headerAddress = writer->getCurrentAddress();
		size_t shaderAddress = 0;
		size_t subshaderAddress = 0;
		size_t texsetsAddress = 0;
		size_t texturesAddress = 0;

		writer->writeNull(16);
		writer->writeChar(materialFlag);
		writer->writeChar(noCulling);
		writer->writeChar(colorBlend);
		writer->writeNull(1);
		
		unsigned char parametersCount = parameters.size();
		unsigned char texturesCount = textures.size();
		writer->writeChar(parametersCount);
		writer->writeNull(2);
		writer->writeChar(texturesCount);

		writer->writeNull(12);
		shaderAddress = writer->getCurrentAddress();
		writer->writeString(shader.c_str());
		subshaderAddress = writer->getCurrentAddress();
		writer->writeString(subShader.c_str());

		// HACK: We store our own layer information right after the sub shader string. Does not affect game at all.
		writer->writeString(layer.c_str());
		writer->writeNull(1);
		writer->fixPadding();

		// parameters
		size_t parametersAddress = writer->getCurrentAddress();
		writer->writeNull(parametersCount * 4);
		std::vector<size_t> parameterAddresses;

		for (int i = 0; i < parametersCount; ++i)
		{
			parameterAddresses.push_back(writer->getCurrentAddress());
			parameters[i]->write(writer);
		}
		writer->fixPadding();

		for (int i = 0; i < parametersCount; ++i)
		{
			writer->gotoAddress(parametersAddress + i * 4);
			writer->writeAddress(parameterAddresses[i]);
		}
		writer->gotoEnd();

		// texset
		texsetsAddress = writer->getCurrentAddress();
		writer->writeNull(texturesCount * 4);
		std::vector<size_t> texsetAddresses;
		for (int i = 0; i < texturesCount; ++i)
		{
			texsetAddresses.push_back(writer->getCurrentAddress());
			writer->writeString(textures[i]->getTexSet().c_str());
		}
		writer->fixPadding();

		for (int i = 0; i < texturesCount; ++i)
		{
			writer->gotoAddress(texsetsAddress + i * 4);
			writer->writeAddress(texsetAddresses[i]);
		}
		writer->gotoEnd();

		// textures
		texturesAddress = writer->getCurrentAddress();
		writer->writeNull(texturesCount * 4);
		std::vector<size_t> textureAddresses;
		for (int i = 0; i < texturesCount; ++i)
		{
			textureAddresses.push_back(writer->getCurrentAddress());
			textures[i]->write(writer);
		}
		writer->fixPadding();

		for (int i = 0; i < texturesCount; ++i)
		{
			writer->gotoAddress(texturesAddress + i * 4);
			writer->writeAddress(textureAddresses[i]);
		}

		writer->gotoAddress(headerAddress);
		writer->writeAddress(shaderAddress);
		writer->writeAddress(subshaderAddress);
		writer->writeAddress(texsetsAddress);
		writer->writeAddress(texturesAddress);
		writer->moveAddress(8);
		writer->writeAddress(parametersAddress);
		writer->gotoEnd();
	}
}