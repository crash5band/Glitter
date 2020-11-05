#pragma once
#include <string>

enum class TextureSlot
{
	Diffuse,
	Gloss,
	Normal,
	Specular
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
	~TextureData();

	std::string getFullname() const;
	std::string getName() const;
	unsigned int getID() const;
	TextureSlot getSlot() const;
	int getWidth() const;
	int getHeight() const;

	void reload(const std::string& path, TextureSlot slot);
	void read();
	void use();
};

