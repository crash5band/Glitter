#include "Mesh.h"

namespace Glitter
{
	Mesh::Mesh()
	{
		for (int slot = 0; slot < MODEL_SUBMESH_SLOTS; ++slot)
		{
			submeshes[slot].clear();
		}

		aabb.reset();
		extra = "";
	}

	Mesh::Mesh(Mesh* clone, Matrix4 transform, float uv2Left, float uv2Right, float uv2Top, float uv2Bottom)
	{
		for (int slot = 0; slot < MODEL_SUBMESH_SLOTS; ++slot)
		{
			for (int sub = 0; sub < submeshes[slot].size(); ++sub)
			{
				Submesh* submesh = new Submesh(clone->submeshes[slot][sub], transform, uv2Left, uv2Right, uv2Top, uv2Bottom);
				submeshes->push_back(submesh);
			}
		}

		waterSlotString = clone->waterSlotString;
		buildAABB();
	}

	Mesh::~Mesh()
	{
		for (int slot = 0; slot < MODEL_SUBMESH_SLOTS; ++slot)
		{
			for (std::vector<Submesh*>::iterator it = submeshes[slot].begin(); it != submeshes[slot].end(); ++it)
				delete (*it);

			submeshes[slot].clear();
		}
	}

	void Mesh::buildAABB()
	{
		for (int slot = 0; slot < MODEL_SUBMESH_SLOTS; ++slot)
		{
			for (std::vector<Submesh*>::iterator it = submeshes[slot].begin(); it != submeshes[slot].end(); ++it)
				aabb.merge((*it)->getAABB());
		}
	}

	AABB Mesh::getAABB()
	{
		return aabb;
	}

	void Mesh::setExtra(std::string ex)
	{
		extra = ex;
	}

	std::string Mesh::getExtra()
	{
		return extra;
	}

	bool Mesh::hasExtra()
	{
		return extra.size() > 0;
	}

	void Mesh::setWaterSlotString(std::string str)
	{
		waterSlotString = str;
	}

	std::vector<Submesh*> Mesh::getSubmeshes()
	{
		std::vector<Submesh*> submeshV;
		for (int slot = 0; slot < MODEL_SUBMESH_SLOTS; ++slot)
		{
			for (std::vector<Submesh*>::iterator it = submeshes[slot].begin(); it != submeshes[slot].end(); ++it)
			{
				submeshV.push_back(*it);
			}
		}

		return submeshV;
	}

	std::vector<Submesh*> Mesh::getSubmeshes(unsigned int slot)
	{
		std::vector<Submesh*> submeshV;
		if (slot < MODEL_SUBMESH_SLOTS)
		{
			for (std::vector<Submesh*>::iterator it = submeshes[slot].begin(); it != submeshes[slot].end(); ++it)
				submeshV.push_back(*it);
		}

		return submeshV;
	}

	std::vector<Submesh*>* Mesh::getSubmeshSlots()
	{
		return submeshes;
	}

	std::list<Vertex*> Mesh::getVertexList()
	{
		std::list<Vertex*> submeshVertices;
		std::list<Vertex*> meshVertices;

		for (int slot = 0; slot < MODEL_SUBMESH_SLOTS; ++slot)
		{
			for (std::vector<Submesh*>::iterator it = submeshes[slot].begin(); it != submeshes[slot].end(); ++it)
			{
				submeshVertices = (*it)->getVerticesList();
				for (std::list<Vertex*>::iterator it = submeshVertices.begin(); it != submeshVertices.end(); ++it)
					meshVertices.push_back(*it);
			}
		}

		return meshVertices;
	}

	std::list<unsigned int> Mesh::getFacesList()
	{
		std::list<unsigned int> facesList;
		std::list<unsigned int> submeshFaces;

		unsigned int faceOffset = 0;

		for (int slot = 0; slot < MODEL_SUBMESH_SLOTS; ++slot)
		{
			for (std::vector<Submesh*>::iterator it = submeshes[slot].begin(); it != submeshes[slot].end(); ++it)
			{
				submeshFaces = (*it)->getFacesList();
				for (std::list<unsigned int>::iterator it = submeshFaces.begin(); it != submeshFaces.end(); ++it)
					facesList.push_back((*it) + faceOffset);

				faceOffset += (*it)->getVerticesList().size();
			}
		}

		return facesList;
	}

