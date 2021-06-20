#pragma once
#include <string>

namespace Glitter
{
	enum class TextureWrapMode : uint8_t;
}

enum class TextureSlot
{
	Diffuse,
	Gloss,
	Normal,
	Specular,
	Emissive,
	Displacement,
	Reflection,
	SlotMax
};

class TextureData
{
private:
	unsigned int ID;
	std::string fullname;
	std::string textureName;
	TextureSlot slot;
	bool hasAlpha;
	int width;
	int height;

public:
	TextureData();
	TextureData(const std::string& path, TextureSlot slot);

	std::string getFullname() const;
	std::string getName() const;
	unsigned int getID() const;
	TextureSlot getSlot() const;
	int getWidth() const;
	int getHeight() const;

	unsigned int glWrapMode(Glitter::TextureWrapMode mode);

	bool reload(const std::string& path, TextureSlot slot);
	void read();
	void use();
	void setWrapMode(Glitter::TextureWrapMode u, Glitter::TextureWrapMode v);
	void dispose();
};

