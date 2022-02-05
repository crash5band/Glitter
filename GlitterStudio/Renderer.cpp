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
		indices[index + 0] = offset + 0;
		indices[index + 1] = offset + 1;
		indices[index + 2] = offset + 2;

		indices[index + 3] = offset + 2;
		indices[index + 4] = offset + 3;
		indices[index + 5] = offset + 0;

		offset += 4;
	}

	initGrid();
	initQuad();
	bufferBase = buffer;

	billboardShader		= ResourceManager::getShader("BillboardParticle");
	meshParticleShader	= ResourceManager::getShader("MeshParticle");
	gridShader			= ResourceManager::getShader("Grid");
	meshShader			= ResourceManager::getShader("Mesh");

	// set different blending modes here so that setBlendMode does not return early
	blendMode = Glitter::BlendMode::Zero;
	setBlendMode(Glitter::BlendMode::Add);
}

Renderer::~Renderer()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);

	glDeleteBuffers(1, &gVbo);
	glDeleteVertexArrays(1, &gVao);
}

void Renderer::bindShader(std::shared_ptr<Shader>& s)
{
	shader = s;
	shader->use();
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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Renderer::configureShader(std::shared_ptr<Shader>& s, const Glitter::Editor::Viewport &vp, Glitter::BlendMode blend)
{
	if (shader != s)
		bindShader(s);

	Camera cam = vp.getCamera();
	Glitter::Vector2 size = vp.getSize();

	shader->setMatrix4("view", cam.getViewMatrix());
	shader->setMatrix4("projection", cam.getProjectionMatrix(size.x / size.y));
	if (shader->getName() != "Grid")
	{
		shader->setInt("blendMode", (int)blend);
	}
}

void Renderer::setBlendMode(Glitter::BlendMode mode)
{
	if (mode == Glitter::BlendMode::Zero || mode == Glitter::BlendMode::Opaque)
	{
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glEnable(GL_ALPHA);
		glEnablei(GL_BLEND, 0);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
	}

	if (blendMode == mode)
		return;

	if (batchStarted)
		endBatch();

	switch (mode)
	{
	case Glitter::BlendMode::Add:
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		break;

	case Glitter::BlendMode::Subtract:
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
		break;

	case Glitter::BlendMode::Multiply:
		glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_ONE);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		break;

	default:
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
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

void Renderer::getUVCoords(std::shared_ptr<Glitter::Editor::MaterialNode> mat)
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
		arr[0] = DirectX::XMVECTOR{ left, top };
		arr[1] = DirectX::XMVECTOR{ left, bottom };
		arr[2] = DirectX::XMVECTOR{ right, bottom };
		arr[3] = DirectX::XMVECTOR{ right, top };
		uvCoords.emplace_back(arr);
	}
}

void Renderer::drawPoolMesh(Glitter::Editor::ParticleInstance &instance, const Camera &camera)
{
	std::vector<Glitter::Editor::ParticleStatus> pool = instance.getPool();
	
	for (auto& p : pool)
	{
		if (!p.dead)
		{
			DirectX::XMMATRIX model = p.mat4;

			meshParticleShader->setMatrix4("model", model);
			meshParticleShader->setVec4("color", DirectX::XMVECTOR{ p.color.r, p.color.g, p.color.b, p.color.a });
			meshParticleShader->setVec2("uvOffset", DirectX::XMVECTOR{ p.uvScroll.x, p.uvScroll.y });
			instance.getReference()->getMesh()->draw(meshParticleShader.get(), 0);
		}
	}
}

void Renderer::drawPoolQuad(Glitter::Editor::ParticleInstance& instance, const Camera &camera)
{
	std::shared_ptr<Glitter::Editor::MaterialNode> mat = instance.getReference()->getMaterialNode();
	std::vector<Glitter::Editor::ParticleStatus> &pool = instance.getPool();

	getUVCoords(mat);
	if (instance.getParticle()->getType() == Glitter::ParticleType::Quad)
	{
		for (auto& p : pool)
		{
			if (!p.dead)
				drawQuad(p.mat4, p.color, p.UVIndex, p.uvScroll, mat->getTexture());
		}
	}
	else if (instance.getParticle()->getType() == Glitter::ParticleType::Locus)
	{
		for (auto& p : pool)
		{
			//if (!p.dead)
				drawLocus(p, mat->getTexture());
		}
	}
}

