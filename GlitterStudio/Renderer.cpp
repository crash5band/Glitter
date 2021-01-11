#include "Renderer.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Utilities.h"
#include "ResourceManager.h"
#include "..\DirectXMath-master\Inc\DirectXMath.h"

Renderer::Renderer() :
	numVertices{ 0 }, numIndices{ 0 }, numQuads{ 0 }, texID{ -1 }, batchStarted{ false }
{
	size_t offset = 0;
	for (size_t index = 0; index < maxIndices; index += 6)
	{
		indices[index + 0] = (offset + 0);
		indices[index + 1] = (offset + 1);
		indices[index + 2] = (offset + 2);

		indices[index + 3] = (offset + 2);
		indices[index + 4] = (offset + 3);
		indices[index + 5] = (offset + 0);

		offset += 4;
	}

	initQuad();
	bufferBase = buffer;

	billboardShader		= ResourceManager::getShader("BillboardParticle");
	meshParticleShader	= ResourceManager::getShader("MeshParticle");
	blendMode = Glitter::BlendMode::Zero;
	setBlendMode(Glitter::BlendMode::Add);
}

Renderer::~Renderer()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void Renderer::bindShader(std::shared_ptr<Shader>& s)
{
	shader = s;
	shader->use();
}

Glitter::Vector3 Renderer::getAnchorPoint(Glitter::PivotPosition pivot)
{
	switch (pivot)
	{
	case Glitter::PivotPosition::TopLeft:
		return Glitter::Vector3(-0.5, 0.5, 0);

	case Glitter::PivotPosition::TopCenter:
		return Glitter::Vector3(0, 0.5, 0);

	case Glitter::PivotPosition::TopRight:
		return Glitter::Vector3(0.5, 0.5, 0);

	case Glitter::PivotPosition::MiddleLeft:
		return Glitter::Vector3(-0.5, 0, 0);

	case Glitter::PivotPosition::MiddleRight:
		return Glitter::Vector3(0.5, 0, 0);

	case Glitter::PivotPosition::BottomLeft:
		return Glitter::Vector3(-0.5, -0.5, 0);

	case Glitter::PivotPosition::BottomCenter:
		return Glitter::Vector3(0, -0.5, 0);

	case Glitter::PivotPosition::BottomRight:
		return Glitter::Vector3(0.5, -0.5, 0);

	default:
		return Glitter::Vector3(0, 0, 0);
	}
}

void Renderer::resetVPos()
{
	DirectX::XMVECTOR topRight{ 0.5f, 0.5f, 0.0f, 1.0f };
	DirectX::XMVECTOR bottomRight{ 0.5f, -0.5f, 0.0f, 1.0f };
	DirectX::XMVECTOR bottomLeft{ -0.5f, -0.5f, 0.0f, 1.0f };
	DirectX::XMVECTOR topLeft{ -0.5f, 0.5f, 0.0f, 1.0f };

	vPos[0] = topRight;
	vPos[1] = bottomRight;
	vPos[2] = bottomLeft;
	vPos[3] = topLeft;
}

void Renderer::initQuad()
{
	resetVPos();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(VertexBuffer), NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBuffer), (void*)offsetof(VertexBuffer, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBuffer), (void*)offsetof(VertexBuffer, color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexBuffer), (void*)offsetof(VertexBuffer, uv));

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Renderer::configureShader(std::shared_ptr<Shader>& s, Camera* camera, Glitter::Vector2 size)
{
	if (shader != s)
		bindShader(s);

	shader->setMatrix4("view", camera->getViewMatrix());
	shader->setMatrix4("projection", camera->getProjectionMatrix(size.x / size.y));
}

void Renderer::setBlendMode(Glitter::BlendMode mode)
{
	if (mode == Glitter::BlendMode::Zero || mode == Glitter::BlendMode::Opaque)
	{
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	}

	if (blendMode == mode)
		return;

	if (batchStarted)
		endBatch();

	switch (mode)
	{
	case Glitter::BlendMode::Multiply:
		glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
		glBlendEquation(GL_FUNC_SUBTRACT);
		break;

	case Glitter::BlendMode::Subtract:
		glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
		glBlendEquation(GL_FUNC_SUBTRACT);
		break;

	case Glitter::BlendMode::Typical:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		break;

	default:
		glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		break;
	}

	blendMode = mode;
}

void Renderer::beginBatch()
{
	bufferCurrent	= bufferBase;
	numVertices		= 0;
	numIndices		= 0;
	numQuads		= 0;
	batchStarted	= true;
}

void Renderer::endBatch()
{
	size_t size = (uint8_t*)bufferCurrent - (uint8_t*)bufferBase;
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, buffer);
	
	flush();
	texID = -1;
	batchStarted = false;
}

void Renderer::flush()
{
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}

void Renderer::getUVCoords(std::shared_ptr<MaterialNode> mat)
{
	auto tex = mat->getTexture();

	Glitter::Vector2 split = mat->getMaterial()->getSplit();
	size_t maxUV = (split.x * split.y) - 1;

	uvCoords.clear();
	uvCoords.reserve(maxUV + 1);
	for (size_t index = 0; index <= maxUV; ++index)
	{
		float x = index % (int)split.x;
		float y = index / (int)split.y;
		float w = tex->getWidth() / (int)split.x;
		float h = tex->getHeight() / (int)split.y;

		float left = (x * w) / tex->getWidth();
		float right = ((x * w) + w) / tex->getWidth();
		float top = (y * h) / tex->getHeight();
		float bottom = ((y * h) + h) / tex->getHeight();

		std::array<DirectX::XMVECTOR, 4> arr;
		arr[0] = DirectX::XMVECTOR{ right, top };
		arr[1] = DirectX::XMVECTOR{ right, bottom };
		arr[2] = DirectX::XMVECTOR{ left, bottom };
		arr[3] = DirectX::XMVECTOR{ left, top };
		uvCoords.emplace_back(arr);
	}
}