	std::list<std::string> Mesh::getMaterialNames()
	{
		std::list<std::string> materialNames;

		for (size_t slot = 0; slot < MODEL_SUBMESH_SLOTS; ++slot)
		{
			for (std::vector<Submesh*>::iterator it = submeshes[slot].begin(); it != submeshes[slot].end(); it++)
			{
				std::string materialName = (*it)->getMaterialName();

				bool found = false;
				for (std::list<std::string>::iterator it_m = materialNames.begin(); it_m != materialNames.end(); it_m++)
				{
					if ((*it_m) == materialName)
					{
						found = true;
						break;
					}
				}

				if (!found)
					materialNames.push_back(materialName);
			}
		}

		return materialNames;
	}

	std::vector<unsigned int> Mesh::getMaterialMappings(std::list<std::string>& material_names)
	{
		std::vector<unsigned int> materialMappings;

		for (int slot = 0; slot < MODEL_SUBMESH_SLOTS; ++slot)
		{
			for (std::vector<Submesh*>::iterator it = submeshes[slot].begin(); it != submeshes[slot].end(); it++)
			{
				std::string materialName = (*it)->getMaterialName();

				unsigned int materialMap = 0;
				unsigned int materialIndex = 0;
				for (std::list<std::string>::iterator it_m = material_names.begin(); it_m != material_names.end(); it_m++)
				{
					if ((*it_m) == materialName)
					{
						materialMap = materialIndex;
						break;
					}

					materialIndex++;
				}

				size_t faces_size = (*it)->getFacesList().size();
				for (size_t i = 0; i < faces_size; i++)
					materialMappings.push_back(materialMap);
			}
		}

		return materialMappings;
	}

	void Mesh::fixVertexFormatForPC()
	{
		for (int slot = 0; slot < MODEL_SUBMESH_SLOTS; slot++)
		{
			for (std::vector<Submesh*>::iterator it = submeshes[slot].begin(); it != submeshes[slot].end(); it++)
				(*it)->fixVertexFormatForPC();
		}
	}

	void Mesh::changeVertexFormat(int format)
	{
		for (int slot = 0; slot < MODEL_SUBMESH_SLOTS; slot++)
		{
			for (std::vector<Submesh*>::iterator it = submeshes[slot].begin(); it != submeshes[slot].end(); it++)
				(*it)->changeVertexFormat(format);
		}
	}

	void Mesh::addSubmesh(Submesh* submesh, int slot)
	{
		if (slot < MODEL_SUBMESH_SLOTS)
			submeshes[slot].push_back(submesh);
	}

	void Mesh::removeSubmesh(Submesh* submesh, int slot)
	{
		if (slot < MODEL_SUBMESH_SLOTS)
		{
			for (std::vector<Submesh*>::iterator it = submeshes[slot].begin(); it != submeshes[slot].end(); ++it)
			{
				if ((*it) == submesh)
				{
					submeshes[slot].erase(it);
					break;
				}
			}
		}
	}

	unsigned int Mesh::getEstimatedMemorySize()
	{
		unsigned int total = 0;
		for (int slot = 0; slot < MODEL_SUBMESH_SLOTS; ++slot)
		{
			for (std::vector<Submesh*>::iterator it = submeshes[slot].begin(); it != submeshes[slot].end(); ++it)
				total += (*it)->getEstimatedMemorySize();
		}

		return total;
	}

	void Mesh::read(BinaryReader* reader)
	{
		size_t headerAddress = reader->getCurrentAddress();
		for (int slot = 0; slot < MODEL_SUBMESH_SLOTS; ++slot)
		{
			reader->gotoAddress(headerAddress + slot * 8);

			// submesh table
			unsigned int submeshCount = reader->readInt32();
			size_t submeshTableAddress = reader->readAddress();
			if (slot == MODEL_SUBMESH_SLOT_WATER)
			{
				if (submeshCount > 1)
				{
					printf("Water slot more than 1? Unhandled case.\n");
				}

				if (submeshCount == 0) break;

				size_t submeshCountAddress = reader->readAddress();
				size_t submeshSubtableAddress = reader->readAddress();

				reader->gotoAddress(submeshTableAddress);
				size_t address = reader->readAddress();
				waterSlotString = reader->readString();

				if ((waterSlotString != "water") && (waterSlotString != "transparentz") && (waterSlotString != "transparentz2") && (waterSlotString != "transparentafterpunch") && (waterSlotString != "opaqueblending")) {
					printf("A string different than water for the 4th slot? Report this model! %s\n", waterSlotString.c_str());
				}

				reader->gotoAddress(submeshCountAddress);
				address = reader->readAddress();
				reader->gotoAddress(address);
				unsigned int submeshCount = reader->readInt32();

				reader->gotoAddress(submeshSubtableAddress);
				address = reader->readInt32();

				for (int i = 0; i < submeshCount; ++i)
				{
					reader->gotoAddress(address + i * 4);
					size_t submeshAddress = reader->readAddress();
					reader->gotoAddress(submeshAddress);

					Submesh* submesh = new Submesh();
					submesh->read(reader);
					submesh->buildAABB();
					submeshes[slot].push_back(submesh);
				}
			}
			else
			{
				submeshes[slot].reserve(submeshCount);

				for (int i = 0; i < submeshCount; ++i)
				{
					reader->gotoAddress(submeshTableAddress + i * 4);
					size_t submeshAddress = reader->readAddress();
					reader->gotoAddress(submeshAddress);

					Submesh* submesh = new Submesh();
					submesh->read(reader);
					submesh->buildAABB();
					submeshes[slot].push_back(submesh);

					if (submesh->getBoneTable().size() > 25)
					{
						printf(std::string(std::to_string(slot) + " - " + std::to_string(i) + " - Bone Table Size: " + std::to_string(submesh->getBoneTable().size()) + "Material Name: " + submesh->getMaterialName()).c_str());
					}

					if (submesh->getVerticesSize() >= 32768)
					{
						printf(std::string(std::to_string(slot) + " - " + std::to_string(i) + " - Vertex Table Size: " + std::to_string(submesh->getVerticesSize()) + "Material Name: " + submesh->getMaterialName()).c_str());
					}
				}
			}
		}
	}

