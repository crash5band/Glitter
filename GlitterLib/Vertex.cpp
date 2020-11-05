#include "Vertex.h"
#include <list>

namespace Glitter
{
	Vertex::Vertex()
	{
		position = Vector3();
		normal = Vector3();
		tangent = Vector3();
		binormal = Vector3();
		for (int i = 0; i < 4; ++i)
		{
			uv[i] = Vector2();
			boneIndices[i] = (i == 0 ? 0 : 0xff);
			boneWeights[i] = (i == 0 ? 0xff : 0);
		}

		color = Color();
	}

	Vertex::Vertex(Vertex* clone, Matrix4 transform, float uv2Left, float uv2Right, float uv2Top, float uv2Bottom)
	{
		Vector3 pos, scale;
		Quaternion orient;
		transform.decomposition(pos, scale, orient);

		position = transform * clone->position;
		normal = orient * clone->normal;
		tangent = orient * clone->tangent;
		binormal = orient * clone->binormal;
		normal.normalise();
		tangent.normalise();
		binormal.normalise();

		for (int i = 0; i < 4; ++i)
		{
			uv[i] = clone->uv[i];
			boneIndices[i] = clone->boneIndices[i];
			boneWeights[i] = clone->boneIndices[i];
		}

		uv[1].x = uv2Left + uv[1].x * (uv2Right - uv2Left);
		uv[1].y = uv2Top + uv[1].y * (uv2Bottom - uv2Top);

		parent = nullptr;
	}

	bool Vertex::operator==(const Vertex& vertex)
	{
		if (position != vertex.position) return false;
		if (normal != vertex.normal) return false;
		if (tangent != vertex.tangent) return false;
		if (binormal != vertex.binormal) return false;

		for (size_t i = 0; i < 4; i++)
		{
			if (uv[i] != vertex.uv[i]) return false;
			if (boneIndices[i] != vertex.boneIndices[i]) return false;
			if (boneWeights[i] != vertex.boneWeights[i]) return false;
		}

		if (color != vertex.color) return false;
		return true;
	}

	Vector3 Vertex::getTransformPosition(const Matrix4& matrix)
	{
		return matrix * position;
	}

	Vector3 Vertex::getPosition() { return position; }

	Vector3 Vertex::getNormal() { return normal; }

	Vector3 Vertex::getBinormal() { return binormal; }

	Vector3 Vertex::getTangent() { return tangent; }

	Vector2 Vertex::getUV(unsigned int channel) { return uv[channel]; }

	unsigned int Vertex::getBoneIndex(unsigned int index) { return boneIndices[index]; }

	unsigned int Vertex::getBoneWeight(unsigned int index) { return boneWeights[index]; }

	Submesh* Vertex::getParent() { return parent; }

	Color Vertex::getColor() { return color; }

	void Vertex::setPosition(Vector3 pos) { position = pos; }

	void Vertex::setNormal(Vector3 norm) { normal = norm; }

	void Vertex::setTangnet(Vector3 tan) { tangent = tan; }

	void Vertex::setBinormal(Vector3 binorm) { binormal = binorm; }

	void Vertex::setParent(Submesh* submesh) { parent = submesh; }

	void Vertex::setColor(Color c) { color = c; }

	void Vertex::setUV(Vector2 v2, unsigned int index)
	{
		if (index < 4)
		{
			uv[index] = v2;
		}
	}

	void Vertex::setBoneIndex(unsigned char value, unsigned int index)
	{
		if (index < 4)
		{
			boneIndices[index] = value;
		}
	}

	void Vertex::setBoneWeight(unsigned char value, unsigned int index)
	{
		if (index < 4)
		{
			boneWeights[index] = value;
		}
	}

	void Vertex::transform(const Matrix4& matrix)
	{
		position = matrix * position;
	}

	void Vertex::read(BinaryReader* reader, VertexFormat* vformat)
	{
		size_t header_address = reader->getCurrentAddress();

		std::list<VertexFormatElement> reference = vformat->getElements();
		for (std::list<VertexFormatElement>::iterator it = reference.begin(); it != reference.end(); it++)
		{
			reader->gotoAddress(header_address + (*it).getOffset());

			switch ((*it).getId())
			{
			case POSITION:
				if ((*it).getData() == VECTOR3) position = reader->readVector3();
				break;
			case BONE_WEIGHTS:
				if ((*it).getData() == VECTOR4_CHAR) reader->readSize(boneWeights, 4);
				break;
			case BONE_INDICES:
				if ((*it).getData() == INDICES) reader->readSize(boneIndices, 4);
				break;
			case NORMAL:
				if ((*it).getData() == VECTOR3) normal = reader->readVector3();
				if ((*it).getData() == VECTOR3_360) normal = reader->readVector3Normal360();
				break;
			case UV:
				if ((*it).getData() == VECTOR2) uv[(*it).getIndex()] = reader->readVector2();
				if ((*it).getData() == VECTOR2_HALF) uv[(*it).getIndex()] = reader->readVector2Half();
				break;
			case BINORMAL:
				if ((*it).getData() == VECTOR3) binormal = reader->readVector3();
				if ((*it).getData() == VECTOR3_360) binormal = reader->readVector3Normal360();
				break;
			case TANGENT:
				if ((*it).getData() == VECTOR3) tangent = reader->readVector3();
				if ((*it).getData() == VECTOR3_360) tangent = reader->readVector3Normal360();
				break;
			case RGBA:
				if ((*it).getData() == VECTOR4) color = reader->readRGBA();
				if ((*it).getData() == VECTOR4_CHAR) color = reader->readABGR8();
				break;
			}
		}

		// Verify Bone Weights
		unsigned char totalWeight = 0;
		for (size_t i = 0; i < 4; i++)
			totalWeight += boneWeights[i];

		if (totalWeight != 0xFF)
			boneWeights[0] += 0xFF - totalWeight;
	}

	void Vertex::write(BinaryWriter* writer, VertexFormat* vformat)
	{
		size_t header_address = writer->getCurrentAddress();
		writer->writeNull(vformat->getSize());

		std::list<VertexFormatElement> reference = vformat->getElements();
		for (std::list<VertexFormatElement>::iterator it = reference.begin(); it != reference.end(); it++) {
			writer->gotoAddress(header_address + (*it).getOffset());

			switch ((*it).getId()) {
			case POSITION:
				if ((*it).getData() == VECTOR3) writer->write(position);
				break;
			case BONE_WEIGHTS:
				if ((*it).getData() == VECTOR4_CHAR) writer->writeSize(&boneWeights, 4);
				break;
			case BONE_INDICES:
				if ((*it).getData() == INDICES) writer->writeSize(&boneIndices, 4);
				break;
			case NORMAL:
				if ((*it).getData() == VECTOR3) writer->write(normal);
				break;
			case UV:
				if ((*it).getData() == VECTOR2) writer->write(uv[(*it).getIndex()]);
				break;
			case BINORMAL:
				if ((*it).getData() == VECTOR3) writer->write(binormal);
				break;
			case TANGENT:
				if ((*it).getData() == VECTOR3) writer->write(tangent);
				break;
			case RGBA:
				if ((*it).getData() == VECTOR4) writer->write(color, false);
				break;
			}
		}

		writer->gotoEnd();
	}
}