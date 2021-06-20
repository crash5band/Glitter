#include "SubmeshData.h"
#include "glad/glad.h"

SubmeshData::SubmeshData(const std::vector<VertexData> &v, const std::vector<unsigned int> &i, const MaterialData &m, std::shared_ptr<Glitter::UVAnimation> uv, PirimitveType pirimitive)
{
	vertices = v;
	faces = i;
	material = m;
	uvAnim = uv;
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

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, normal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, tangent));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, binormal));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, color));
	glEnableVertexAttribArray(4);

	glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, uv));
	glEnableVertexAttribArray(5);

	glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, uv[1]));
	glEnableVertexAttribArray(6);

	glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, uv[2]));
	glEnableVertexAttribArray(7);

	glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, uv[3]));
	glEnableVertexAttribArray(8);
	
	glBindVertexArray(0);
}

void SubmeshData::dispose()
{
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void SubmeshData::setMaterialParams(Shader* shader)
{
	// set material params
	Glitter::Color diffuse = material.material->getParameterByName("diffuse")->color;
	Glitter::Color ambient = material.material->getParameterByName("ambient")->color;
	Glitter::Color specular = material.material->getParameterByName("specular")->color;
	Glitter::Color emissive = material.material->getParameterByName("emissive")->color;
	Glitter::Color powGloss = material.material->getParameterByName("power_gloss_level")->color;

	shader->setVec4("material.diffuse", DirectX::XMVECTOR{ diffuse.r, diffuse.g, diffuse.b, diffuse.a });
	shader->setVec4("material.ambient", DirectX::XMVECTOR{ ambient.r, ambient.g, ambient.b, ambient.a });
	shader->setVec4("material.specular", DirectX::XMVECTOR{ specular.r, specular.g, specular.b, specular.a });
	shader->setVec4("material.emissive", DirectX::XMVECTOR{ emissive.r, emissive.g, emissive.b, emissive.a });
	shader->setVec4("material.power_gloss_level", DirectX::XMVECTOR{ powGloss.r, powGloss.g, powGloss.b, powGloss.a });

	// set material flags
	if (material.material->hasNoCulling())
	{
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glEnable(GL_CULL_FACE);
	}
}

void SubmeshData::appendVerticesTo(std::vector<VertexData>& list)
{
	list.insert(list.end(), vertices.begin(), vertices.end());
}

void SubmeshData::draw(Shader* shader, float time)
{
	// assign textures
	unsigned int diffuseIndex = 0;
	unsigned int glossIndex = 0;
	unsigned int normalIndex = 0;
	unsigned int emissiveIndex = 0;
	unsigned int specularIndex = 0;
	unsigned int reflectionIndex = 0;
	unsigned int displacementIndex = 0;
	for (unsigned int i = 0; i < material.textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		std::string samplerName;
		std::string samplerNum;

		std::shared_ptr<TextureData> &texture = material.textures[i];
		Glitter::Texture* tex = material.material->getTextureByIndex(i);

		switch (texture->getSlot())
		{
		case TextureSlot::Diffuse:
			samplerNum = std::to_string(diffuseIndex++);
			samplerName = "material.diffuse" + samplerNum;
			shader->setInt("material.diffIndex", tex->getUVIndex());
			break;

		case TextureSlot::Gloss:
			samplerNum = std::to_string(glossIndex++);
			samplerName = "material.gloss" + samplerNum;
			shader->setInt("material.glossIndex", tex->getUVIndex());
			break;

		case TextureSlot::Normal:
			samplerNum = std::to_string(normalIndex++);
			samplerName = "material.normal" + samplerNum;
			shader->setInt("material.normIndex", tex->getUVIndex());
			break;

		case TextureSlot::Displacement:
			samplerNum = std::to_string(displacementIndex++);
			samplerName = "material.displacement" + samplerNum;
			shader->setInt("material.disIndex", tex->getUVIndex());
			break;

		case TextureSlot::Emissive:
			samplerNum = std::to_string(emissiveIndex++);
			samplerName = "material.emissive" + samplerNum;
			shader->setInt("material.emsIndex", tex->getUVIndex());
			break;

		case TextureSlot::Specular:
			samplerNum = std::to_string(specularIndex++);
			samplerName = "material.specular" + samplerNum;
			shader->setInt("material.specIndex", tex->getUVIndex());
			break;

		case TextureSlot::Reflection:
			samplerNum = std::to_string(reflectionIndex++);
			samplerName = "material.reflection" + samplerNum;
			shader->setInt("material.refIndex", tex->getUVIndex());
			break;

		default:
			continue;
		}

		shader->setInt(samplerName, i);
		texture->setWrapMode(tex->getWrapModeU(), tex->getWrapModeV());
		texture->use();

		shader->setInt("material.hasNormal", normalIndex);
		shader->setInt("material.hasGloss", glossIndex);
	}

	setMaterialParams(shader);
	
	// animate uvs if available
	Glitter::Vector2 uvOffset;
	if (uvAnim)
	{
		Glitter::UVAnimationSet animSet = uvAnim->getAnimationSet();
		//uvOffset.x = animSet.getValue(time, Glitter::uAnim);
		//uvOffset.y = animSet.getValue(time, Glitter::vAnim);
	}

	shader->setVec2("material.uvOffset0", DirectX::XMVECTOR{ uvOffset.x, uvOffset.y });
	shader->setVec2("material.uvOffset1", DirectX::XMVECTOR{ uvOffset.x, uvOffset.y });
	shader->setVec2("material.uvOffset2", DirectX::XMVECTOR{ uvOffset.x, uvOffset.y });
	shader->setVec2("material.uvOffset3", DirectX::XMVECTOR{ uvOffset.x, uvOffset.y });

	// draw
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);
}