	void Mesh::write(BinaryWriter* writer)
	{
		size_t headerAddress = writer->getCurrentAddress();
		std::vector<unsigned int> slotAddresses;
		unsigned int waterSlotAddress;

		// prepare address table
		for (int slot = 0; slot < MODEL_SUBMESH_SLOTS; ++slot)
		{
			unsigned int submeshCount = submeshes[slot].size();
			if (slot == MODEL_SUBMESH_SLOT_WATER)
			{
				if (submeshCount == 0)
				{
					writer->writeNull(4);
					unsigned char filler = 0xff;
					for (int i = 0; i < 12; ++i)
						writer->writeChar(filler);

					writer->writeNull(4);
				}
				else
				{
					unsigned int waterCount = 1;
					writer->writeInt32(waterCount);
					waterSlotAddress = writer->getCurrentAddress();
					writer->writeNull(16);
				}
			}
			else
			{
				writer->writeInt32(submeshCount);
				writer->writeNull(4);
			}
		}

		// write submesh slots
		for (int slot = 0; slot < MODEL_SUBMESH_SLOTS; ++slot)
		{
			std::vector<unsigned int> submeshAddresses;
			unsigned int submeshCount = submeshes[slot].size();

			if (slot == MODEL_SUBMESH_SLOT_WATER && submeshCount)
			{
				size_t waterStringAddress = writer->getCurrentAddress();
				size_t waterTotalAddress = writer->getCurrentAddress() + 4;
				size_t waterTableAddress = writer->getCurrentAddress() + 8;

				writer->gotoAddress(waterSlotAddress);
				writer->writeAddress(waterStringAddress);
				writer->writeAddress(waterTotalAddress);
				writer->writeAddress(waterTableAddress);
				writer->gotoEnd();
				writer->writeNull(12);

				size_t subWaterStringAddress = writer->getCurrentAddress();
				writer->writeString(waterSlotString.c_str());
				writer->fixPadding();
				
				size_t subWaterTotalAddress = writer->getCurrentAddress();
				unsigned int subWaterTotal = submeshCount;
				writer->writeInt32(subWaterTotal);

				size_t subWaterTableAddress = writer->getCurrentAddress();

				writer->gotoAddress(waterStringAddress);
				writer->writeAddress(subWaterStringAddress);
				writer->gotoAddress(waterTotalAddress);
				writer->writeAddress(subWaterTotalAddress);
				writer->gotoAddress(waterTableAddress);
				writer->writeAddress(subWaterTableAddress);
				writer->gotoEnd();
			}

			slotAddresses.push_back(writer->getCurrentAddress());

			size_t submeshTableAddress = writer->getCurrentAddress();
			writer->writeNull(submeshCount * 4);

			for (int i = 0; i < submeshCount; ++i)
			{
				submeshAddresses.push_back(writer->getCurrentAddress());
				submeshes[slot][i]->write(writer);
			}

			for (int i = 0; i < submeshCount; ++i)
			{
				writer->gotoAddress(submeshTableAddress + i * 4);
				writer->writeAddress(submeshAddresses[i]);
			}

			writer->gotoEnd();
		}

		// fix address table
		for (int slot = 0; slot < MODEL_SUBMESH_SLOTS - 1; ++slot)
		{
			writer->gotoAddress(headerAddress + 4 + slot * 8);
			writer->writeInt32(slotAddresses[slot]);
		}

		writer->gotoEnd();
	}
}