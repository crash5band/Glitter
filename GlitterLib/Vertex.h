#pragma once
#include "Submesh.h"
#include "VertexFormat.h"
#include "MathGens.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "Constants.h"

namespace Glitter
{
	class Submesh;

	class Vertex
	{
	private:
		Vector3 position;
		Vector3 normal;
		Vector3 tangent;
		Vector3 binormal;
		Vector2 uv[4];
		unsigned char boneIndices[4];
		unsigned char boneWeights[4];
		Submesh* parent;
		Color color;

	public:
		Vertex();
		Vertex(Vertex* clone, Matrix4 transform, float uv2Left, float uv2Right, float uv2Top, float uv2Bottom);

		bool operator==(const Vertex& v);
		Vector3 getTransformPosition(const Matrix4& m);
		Vector3 getPosition();
		Vector3 getNormal();
		Vector3 getTangent();
		Vector3 getBinormal();
		Vector2 getUV(unsigned int channel);
		unsigned int getBoneIndex(unsigned int index);
		unsigned int getBoneWeight(unsigned int index);
		Submesh* getParent();
		Color getColor();
		void setPosition(Vector3 pos);
		void setNormal(Vector3 norm);
		void setTangnet(Vector3 tan);
		void setBinormal(Vector3 binorm);
		void setUV(Vector2 uv, unsigned int index);
		void setBoneIndex(unsigned char value, unsigned int index);
		void setBoneWeight(unsigned char weight, unsigned int index);
		void setParent(Submesh* parent);
		void setColor(Color color);
		void transform(const Matrix4& m);

		void read(BinaryReader *reader, VertexFormat* vformat);
		void write(BinaryWriter* writer, VertexFormat* vformat);
	};
}

