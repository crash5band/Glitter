#pragma once
#include "MathGens.h"
#include "TextureData.h"
#include "Shader.h"
#include "Material.h"
#include "UVAnimation.h"
#include <memory>
#include <vector>

enum class PirimitveType
{
	Triangle,
	TriangleStrip
};

struct VertexData
{
	Glitter::Vector3 position;
	Glitter::Vector3 normal;
	Glitter::Vector3 tangent;
	Glitter::Vector3 binormal;
	Glitter::Vector2 uv[4];
	Glitter::Color color;
};

struct MaterialData
{
	std::shared_ptr<Glitter::Material> material;
	std::vector<std::shared_ptr<TextureData>> textures;
};

class SubmeshData
{
private:
	std::vector<VertexData> vertices;
	std::vector<unsigned int> faces;
	PirimitveType pirimitiveType;

	unsigned int vao, vbo, ebo;

	void build();
	void setMaterialParams(Shader* shader);

public:
	MaterialData material;

	SubmeshData(const std::vector<VertexData> &v, const std::vector<unsigned int> &i, const MaterialData &mat, PirimitveType pirimitive);
	~SubmeshData();

	void dispose();
	void appendVerticesTo(std::vector<VertexData>& list);
	void draw(Shader* shader, float time);
};