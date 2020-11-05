#include "ModelData.h"
#include "Material.h"
#include "File.h"
#include "ResourceManager.h"

ModelData::ModelData(const std::string& path)
{
	reload(path);
}

ModelData::ModelData()
{

}

void ModelData::buildGensModel(Glitter::Model &model)
{
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

		vData.position.x = vertex->getPosition().x;
		vData.position.y = vertex->getPosition().y;
		vData.position.z = vertex->getPosition().z;

		vData.normal.x = vertex->getNormal().x;
		vData.normal.y = vertex->getNormal().y;
		vData.normal.z = vertex->getNormal().z;

		vData.tangent.x = vertex->getTangent().x;
		vData.tangent.y = vertex->getTangent().y;
		vData.tangent.z = vertex->getTangent().z;

		vData.binormal.x = vertex->getBinormal().x;
		vData.binormal.y = vertex->getBinormal().y;
		vData.binormal.z = vertex->getBinormal().z;

		for (int channel = 0; channel < 4; ++channel)
		{
			vData.uv[channel].x = vertex->getUV(channel).x;
			vData.uv[channel].y = vertex->getUV(channel).y;
		}

		vData.color.r = vertex->getColor().r;
		vData.color.g = vertex->getColor().g;
		vData.color.b = vertex->getColor().b;
		vData.color.a = vertex->getColor().a;

		vertexData.push_back(vData);
	}

	// process faces
	std::list<unsigned int> faces = submesh->getFacesList();
	std::vector<unsigned int> faceData;
	faceData.reserve(faces.size());

	for (std::list<unsigned int>::iterator fIt = faces.begin(); fIt != faces.end(); ++fIt)
	{
		faceData.push_back((*fIt));
	}

	// process materials
	std::string materialName = submesh->getMaterialName();
	Glitter::Material material(directory + materialName + ".material");

	std::vector<Glitter::Texture*> textures = material.getTextures();
	std::vector<TextureData> texturesData;
	texturesData.reserve(textures.size());

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

		ResourceManager::loadTexture(directory + textureName + ".dds", slot);
		std::shared_ptr<TextureData> texture = ResourceManager::getTexture(textureName + ".dds");
		
		// some materials have weird texture internal names that do not reflect the texture filename
		// or it could be that they are not located in the same directory
		if (texture)
			texturesData.push_back(*texture.get());
	}

	return SubmeshData(vertexData, faceData, texturesData, PirimitveType::TriangleStrip);
}

void ModelData::draw(Shader* shader)
{
	for (auto& mesh : meshes)
		mesh.draw(shader);
}

std::vector<VertexData>& ModelData::getVertices()
{
	return vertices;
}

void ModelData::reload(const std::string& path)
{
	Glitter::Model model(path);
	modelName = Glitter::File::getFileName(path);
	directory = Glitter::File::getFilePath(path);
	buildGensModel(model);
}

std::string ModelData::getName() const
{
	return modelName;
}