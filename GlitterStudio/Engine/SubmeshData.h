#pragma once
#include "MathGens.h"
#include "TextureData.h"
#include "Shader.h"
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

class SubmeshData
{
private:
	std::vector<VertexData> vertices;
	std::vector<unsigned int> faces;
	std::vector<std::shared_ptr<TextureData>> textures;
	PirimitveType pirimitiveType;

	unsigned int vao, vbo, ebo;

	void build();

public:
	SubmeshData(const std::vector<VertexData> &v, const std::vector<unsigned int> &i, const std::vector<std::shared_ptr<TextureData>>& t, PirimitveType pirimitive);
	SubmeshData(const std::vector<VertexData>& v, const std::vector<unsigned int>& i, std::shared_ptr<TextureData> & t, PirimitveType pirimitive);
	~SubmeshData();

	void dispose();
	void appendVerticesTo(std::vector<VertexData>& list);
	void draw(Shader* shader);
};