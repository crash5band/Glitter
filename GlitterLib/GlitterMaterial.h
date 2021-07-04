#pragma once
#include "Particle.h"
#include <string>
#include <vector>

namespace Glitter
{
	constexpr const char* defaultShader = "BillboardParticle_d[v]";

	struct Parameter
	{
		unsigned int id;
		float value;
	};

	struct Shader
	{
		std::string name;
		Parameter parameters[4];

		Shader() : name{ defaultShader }
		{
			for (unsigned int i = 0; i < 4; ++i)
				parameters[i] = Parameter{ i, 0 };
		}

		Shader(std::string n) : name{ n }
		{
			for (unsigned int i = 0; i < 4; ++i)
				parameters[i] = Parameter{ i, 0 };
		}

	};

	class GlitterMaterial
	{
	private:
		std::string name;
		std::string texture;
		std::string secondaryTexture;
		BlendMode blendMode;
		AddressMode addressMode;
		Vector2 split;
		Shader shader;
		std::string filename;

		void prepare(tinyxml2::XMLDocument* xml);

	public:
		GlitterMaterial(const std::string& name, BlendMode blend);
		GlitterMaterial(const std::string& filepath);

		std::string getName() const;
		std::string getTexture() const;
		std::string getSecondaryTexture() const;
		BlendMode getBlendMode() const;
		AddressMode getAddressMode() const;
		Vector2 getSplit() const;
		Shader &getShader();
		std::string getFilename() const;

		void setFilename(const std::string& filename);
		void setName(std::string n);
		void setTexture(std::string tex);
		void setSecondaryTexture(std::string tex);
		void setBlendMode(BlendMode mode);
		void setAddressMode(AddressMode mode);
		void setSplit(Vector2 uv);

		void read(const std::string& filename);
		void write(const std::string& filename);
		void writeXML(const std::string& filename);
	};
}
