#pragma once
#include <string>
#include <vector>
#include "Texture.h"
#include "Parameter.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"

namespace Glitter
{
	class Material
	{
	private:
		std::string name;
		std::string folder;
		std::vector<Texture*> textures;
		std::vector<Parameter*> parameters;
		std::string shader;
		std::string subShader;
		std::string extra;
		std::string giExtra;
		std::string layer;
		unsigned char materialFlag;
		bool noCulling;
		bool colorBlend;

	public:
		static const std::string layerOpaque;
		static const std::string layerTrans;
		static const std::string layerPunch;

		Material();
		Material(std::string filename);
		~Material();

		std::string getName() const;
		std::string getFolder() const;
		std::string getShader() const;
		std::string getSubShader() const;
		std::string getExtra() const;
		std::string getGiExtra() const;
		std::string getLayer() const;
		std::vector<Texture*> getTextures() const;
		std::vector<Parameter*> getParameters() const;
		Texture* getTextureByUnit(std::string unit, size_t = 0);
		Texture* getTextureByIndex(size_t index);
		Parameter* getParameterByName(std::string name);
		Parameter* getParameterByIndex(size_t index);
		unsigned char getMaterialFlag() const;
		bool hasNoCulling() const;
		bool hasColorBlend() const;
		bool hasExtra() const;
		bool hasGiExtra() const;
		int getTextureUnitsSize() const;

		void addParameter(Parameter* param);
		void setParameter(std::string paramName, Color color);
		void removeParameter(std::string name);
		void removeParameterByIndex(size_t index);
		void removeAllParameters();
		void addTextureUnit(Texture* texture);
		void removeTextureUnit(std::string unit);
		void removeTextureUnitByIndex(size_t index);
		void setName(std::string name);
		void setShader(std::string shader);
		void setSubShader(std::string subshader);
		void setExtra(std::string extra);
		void setGiExtra(std::string extra);
		void setMaterialFlag(unsigned char flag);
		void setCulling(bool culling);
		void setColorBlend(bool blend);
		void setLayer(std::string layer);
		void read(BinaryReader* reader);
		void write(BinaryWriter* writer);
		void save(std::string filename);
	};
}

