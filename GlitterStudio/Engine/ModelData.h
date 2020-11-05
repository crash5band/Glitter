#pragma once
#include "MeshData.h"
#include "Model.h"
#include "Shader.h"
#include "Transform.h"

class ModelData
{
private:
	std::vector<MeshData> meshes;
	std::vector<VertexData> vertices;
	Transform transform;
	std::string modelName;
	std::string directory;

	SubmeshData buildGensSubMesh(Glitter::Submesh *submesh);

public:
	ModelData(const std::string& path);
	ModelData();

	void buildGensModel(Glitter::Model &model);
	void reload(const std::string& path);
	void draw(Shader* shader);
	std::vector<VertexData>& getVertices();
	std::string getName() const;
};

