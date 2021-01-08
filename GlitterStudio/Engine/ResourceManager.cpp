#include "ResourceManager.h"
#include "../Logger.h"
#include "File.h"

std::vector<std::shared_ptr<ModelData>> ResourceManager::models;
std::vector<std::shared_ptr<TextureData>> ResourceManager::textures;
std::vector<std::shared_ptr<Shader>> ResourceManager::shaders;

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

void ResourceManager::loadModel(const std::string& filepath)
{
	const std::string modelName = Glitter::File::getFileName(filepath);
	std::shared_ptr<ModelData> model = getModel(modelName);

	if (!model)
	{
		models.emplace_back(std::make_shared<ModelData>(filepath));
		Logger::log(Message(MessageType::Normal, std::string("loaded model " + modelName )));
	}
}

void ResourceManager::loadTexture(const std::string& filepath, TextureSlot slot)
{
	const std::string textureName = Glitter::File::getFileName(filepath);
	std::shared_ptr<TextureData> texture = getTexture(textureName);

	if (!texture)
	{
		textures.emplace_back(std::make_shared<TextureData>(filepath, slot));
		Logger::log(Message( MessageType::Normal, std::string("loaded texture " + textureName )));
	}
}

void ResourceManager::loadShader(const std::string& name, const std::string& vPath, const std::string& fPath)
{
	shaders.emplace_back(std::make_shared<Shader>(name, vPath.c_str(), fPath.c_str()));
	//Logger::log(Message( MessageType::Normal, std::string("loaded shader " + name )));
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
