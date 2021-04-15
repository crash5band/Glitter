#pragma once
#include "EffectNode.h"
#include "Viewport.h"
#include "..\Dependencies\DirectXMath-master\Inc\DirectXMath.h"
#include <array>

constexpr size_t maxVertices	= 10000;
constexpr size_t maxIndices		= 15000;
constexpr size_t maxQuads		= 2500;

constexpr float gridSize		= 10.0f;
constexpr float gridSpacing		= 0.5f;
constexpr int gridVertexCount	= (((gridSize * 2) / gridSpacing) + 1) * 4;

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
	VertexBuffer* gridBuffer;
	unsigned int vao, vbo, ebo, gVao, gVbo;
	int texID;
	bool batchStarted;

	std::shared_ptr<Shader> billboardShader;
	std::shared_ptr<Shader> meshParticleShader;
	std::shared_ptr<Shader> gridShader;

	VertexBuffer buffer[maxVertices];
	std::array<unsigned int, maxIndices> indices;
	std::array<DirectX::XMVECTOR, 4> vPos;
	std::vector<std::array<DirectX::XMVECTOR, 4>> uvCoords;
	std::shared_ptr<Shader> shader;

	Glitter::BlendMode blendMode;

	void initQuad();
	void initGrid();
	void resetVPos();
	void drawPoolQuad(ParticleInstance& instance, const Camera &camera);
	void drawPoolMesh(ParticleInstance& instance, const Camera &camera);
	void getUVCoords(std::shared_ptr<MaterialNode> mat);
	Glitter::Vector3 getAnchorPoint(Glitter::PivotPosition pivot);

public:
	Renderer();
	~Renderer();

	void bindShader(std::shared_ptr<Shader>& shader);
	void configureShader(std::shared_ptr<Shader>& shader, const Viewport &vp, Glitter::BlendMode blend);
	void setBlendMode(Glitter::BlendMode mode);
	void beginBatch();
	void drawGrid(const Viewport &vp);
	void drawEffect(EffectNode* eff, const Viewport &vp);
	void drawQuad(Transform &transform, Glitter::Vector3 pivot, Glitter::Color &color, unsigned int uvIndex, std::shared_ptr<TextureData> tex);
	void drawMesh(std::shared_ptr<ModelData> model, const Viewport &vp);
	void flush();
	void endBatch();
};
