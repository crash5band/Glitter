#include "SubmeshData.h"
#include "glad/glad.h"

SubmeshData::SubmeshData(const std::vector<VertexData> &v, const std::vector<unsigned int> &i, const std::vector<TextureData> &t, PirimitveType pirimitive)
{
	vertices = v;
	faces = i;
	textures = t;
	pirimitiveType = pirimitive;
	
	build();
}

SubmeshData::SubmeshData(const std::vector<VertexData>& v, const std::vector<unsigned int>& i, TextureData &t, PirimitveType pirimitive)
{
	vertices = v;
	faces = i;
	textures.push_back(t);
	pirimitiveType = pirimitive;

	build();
}

SubmeshData::~SubmeshData()
{
	//textures.clear();

	//glDeleteBuffers(1, &ebo);
	//glDeleteBuffers(1, &vbo);
	//glDeleteVertexArrays(1, &vao);
}

void SubmeshData::build()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(unsigned int), &faces[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, color));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, uv));
	glEnableVertexAttribArray(2);
	
	glBindVertexArray(0);
}

void SubmeshData::dispose()
{
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void SubmeshData::appendVerticesTo(std::vector<VertexData>& list)
{
	list.insert(list.end(), vertices.begin(), vertices.end());
}

void SubmeshData::draw(Shader* shader)
{
	unsigned int diffuseIndex = 0;
	for (unsigned int i = 0; i < textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		std::string samplerName;
		std::string samplerNum;

		TextureData &texture = textures[i];
		switch (texture.getSlot())
		{
		case TextureSlot::Diffuse:
			samplerNum = std::to_string(diffuseIndex++);
			samplerName = "material.diffuse" + samplerNum;
			break;

		default:
			continue;
		}

		shader->setInt(samplerName, i);
		texture.use();
	}

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);
}