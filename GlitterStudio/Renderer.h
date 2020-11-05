#pragma once
#include "EffectNode.h"
#include "..\Dependencies\DirectXMath-master\Inc\DirectXMath.h"
#include <array>

constexpr size_t maxVertices	= 10000;
constexpr size_t maxIndices		= 15000;
constexpr size_t maxQuads		= 2500;

struct VertexBuffer
{
	DirectX::XMVECTOR position;
	DirectX::XMVECTOR color;
	DirectX::XMVECTOR uv;
};

class Renderer
{
private:
	size_t numVertices;
	size_t numIndices;
	size_t numQuads;
	VertexBuffer* bufferBase;
	VertexBuffer* bufferCurrent;
	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;
	int texID;
	bool batchStarted;

	std::shared_ptr<Shader> billboardShader;
	std::shared_ptr<Shader> meshParticleShader;

	VertexBuffer buffer[maxVertices];
	std::array<unsigned int, maxIndices> indices;
	std::array<DirectX::XMVECTOR, 4> vPos;
	std::vector<std::array<DirectX::XMVECTOR, 4>> uvCoords;
	std::shared_ptr<Shader> shader;

	Glitter::BlendMode blendMode;

	void initQuad();
	void resetVPos();
	void drawPoolQuad(ParticleInstance& instance, Camera* camera);
	void drawPoolMesh(ParticleInstance& instance, Camera* camera);
	void getUVCoords(std::shared_ptr<MaterialNode> mat);
	Glitter::Vector3 getAnchorPoint(Glitter::PivotPosition pivot);

public:
	Renderer();
	~Renderer();

	void bindShader(std::shared_ptr<Shader>& shader);
	void configureShader(std::shared_ptr<Shader>& shader, Camera* camera, Glitter::Vector2 size);
	void setBlendMode(Glitter::BlendMode mode);
	void beginBatch();
	void drawEffect(EffectNode* eff, Camera* camera, const Glitter::Vector2 &viewportSize);
	void drawQuad(Transform &transform, Glitter::Vector3 pivot, Glitter::Color &color, unsigned int uvIndex, std::shared_ptr<TextureData> tex);
	void flush();
	void endBatch();
};

