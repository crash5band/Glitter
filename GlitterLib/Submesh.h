#pragma once
#include <vector>
#include <list>
#include <string>
#include "Vertex.h"
#include "VertexFormat.h"

namespace Glitter
{
	class Vertex;

	struct Polygon
	{
		unsigned int a, b, c;
	};

	class Submesh
	{
	private:
		std::vector<Vertex*> vertices;
		std::vector<unsigned short> faces;
		std::vector<Polygon> facesVectors;
		std::vector<unsigned char> boneTable;
		std::vector<std::string> textureUnits;
		std::vector<unsigned int> textureIds;
		std::string materialName;
		VertexFormat *vertexFormat;
		AABB aabb;
		std::string extra;
		std::vector<Vector3> points;

	public:
		Submesh();
		Submesh(Submesh* clone, Matrix4 transform, float uv2Left, float uv2Right, float uv2Top, float uv2Bottom);
		~Submesh();

		std::list<Vertex*> getVerticesList() const;
		std::list<unsigned int> getFacesList() const;
		std::vector<Polygon> getFaces() const;
		std::vector<unsigned char> getBoneTable() const;
		std::vector<std::string> getTextureUnits() const;
		std::vector<unsigned int> getTextureIds() const;
		std::string getMaterialName() const;
		VertexFormat getVertexFormat() const;
		AABB getAABB() const;
		std::string getExtra() const;
		bool hasExtra() const;
		std::vector<Vector3> getPoints() const;
		unsigned char getBone(unsigned int index) const;
		size_t getVerticesSize() const;
		size_t getFacesSize() const;
		
		unsigned int getEstimatedMemorySize() const;
		void build(std::vector<Vertex*> vertices, std::vector<Polygon> faceVectors);
		void fixVertexFormatForPC();
		void buildAABB();
		void setExtra(std::string ex);
		void setMaterialName(std::string name);
		void setVertexFormat(VertexFormat *vformat);
		void addBone(unsigned char bone);
		void addTextureUnit(std::string unit);
		void addTextureId(unsigned int id);
		void changeVertexFormat(int vformat);
		void read(BinaryReader* reader);
		void write(BinaryWriter* writer);

	};
}

