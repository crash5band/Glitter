#include "Submesh.h"
#include "../tristripper/tri_stripper.h"

namespace Glitter
{
	Submesh::Submesh()
	{
		materialName = MODEL_SUBMESH_UNKNOWN_MATERIAL;
		vertexFormat = nullptr;
		extra = "";
	}

	Submesh::Submesh(Submesh* clone, Matrix4 transform, float uv2Left, float uv2Right, float uv2Top, float uv2Bottom)
	{
		size_t vertSize = clone->getVerticesSize();
		for (int i = 0; i < vertSize; ++i)
		{
			Vertex* vertex = new Vertex(clone->vertices[i], transform, uv2Left, uv2Right, uv2Top, uv2Bottom);
			vertex->setParent(this);
			vertices.push_back(vertex);
		}

		faces = clone->faces;
		facesVectors = clone->facesVectors;
		boneTable = clone->boneTable;
		textureUnits = clone->textureUnits;
		textureIds = clone->textureIds;
		materialName = clone->materialName;
		vertexFormat = new VertexFormat(clone->vertexFormat);
		buildAABB();
	}

	Submesh::~Submesh()
	{
		for (std::vector<Vertex*>::iterator it = vertices.begin(); it != vertices.end(); ++it)
			delete (*it);

		vertices.clear();

		if (vertexFormat)
			delete vertexFormat;
	}

	std::list<Vertex*> Submesh::getVerticesList() const
	{ 
		std::list<Vertex*> newList;
		std::copy(vertices.begin(), vertices.end(), std::back_inserter(newList));
		return newList;
	}

	std::list<unsigned int> Submesh::getFacesList() const
	{ 
		std::list<unsigned int> newList;
		for (int i = 0; i < facesVectors.size(); ++i)
		{
			newList.push_back((unsigned int)facesVectors[i].a);
			newList.push_back((unsigned int)facesVectors[i].b);
			newList.push_back((unsigned int)facesVectors[i].c);
		}

		return newList;
	}

	std::vector<Polygon> Submesh::getFaces() const
	{ 
		return facesVectors;
	}

	std::vector<unsigned char> Submesh::getBoneTable() const
	{ 
		return boneTable;
	}

	unsigned char Submesh::getBone(unsigned int index) const
	{
		if (index < boneTable.size())
			return boneTable[index];
	}

	std::vector<std::string> Submesh::getTextureUnits() const
	{ 
		return textureUnits;
	}

	std::vector<unsigned int> Submesh::getTextureIds() const
	{ 
		return textureIds;
	}

	std::string Submesh::getMaterialName() const
	{ 
		return materialName;
	}

	VertexFormat Submesh::getVertexFormat() const
	{ 
		return vertexFormat;
	}

	AABB Submesh::getAABB() const 
	{ 
		return aabb;
	}

	std::string Submesh::getExtra() const
	{ 
		return extra;
	}

	bool Submesh::hasExtra() const
	{ 
		return extra.size() > 0;
	}

	std::vector<Vector3> Submesh::getPoints() const
	{ 
		return points;
	}

	size_t Submesh::getVerticesSize() const
	{
		return vertices.size();
	}

	size_t Submesh::getFacesSize() const
	{
		return faces.size();
	}

	void Submesh::setExtra(std::string ex)
	{ 
		extra = ex;
	}

	void Submesh::setMaterialName(std::string name)
	{ 
		materialName = name;
	}

	void Submesh::setVertexFormat(VertexFormat *vformat) 
	{ 
		vertexFormat = vformat;
	}

	void Submesh::addBone(unsigned char bone)
	{
		boneTable.push_back(bone);
	}

	void Submesh::addTextureUnit(std::string unit)
	{
		textureUnits.push_back(unit);
	}

	void Submesh::addTextureId(unsigned int id)
	{
		textureIds.push_back(id);
	}

	void Submesh::changeVertexFormat(int format)
	{
		delete vertexFormat;
		setVertexFormat(new VertexFormat(format));
	}

	unsigned int Submesh::getEstimatedMemorySize() const
	{
		return vertexFormat->getSize() * vertices.size() + faces.size() * 2;
	}

