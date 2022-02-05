#pragma once
#include "MeshData.h"
#include "Model.h"
#include "Shader.h"


class ModelData
{
private:
	std::vector<MeshData> meshes;
	std::vector<VertexData> vertices;
	std::string modelName;
	std::string directory;

	SubmeshData buildGensSubMesh(Glitter::Submesh *submesh);

public:
	ModelData(const std::string& path);
	ModelData();

	bool reload(const std::string& path);
	void dispose();
	void buildGensModel(Glitter::Model &model);
	void draw(Shader* shader, float time);

	std::vector<VertexData>& getVertices();
	std::vector<std::shared_ptr<Glitter::Material>> getMaterials();
	std::string getName() const;
};

