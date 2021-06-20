#include "ModelData.h"
#include "Material.h"
#include "File.h"
#include "ResourceManager.h"
#include "../Logger.h"
#include <filesystem>

ModelData::ModelData(const std::string& path)
{
	reload(path);
}

ModelData::ModelData()
{

}

void ModelData::dispose()
{
	for (auto& mesh : meshes)
		mesh.dispose();
}

void ModelData::buildGensModel(Glitter::Model &model)
{
	// BAD: load all uv-anims in the current directory and check if any belongs to the model later
	if (!std::filesystem::exists(directory))
		return;

	/*
	for (const auto& file : std::filesystem::directory_iterator(directory))
	{
		if (file.path().extension().string() == ".uv-anim")
		{
			ResourceManager::loadUVAnimation(file.path().string());
		}
	}
	*/

	std::vector<Glitter::Mesh*> gensMeshes = model.getMeshes();
	meshes.reserve(meshes.size());

	for (int i = 0; i < gensMeshes.size(); ++i)
	{
		MeshData meshData;
		for (int slot = 0; slot < Glitter::MODEL_SUBMESH_SLOTS; ++slot)
		{
			std::vector<Glitter::Submesh*> slotSubmeshes = gensMeshes[i]->getSubmeshes(slot);

			for (int sub = 0; sub < slotSubmeshes.size(); ++sub)
			{
				SubmeshData submeshData = buildGensSubMesh(slotSubmeshes[sub]);
				submeshData.appendVerticesTo(vertices);
				meshData.addSubmesh(submeshData);
			}
		}

		meshes.push_back(meshData);
	}
}

SubmeshData ModelData::buildGensSubMesh(Glitter::Submesh *submesh)
{
	// process vertices
	std::list<Glitter::Vertex*> vertices = submesh->getVerticesList();
	std::vector<VertexData> vertexData;
	vertexData.reserve(vertices.size());

	for (std::list<Glitter::Vertex*>::iterator vIt = vertices.begin(); vIt != vertices.end(); ++vIt)
	{
		Glitter::Vertex* vertex = (*vIt);
		VertexData vData;

		vData.position	= vertex->getPosition();
		vData.normal	= vertex->getNormal();
		vData.tangent	= vertex->getTangent();
		vData.binormal	= vertex->getBinormal();

		for (int channel = 0; channel < 4; ++channel)
			vData.uv[channel] = vertex->getUV(channel);

		vData.color = vertex->getColor();

		vertexData.emplace_back(vData);
	}

	// process faces
	std::list<unsigned int> faces = submesh->getFacesList();
	std::vector<unsigned int> faceData;
	faceData.reserve(faces.size());

	for (std::list<unsigned int>::iterator fIt = faces.begin(); fIt != faces.end(); ++fIt)
	{
		faceData.emplace_back((*fIt));
	}

	// process materials
	std::string materialName = submesh->getMaterialName();
	ResourceManager::loadMaterial(directory + materialName + ".material");

	MaterialData matData;
	matData.material = ResourceManager::getMaterial(materialName);

	std::vector<Glitter::Texture*> textures = matData.material->getTextures();
	matData.textures.reserve(textures.size());

	// process textures
	for (std::vector<Glitter::Texture*>::iterator tIt = textures.begin(); tIt != textures.end(); ++tIt)
	{
		std::string textureName = (*tIt)->getName();
		std::string textureUnit = (*tIt)->getUnit();

		TextureSlot slot = TextureSlot::Diffuse;

		if (textureUnit == "gloss")
			slot = TextureSlot::Gloss;
		else if (textureUnit == "normal")
			slot = TextureSlot::Normal;
		else if (textureUnit == "specular")
			slot = TextureSlot::Specular;
		else if (textureUnit == "displacement")
			slot = TextureSlot::Displacement;
		else if (textureUnit == "reflection")
			slot = TextureSlot::Reflection;
		else if (textureUnit == "emissive")
			slot = TextureSlot::Emissive;

		ResourceManager::loadTexture(directory + textureName + ".dds", slot);
		std::shared_ptr<TextureData> texture = ResourceManager::getTexture(textureName + ".dds");
		
		if (texture)
			matData.textures.emplace_back(texture);
	}

	// load uv animations
	auto uvAnims = ResourceManager::getUVAnimationsList();
	std::shared_ptr<Glitter::UVAnimation> uvAnim;

	for (const auto& anim : uvAnims)
	{
		if (anim->getMaterial() == matData.material->getName() && anim->getTexset() == textures[0]->getTexSet())
		{
			Logger::log(Message(MessageType::Normal, "Assigned uv-anim" + anim->getName() + " to model " + modelName + "."));
			uvAnim = anim;
			break;
		}
	}

	return SubmeshData(vertexData, faceData, matData, uvAnim, PirimitveType::TriangleStrip);
}

void ModelData::draw(Shader* shader, float time)
{
	for (auto& mesh : meshes)
		mesh.draw(shader, time);
}

std::vector<VertexData>& ModelData::getVertices()
{
	return vertices;
}

bool ModelData::reload(const std::string& path)
{
	if (!Glitter::File::exists(path))
	{
		Logger::log(Message(MessageType::Error, "Failed to load " + path + ". File not found."));
		return false;
	}

	Glitter::Model model(path);
	modelName = Glitter::File::getFileName(path);
	directory = Glitter::File::getFilePath(path);
	buildGensModel(model);

	return true;
}

void ModelData::move(Glitter::Vector3 pos)
{
	transform.position = pos;
}

void ModelData::rotate(Glitter::Quaternion q)
{
	transform.rotation = q;
}

void ModelData::scale(Glitter::Vector3 scale)
{
	transform.scale = scale;
}

std::string ModelData::getName() const
{
	return modelName;
}

Transform ModelData::getTransform()
{
	return transform;
}

std::vector<std::shared_ptr<Glitter::Material>> ModelData::getMaterials()
{
	std::vector<std::shared_ptr<Glitter::Material>> materials;
	for (auto& mesh : meshes)
	{
		auto submeshes = mesh.getSubmeshes();
		for (auto& submesh : submeshes)
		{
			std::shared_ptr<Glitter::Material> mat = submesh.material.material;
			std::string name = mat->getName();
			materials.emplace_back(mat);
		}
	}

	return materials;
}