	void Submesh::build(std::vector<Vertex*> vertices, std::vector<Polygon> facesVectors)
	{
		this->vertices = vertices;
		this->facesVectors = facesVectors;

		std::vector<Vertex*> new_vertices;
		new_vertices.clear();

		std::vector<unsigned short> newFaceMap;
		newFaceMap.clear();

		for (size_t x = 0; x < vertices.size(); x++) {
			Vertex* v = vertices[x];

			bool clone = false;
			for (unsigned int y = 0; y < new_vertices.size(); y++) {
				if ((*new_vertices[y]) == (*v)) {
					clone = true;
					newFaceMap.push_back(y);
					delete v;
					break;
				}
			}

			if (!clone) {
				new_vertices.push_back(v);
				newFaceMap.push_back(new_vertices.size() - 1);
			}
		}

		vertices = new_vertices;

		for (size_t i = 0; i < facesVectors.size(); i++) {
			facesVectors[i].a = newFaceMap[(int)facesVectors[i].a];
			facesVectors[i].b = newFaceMap[(int)facesVectors[i].b];
			facesVectors[i].c = newFaceMap[(int)facesVectors[i].c];
		}


		triangle_stripper::indices tri_indices;
		for (size_t i = 0; i < facesVectors.size(); i++) {
			tri_indices.push_back((int)facesVectors[i].a);
			tri_indices.push_back((int)facesVectors[i].b);
			tri_indices.push_back((int)facesVectors[i].c);
		}

		triangle_stripper::tri_stripper stripper(tri_indices);
		stripper.SetCacheSize(0);
		stripper.SetBackwardSearch(false);
		triangle_stripper::primitive_vector out_vector;
		stripper.Strip(&out_vector);

		for (size_t i = 0; i < out_vector.size(); i += 1) {
			if (out_vector[i].Type == triangle_stripper::TRIANGLE_STRIP) {
				for (size_t j = 0; j < out_vector[i].Indices.size(); j++) {
					faces.push_back(out_vector[i].Indices[j]);
				}
				faces.push_back(0xFFFF);
			}
			else {
				for (size_t j = 0; j < out_vector[i].Indices.size(); j += 3) {
					faces.push_back(out_vector[i].Indices[j]);
					faces.push_back(out_vector[i].Indices[j + 1]);
					faces.push_back(out_vector[i].Indices[j + 2]);
					faces.push_back(0xFFFF);
				}
			}
		}

		if (faces[faces.size() - 1] == 0xFFFF) faces.resize(faces.size() - 1);

		buildAABB();
	}

	void Submesh::fixVertexFormatForPC()
	{
		if (vertexFormat)
			vertexFormat->fixForPC();
	}

	void Submesh::buildAABB()
	{
		aabb.reset();
		for (int i = 0; i < vertices.size(); ++i)
			aabb.addPoint(vertices[i]->getPosition());
	}

	void Submesh::read(BinaryReader* reader)
	{
		size_t header = reader->getCurrentAddress();

		// header
		size_t materialNameAddress = reader->readAddress();
		unsigned int facesCount = reader->readInt32();
		size_t facesAddress = reader->readAddress();
		unsigned int verticesCount = reader->readInt32();
		unsigned int vertexSize = reader->readInt32();
		size_t verticesAddress = reader->readAddress();
		size_t vertexFormatAddress = reader->readAddress();
		unsigned int bonesSize = reader->readInt32();
		size_t bonesAddress = reader->readAddress();

		unsigned int textureUnitSize = reader->readInt32();
		size_t textureUnitAddress = reader->readAddress();

		reader->gotoAddress(facesAddress);
		faces.reserve(facesCount);
		for (int i = 0; i < facesCount; ++i)
		{
			unsigned short face = reader->readInt16();
			faces.push_back(face);
		}

		// convert faces to triangle strips
		unsigned short int face1 = 0;
		unsigned short int face2 = 0;
		unsigned short int face3 = 0;
		unsigned short int lastFace = 0;

		int newStrip = 3;
		int newIndex = 0;

		facesVectors.reserve(facesCount);
		for (int i = 0; i < facesCount; ++i)
		{
			unsigned short int t = faces[i];
			if (t == 0xffff)
			{
				newStrip = 3;
				newIndex = 0;
			}
			else
			{
				if (newStrip == 3)
					lastFace = t;

				newStrip -= 1;
				face3 = face2;
				face2 = face1;
				face1 = t;
				
				if (newStrip == 0)
				{
					Polygon poly = { face1, face2, face3 };
					Polygon invPoly = { face3, face2, face1 };

					if ((face1 != face2) && (face2 != face3) && (face1 != face3))
					{
						facesVectors.push_back((newIndex % 2) ? invPoly : poly);
					}

					newStrip = 1;
					++newIndex;
				}
			}
		}

		facesVectors.shrink_to_fit();

		// vertex format
		reader->gotoAddress(vertexFormatAddress);
		vertexFormat = new VertexFormat();
		vertexFormat->read(reader);
		vertexFormat->setSize(vertexSize);

		vertices.reserve(verticesCount);

		// vertices
		for (int i = 0; i < verticesCount; ++i)
		{
			reader->gotoAddress(verticesAddress + i * vertexSize);

			Vertex* v = new Vertex();
			v->read(reader, vertexFormat);
			v->setParent(this);
			vertices.push_back(v);
		}

		boneTable.reserve(bonesSize);

		// bone table
		for (int i = 0; i < bonesSize; ++i)
		{
			reader->gotoAddress(bonesAddress);

			unsigned char bone = reader->readChar();
			boneTable.push_back(bone);
		}

		textureUnits.reserve(textureUnitSize);
		textureIds.reserve(textureUnitSize);

		// material texture units
		for (int i = 0; i < textureUnitSize; ++i)
		{
			reader->gotoAddress(textureUnitAddress + i * 4);
			size_t textureUnitAddress = reader->readAddress();
			reader->gotoAddress(textureUnitAddress);

			size_t subTextureUnitAddress = reader->readAddress();
			unsigned int textureId = reader->readInt32();
			reader->gotoAddress(subTextureUnitAddress);

			std::string textureUnit = reader->readString();
			textureUnits.push_back(textureUnit);
			textureIds.push_back(textureId);
		}

		// material name
		reader->gotoAddress(materialNameAddress);
		materialName = reader->readString();
	}

