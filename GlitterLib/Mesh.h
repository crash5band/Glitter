#pragma once
#include <vector>
#include <string>
#include "Submesh.h"
#include "Constants.h"

namespace Glitter
{
	class Mesh
	{
	private:
		std::vector<Submesh*> submeshes[MODEL_SUBMESH_SLOTS];
		AABB aabb;
		std::string extra;
		std::string waterSlotString;

	public:
		Mesh();
		Mesh(Mesh* clone, Matrix4 transform, float uv2Left, float uv2Right, float uv2Top, float uv2Bottom);
		~Mesh();

		std::vector<Submesh*> getSubmeshes();
		std::vector<Submesh*> getSubmeshes(unsigned int slot);
		std::vector<Submesh*>* getSubmeshSlots();
		std::string getExtra();
		bool hasExtra();
		AABB getAABB();
		std::list<Vertex*> getVertexList();
		std::list<unsigned int> getFacesList();
		std::list<std::string> getMaterialNames();
		std::vector<unsigned int> getMaterialMappings(std::list<std::string> &materialNames);
		unsigned int getEstimatedMemorySize();
		void fixVertexFormatForPC();
		void changeVertexFormat(int type);
		void setExtra(std::string ex);
		void setWaterSlotString(std::string str);
		void buildAABB();
		void addSubmesh(Submesh* submesh, int slot);
		void removeSubmesh(Submesh* submesh, int slot);
		void read(BinaryReader* reader);
		void write(BinaryWriter* writer);
	};
}