void Renderer::drawQuad(const DirectX::XMMATRIX& m4, const Glitter::Color& color, unsigned int uvIndex,
	const Glitter::Vector2 &uvS, std::shared_ptr<TextureData> tex)
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

	DirectX::XMMATRIX model = m4;
	DirectX::XMVECTOR colorV{ color.r, color.g, color.b, color.a };

	float ufactor = uvCoords[uvIndex][3].m128_f32[0] - uvCoords[uvIndex][0].m128_f32[0];
	float vFactor = uvCoords[uvIndex][1].m128_f32[1] - uvCoords[uvIndex][0].m128_f32[1];
	DirectX::XMVECTOR uvAdd{ ufactor * uvS.x, vFactor * uvS.y };
	for (size_t i = 0; i < 4; ++i)
	{
		bufferCurrent->position = DirectX::XMVector3Transform(vPos[i], model);
		bufferCurrent->color = colorV;

		DirectX::XMVECTOR uvResult = DirectX::XMVectorAdd(uvCoords[uvIndex][i], uvAdd);
		bufferCurrent->uv = uvResult;
		bufferCurrent++;
	}

	numVertices += 4;
	numIndices += 6;
	++numQuads;
}

void Renderer::drawLocus(const Glitter::Editor::ParticleStatus& p, std::shared_ptr<TextureData> tex)
{
	if (batchStarted)
		endBatch();

	bufferCurrent = bufferBase;

	texID = tex->getID();
	glActiveTexture(GL_TEXTURE0);
	tex->use();

	int uvIndex = p.UVIndex;
	if (uvIndex >= uvCoords.size())
		uvIndex = uvCoords.size() - 1;

	float stepUVY = (uvCoords[uvIndex][1].m128_f32[1] - uvCoords[uvIndex][0].m128_f32[1]) / (float)p.locusHistories.size();

	DirectX::XMVECTOR color{ p.color.r, p.color.g, p.color.b, p.color.a };

	for (int i = 0; i < p.locusHistories.size(); ++i)
	{
		Glitter::Vector3 locPos = p.locusHistories[i].pos;
		DirectX::XMVECTOR leftV{ -0.5f, 0.0f, 0.0f, 1.0f };
		DirectX::XMVECTOR rightV{ 0.5f, 0.0f, 0.0f, 1.0f };
		DirectX::XMVECTOR transformPos{ locPos.x, locPos.y, locPos.z, 0.0f };
		transformPos = DirectX::XMVector3Transform(transformPos, DirectX::XMMatrixInverse(nullptr, p.mat4));

		leftV = DirectX::XMVectorAdd(leftV, transformPos);
		leftV = DirectX::XMVector3Transform(leftV, p.mat4);
		rightV = DirectX::XMVectorAdd(rightV, transformPos);
		rightV = DirectX::XMVector3Transform(rightV, p.mat4);

		float uvY = 1 - (uvCoords[uvIndex][1].m128_f32[1] - (stepUVY * i));
		bufferCurrent->position = leftV;
		bufferCurrent->color = color;
		bufferCurrent->uv = DirectX::XMVECTOR{ uvCoords[uvIndex][0].m128_f32[0], uvY };
		bufferCurrent++;

		bufferCurrent->position = rightV;
		bufferCurrent->color = color;
		bufferCurrent->uv = DirectX::XMVECTOR{ uvCoords[uvIndex][2].m128_f32[0], uvY };
		bufferCurrent++;
	}

	size_t count = bufferCurrent - bufferBase;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(VertexBuffer), buffer);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, count);

	bufferCurrent = bufferBase;
}

