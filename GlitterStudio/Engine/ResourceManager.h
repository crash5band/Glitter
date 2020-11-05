#pragma once
#include "ModelData.h"

class ResourceManager
{
private:
	static std::vector<std::shared_ptr<ModelData>> models;
	static std::vector<std::shared_ptr<TextureData>> textures;
	static std::vector<std::shared_ptr<Shader>> shaders;

public:
	static std::shared_ptr<ModelData> getModel(const std::string& name);
	static std::shared_ptr<TextureData> getTexture(const std::string& name);
	static std::shared_ptr<Shader> getShader(const std::string& name);

	static void loadModel(const std::string& filepath);
	static void loadTexture(const std::string& filepath, TextureSlot slot);
	static void loadShader(const std::string& name, const std::string& vPath, const std::string& fPath);
	static void disposeAll();

	static size_t getModelCount();
	static size_t getTextureCount();
};