void Renderer::drawPoolMesh(ParticleInstance &instance, Camera* camera)
{
	Glitter::Vector3 pivot = getAnchorPoint(instance.getParticle()->getPivotPosition());
	std::vector<ParticleStatus> pool = instance.getPool();
	for (auto& p : pool)
	{
		if (!p.dead)
		{
			float rotX = Utilities::toRadians(p.transform.rotation.x);
			float rotY = Utilities::toRadians(p.transform.rotation.y);
			float rotZ = Utilities::toRadians(p.transform.rotation.z);

			DirectX::XMMATRIX model = DirectX::XMMatrixScaling(p.transform.scale.x, p.transform.scale.y, p.transform.scale.z);
			model *= DirectX::XMMatrixTranslation(-pivot.x, -pivot.y, -pivot.z);
			model *= DirectX::XMMatrixRotationRollPitchYaw(rotX, rotY, rotZ);
			model *= DirectX::XMMatrixTranslation(p.transform.position.x + pivot.x, p.transform.position.y + pivot.y, p.transform.position.z + pivot.z);

			meshParticleShader->setMatrix4("model", model);
			meshParticleShader->setVec4("color", DirectX::XMVECTOR{ p.color.r, p.color.g, p.color.b, p.color.a });
			instance.getReference()->getMesh()->draw(meshParticleShader.get());
		}
	}
}

void Renderer::drawPoolQuad(ParticleInstance& instance, Camera* camera)
{
	std::shared_ptr<MaterialNode> mat = instance.getReference()->getMaterialNode();
	std::vector<ParticleStatus> &pool = instance.getPool();

	getUVCoords(mat);
	for (auto& p : pool)
	{
		if (!p.dead)
		{
			drawQuad(p.transform, getAnchorPoint(instance.getParticle()->getPivotPosition()), p.color, p.UVIndex, mat->getTexture());
		}
	}
}

void Renderer::drawQuad(Transform& transform, Glitter::Vector3 pivot, Glitter::Color& color, unsigned int uvIndex, std::shared_ptr<TextureData> tex)
{
	if (numVertices >= maxVertices || texID != tex->getID())
	{
		endBatch();
		beginBatch();
	}

	if (texID == -1)
	{
		texID = tex->getID();
		glActiveTexture(GL_TEXTURE0);
		tex->use();
	}

	if (uvIndex >= uvCoords.size())
		uvIndex = uvCoords.size() - 1;

	float rotX = Utilities::toRadians(transform.rotation.x);
	float rotY = Utilities::toRadians(transform.rotation.y);
	float rotZ = Utilities::toRadians(transform.rotation.z);
	pivot *= transform.scale;

	DirectX::XMMATRIX model = DirectX::XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z);
	model *= DirectX::XMMatrixTranslation(-pivot.x, -pivot.y, -pivot.z);
	model *= DirectX::XMMatrixRotationRollPitchYaw(rotX, rotY, rotZ);
	model *= DirectX::XMMatrixTranslation(transform.position.x + pivot.x, transform.position.y + pivot.y, transform.position.z + pivot.z);
	DirectX::XMVECTOR colorV{ color.r, color.g, color.b, color.a };

	for (size_t i = 0; i < 4; ++i)
	{
		bufferCurrent->position = DirectX::XMVector3Transform(vPos[i], model);
		bufferCurrent->color = colorV;
		bufferCurrent->uv = uvCoords[uvIndex][i];
		bufferCurrent++;
	}

	numVertices += 4;
	numIndices += 6;
	++numQuads;
}

void Renderer::drawEffect(EffectNode* effNode, Camera* camera, const Glitter::Vector2 &viewportSize)
{
	for (auto& em : effNode->getEmitterNodes())
	{
		if (em->isVisible())
		for (auto& instance : em->getParticles())
		{
			if (!instance.isVisible())
				continue;

			std::shared_ptr<ParticleNode> node = instance.getReference();

			if (node->getMaterialNode())
			{
				Glitter::BlendMode mode = node->getParticle()->getBlendMode();
				if (mode == Glitter::BlendMode::Zero || mode == Glitter::BlendMode::UseMaterial)
					mode = node->getMaterialNode()->getMaterial()->getBlendMode();
				
				setBlendMode(mode);

				if (node->getParticle()->getType() == Glitter::ParticleType::Mesh)
				{
					if (node->getMesh())
					{
						if (batchStarted)
							endBatch();

						configureShader(meshParticleShader, camera, viewportSize);
						drawPoolMesh(instance, camera);
					}
				}
				else
				{
					if (node->getMaterialNode()->getTexture())
					{
						if (!batchStarted)
							beginBatch();

						configureShader(billboardShader, camera, viewportSize);
						drawPoolQuad(instance, camera);
					}
				}
			}
		}
	}

	if (batchStarted)
		endBatch();
}