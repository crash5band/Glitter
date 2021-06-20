#include "ResourceManager.h"
#include "../Logger.h"
#include "File.h"

std::vector<std::shared_ptr<ModelData>> ResourceManager::models;
std::vector<std::shared_ptr<TextureData>> ResourceManager::textures;
std::vector<std::shared_ptr<Shader>> ResourceManager::shaders;
std::vector<std::shared_ptr<Glitter::Material>> ResourceManager::materials;
std::vector<std::shared_ptr<Glitter::UVAnimation>> ResourceManager::uvAnimations;

std::shared_ptr<ModelData> ResourceManager::getModel(const std::string& name)
{
	for (std::vector<std::shared_ptr<ModelData>>::iterator m = models.begin(); m != models.end(); ++m)
	{
		if ((*m)->getName() == name)
			return *m;
	}

	return nullptr;
}

std::shared_ptr<TextureData> ResourceManager::getTexture(const std::string& name)
{
	for (std::vector<std::shared_ptr<TextureData>>::iterator t = textures.begin(); t != textures.end(); ++t)
	{
		if ((*t)->getName() == name)
			return *t;
	}

	return nullptr;
}

std::shared_ptr<Shader> ResourceManager::getShader(const std::string& name)
{
	for (std::vector<std::shared_ptr<Shader>>::iterator s = shaders.begin(); s != shaders.end(); ++s)
	{
		if ((*s)->getName() == name)
			return *s;
	}

	return nullptr;
}

std::shared_ptr<Glitter::Material> ResourceManager::getMaterial(const std::string& name)
{
	for (std::vector<std::shared_ptr<Glitter::Material>>::iterator m = materials.begin(); m != materials.end(); ++m)
	{
		if ((*m)->getName() == name)
			return *m;

	}

	return nullptr;
}

std::shared_ptr<Glitter::UVAnimation> ResourceManager::getUVAnimation(const std::string& name)
{
	for (std::vector<std::shared_ptr<Glitter::UVAnimation>>::iterator uv = uvAnimations.begin(); uv != uvAnimations.end(); ++uv)
	{
		if ((*uv)->getName() == name)
			return *uv;
	}

	return nullptr;
}

void ResourceManager::loadModel(const std::string& filepath)
{
	const std::string modelName = Glitter::File::getFileName(filepath);
	std::shared_ptr<ModelData> model = getModel(modelName);

	// model not loaded
	if (!model)
	{
		model = std::make_shared<ModelData>();
		if (model->reload(filepath))
		{
			models.emplace_back(model);
			Logger::log(Message(MessageType::Normal, std::string("loaded model " + modelName)));
		}
	}
}

void ResourceManager::loadTexture(const std::string& filepath, TextureSlot slot)
{
	const std::string textureName = Glitter::File::getFileName(filepath);
	std::shared_ptr<TextureData> texture = getTexture(textureName);

	// texture not loaded
	if (!texture)
	{
		texture = std::make_shared<TextureData>();
		if (texture->reload(filepath, slot))
		{
			textures.emplace_back(texture);
			Logger::log(Message(MessageType::Normal, std::string("loaded texture " + textureName)));
		}
	}
}

void ResourceManager::loadShader(const std::string& name, const std::string& path)
{
	shaders.emplace_back(std::make_shared<Shader>(name, path));
	//Logger::log(Message( MessageType::Normal, std::string("loaded shader " + name )));
}

void ResourceManager::loadMaterial(const std::string& filepath)
{
	const std::string name = Glitter::File::getFileName(filepath);
	std::shared_ptr<Glitter::Material> material = getMaterial(name);

	if (!material)
	{
		material = std::make_shared<Glitter::Material>(filepath);
		materials.emplace_back(material);

		Logger::log(Message(MessageType::Normal, std::string("loaded material " + name)));
	}
}

void ResourceManager::loadUVAnimation(const std::string& filepath)
{
	const std::string uvName = Glitter::File::getFileNameWithoutExtension(filepath);
	std::shared_ptr<Glitter::UVAnimation> uvAnim = getUVAnimation(uvName);

	if (!uvAnim)
	{
		uvAnim = std::make_shared<Glitter::UVAnimation>(filepath);
		uvAnimations.emplace_back(uvAnim);

		Logger::log(Message(MessageType::Normal, std::string("loaded UV animation " + uvName)));
	}
}

std::vector<std::shared_ptr<ModelData>> ResourceManager::getModelList()
{
	return models;
}

std::vector<std::shared_ptr<TextureData>> ResourceManager::getTextureList()
{
	return textures;
}

std::vector<std::shared_ptr<Glitter::Material>> ResourceManager::getMaterialList()
{
	return materials;
}

std::vector<std::shared_ptr<Glitter::UVAnimation>> ResourceManager::getUVAnimationsList()
{
	return uvAnimations;
}

size_t ResourceManager::getModelCount()
{
	return models.size();
}

size_t ResourceManager::getTextureCount()
{
	return textures.size();
}

void ResourceManager::disposeAll()
{
	models.clear();
	textures.clear();
}

void ResourceManager::cleanModels()
{
	for (std::vector<std::shared_ptr<ModelData>>::iterator it = models.begin(); it != models.end();)
	{
		if ((*it).use_count() < 2)
		{
			(*it)->dispose();
			it = models.erase(it);
		}
		else
			++it;
	}
}

void ResourceManager::cleanTextures()
{
	for (std::vector<std::shared_ptr<TextureData>>::iterator it = textures.begin(); it != textures.end();)
	{
		if ((*it).use_count() < 2)
		{
			(*it)->dispose();
			it = textures.erase(it);
		}
		else
			++it;
	}
}
