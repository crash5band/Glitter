#pragma once
#include <vector>
#include <string>
#include "Bone.h"
#include "Mesh.h"

namespace Glitter
{
	class Model
	{
	private:
		std::vector<Mesh*> meshes;
		std::vector<Bone*> bones;
		std::string name;
		std::string filename;
		unsigned int modelFlag;
		bool terrainMode;
		AABB globalAABB;
	public:
		Model();
		Model(std::string filename);
		Model(BinaryReader* reader, bool terrainMode);
		~Model();

		std::vector<Mesh*> getMeshes();
		std::vector<Bone*> getBones();
		AABB getAABB();
		std::string getName();
		std::string getFilename();
		std::list<Vertex*> getVertexList();
		std::list<unsigned int> getFacesList();
		std::list<std::string> getMaterialNames();
		std::vector<unsigned int> getMaterialMappings(std::list<std::string>& materialNames);
		unsigned int getEstimatedMemorySize();
		int getBoneIndexByName(std::string &name);
		void save(std::string filename, int rootType = MODEL_ROOT_DYNAMIC_GENERATIONS);
		void readRootNode(BinaryReader* reader);
		void readSkeleton(BinaryReader* reader);
		void writeRootNode(BinaryWriter* writer);
		void buildAABB();
		void fixVertexFormatForPC();
		void setName(std::string name);
		void getTotalData(std::list<Vertex*> &vertexList, std::list<unsigned int> &facesList, std::list<std::string> &materialNames, std::vector<unsigned int> &materialMappings);
		void addMesh(Mesh* mesh);
		void cloneMesh(Mesh* mesh, Matrix4 transform, float uv2Left, float uv2Right, float uv2Top, float uv2Bottom);
		void mergeModel(Model* model, Matrix4 transform, float uv2Left, float uv2Right, float uv2Top, float uv2Bottom);
		void changeVertexFormat(int format);

	};
}
