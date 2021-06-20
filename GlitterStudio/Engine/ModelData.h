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

	bool reload(const std::string& path);
	void dispose();
	void buildGensModel(Glitter::Model &model);
	void draw(Shader* shader, float time);
	void move(Glitter::Vector3 pos);
	void rotate(Glitter::Quaternion q);
	void scale(Glitter::Vector3 scale);

	std::vector<VertexData>& getVertices();
	std::vector<std::shared_ptr<Glitter::Material>> getMaterials();
	std::string getName() const;
	Transform getTransform();
};