void Renderer::drawEffect(Glitter::Editor::EffectNode* effNode, const Glitter::Editor::Viewport &vp)
{
	for (auto& em : effNode->getEmitterNodes())
	{
		if (em->isVisible())
		for (auto& instance : em->getParticles())
		{
			if (!instance.isVisible())
				continue;

			std::shared_ptr<Glitter::Editor::ParticleNode> node = instance.getReference();
			
			// particles must have a material bound to render
			if (node->getMaterialNode())
			{
				Glitter::BlendMode mode = node->getParticle()->getBlendMode();

				// switch to material's blend mode if the particle's is not set
				if (mode == Glitter::BlendMode::Zero)
					mode = node->getMaterialNode()->getMaterial()->getBlendMode();

				setBlendMode(mode);
				Glitter::ParticleDirectionType dir = instance.getParticle()->getDirectionType();

				if (node->getParticle()->getType() == Glitter::ParticleType::Mesh)
				{
					if (node->getMesh())
					{
						if (batchStarted)
							endBatch();

						configureShader(meshParticleShader, vp, mode);
						drawPoolMesh(instance, vp.getCamera());
					}
				}
				else
				{
					if (node->getMaterialNode()->getTexture())
					{
						if (!batchStarted)
							beginBatch();

						configureShader(billboardShader, vp, mode);
						drawPoolQuad(instance, vp.getCamera());
					}
				}
			}
		}
	}

	if (batchStarted)
		endBatch();
}

void Renderer::initGrid()
{
	int v = 0;
	gridBuffer = new VertexBuffer[gridVertexCount];

	DirectX::XMVECTOR xColor{ 0.3f, 0.3f, 0.3f, 0.7f };
	DirectX::XMVECTOR zColor(xColor);
	const float y = 0.0f;

	for (float x = -gridSize; x <= gridSize; x += gridSpacing)
	{
		xColor = { 0.3f, 0.3f, 0.3f, 0.7f };
		zColor = (xColor);

		if (x == 0)
		{
			// skip main x and z axes for now to draw them on top
			continue;
		}
		else if (abs(x) - abs((int)x) == 0.5f)
		{
			xColor = DirectX::XMVECTOR{ 0.2f, 0.2f, 0.2f, 0.5f };
			zColor = DirectX::XMVECTOR{ 0.2f, 0.2f, 0.2f, 0.5f };
		}

		gridBuffer[v].position = DirectX::XMVECTOR{ x, y, -gridSize};
		gridBuffer[v++].color = xColor;
		gridBuffer[v].position = DirectX::XMVECTOR{ x, y, gridSize};
		gridBuffer[v++].color = xColor;

		gridBuffer[v].position = DirectX::XMVECTOR{ -gridSize,y, x};
		gridBuffer[v++].color = zColor;
		gridBuffer[v].position = DirectX::XMVECTOR{ gridSize, y, x};
		gridBuffer[v++].color = zColor;
	}

	// draw main x and z axes
	xColor = { 0.7f, 0.2f, 0.2f, 0.7f };
	zColor = { 0.2f, 0.7f, 0.2f, 0.7f };

	gridBuffer[v].position = DirectX::XMVECTOR{ 0, y, -gridSize };
	gridBuffer[v++].color = xColor;
	gridBuffer[v].position = DirectX::XMVECTOR{ 0, y, gridSize };
	gridBuffer[v++].color = xColor;

	gridBuffer[v].position = DirectX::XMVECTOR{ -gridSize,y, 0 };
	gridBuffer[v++].color = zColor;
	gridBuffer[v].position = DirectX::XMVECTOR{ gridSize, y, 0 };
	gridBuffer[v++].color = zColor;

	glGenVertexArrays(1, &gVao);
	glBindVertexArray(gVao);

	glGenBuffers(1, &gVbo);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo);

	glBufferData(GL_ARRAY_BUFFER, gridVertexCount * sizeof(VertexBuffer), gridBuffer, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBuffer), (void*)offsetof(VertexBuffer, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBuffer), (void*)offsetof(VertexBuffer, color));

	glBindVertexArray(0);
}

void Renderer::drawGrid(const Glitter::Editor::Viewport &vp)
{
	configureShader(gridShader, vp, Glitter::BlendMode::Typical);
	setBlendMode(Glitter::BlendMode::Typical);

	glBindVertexArray(gVao);
	glDrawArrays(GL_LINES, 0, gridVertexCount);
}
