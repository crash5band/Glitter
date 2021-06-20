#pragma once
#include "ModelData.h"
#include "UVAnimation.h"
#include "../FileGUI.h"

/// <summary>
/// Holds used models, textures and shaders and provides methods to load and retrive them.
/// </summary>
class ResourceManager
{
private:
	static std::vector<std::shared_ptr<ModelData>> models;
	static std::vector<std::shared_ptr<TextureData>> textures;
	static std::vector<std::shared_ptr<Shader>> shaders;
	static std::vector<std::shared_ptr<Glitter::Material>> materials;
	static std::vector<std::shared_ptr<Glitter::UVAnimation>> uvAnimations;

public:
	static std::shared_ptr<ModelData> getModel(const std::string& name);
	static std::shared_ptr<TextureData> getTexture(const std::string& name);
	static std::shared_ptr<Shader> getShader(const std::string& name);
	static std::shared_ptr<Glitter::Material> getMaterial(const std::string& name);
	static std::shared_ptr<Glitter::UVAnimation> getUVAnimation(const std::string& name);

	static std::vector<std::shared_ptr<ModelData>> getModelList();
	static std::vector<std::shared_ptr<TextureData>> getTextureList();
	static std::vector<std::shared_ptr<Glitter::Material>> getMaterialList();
	static std::vector<std::shared_ptr<Glitter::UVAnimation>> getUVAnimationsList();

	static void loadModel(const std::string& filepath);
	static void loadTexture(const std::string& filepath, TextureSlot slot);
	static void loadShader(const std::string& name, const std::string& path);
	static void loadMaterial(const std::string& filepath);
	static void loadUVAnimation(const std::string& filepath);
	static void disposeAll();
	static void cleanModels();
	static void cleanTextures();

	static size_t getModelCount();
	static size_t getTextureCount();
};