	void Submesh::write(BinaryWriter* writer)
	{
		size_t headerAddress = writer->getCurrentAddress();

		//header
		size_t materialNameAddress = 0;
		unsigned int facesCount = faces.size();
		size_t facesAddress = 0;
		unsigned int verticesCount = vertices.size();
		unsigned int vertexSize = vertexFormat->getSize();
		size_t verticesAddress = 0;
		size_t vertexFormatAddress = 0;
		unsigned int bonesSize = boneTable.size();
		size_t bonesAddress = 0;
		unsigned int textureUnitsSize = textureUnits.size();
		size_t textureUnitsAddress = 0;

		writer->writeNull(4);
		writer->writeInt32(facesCount);
		writer->writeNull(4);
		writer->writeInt32(verticesCount);
		writer->writeInt32(vertexSize);
		writer->writeNull(8);
		writer->writeInt32(bonesSize);
		writer->writeNull(4);
		writer->writeInt32(textureUnitsSize);
		writer->writeNull(4);

		// faces in traingle strip format
		facesAddress = writer->getCurrentAddress();
		for (int i = 0; i < facesCount; ++i)
		{
			writer->writeInt16(faces[i]);
		}
		writer->fixPadding();

		// vertices
		verticesAddress = writer->getCurrentAddress();
		for (int i = 0; i < verticesCount; ++i)
			vertices[i]->write(writer, vertexFormat);

		// vertex format
		vertexFormatAddress = writer->getCurrentAddress();
		vertexFormat->write(writer);

		// bone table
		bonesAddress = writer->getCurrentAddress();
		for (int i = 0; i < bonesSize; ++i)
			writer->writeChar(boneTable[i]);

		writer->fixPadding();

		// texture units
		textureUnitsAddress = writer->getCurrentAddress();
		std::vector<unsigned int> textureUnitsAddresses;
		writer->writeNull(4 * textureUnitsSize);

		for (int i = 0; i < textureUnitsSize; ++i)
		{
			textureUnitsAddresses.push_back(writer->getCurrentAddress());

			size_t textureUnitAddress = writer->getCurrentAddress() + 8;
			writer->writeAddress(textureUnitAddress);
			writer->writeInt32(textureIds[i]);
			writer->writeString(textureUnits[i].c_str());
			writer->fixPadding();
		}

		for (int i = 0; i < textureUnits.size(); ++i)
		{
			writer->gotoAddress(textureUnitsAddress + i * 4);
			writer->writeInt32(textureUnitsAddresses[i]);
		}
		writer->gotoEnd();

		// material name
		materialNameAddress = writer->getCurrentAddress();
		writer->writeString(materialName.c_str());
		writer->fixPadding();

		// fill header with missing addresses
		writer->gotoAddress(headerAddress);
		writer->writeAddress(materialNameAddress);
		writer->moveAddress(4);
		writer->writeAddress(facesAddress);
		writer->moveAddress(8);
		writer->writeAddress(verticesAddress);
		writer->writeAddress(vertexFormatAddress);
		writer->writeNull(4);
		writer->writeInt32(bonesAddress);
		writer->moveAddress(4);
		writer->writeInt32(textureUnitsAddress);
		writer->gotoEnd();
	}

}